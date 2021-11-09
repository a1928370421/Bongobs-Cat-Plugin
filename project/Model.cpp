/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "Model.hpp"
#include <fstream>
#include <vector>
#include <CubismModelSettingJson.hpp>
#include <Motion/CubismMotion.hpp>
#include <Physics/CubismPhysics.hpp>
#include <CubismDefaultParameterId.hpp>
#include <Rendering/OpenGL/CubismRenderer_OpenGLES2.hpp>
#include <Utils/CubismString.hpp>
#include <Id/CubismIdManager.hpp>
#include <Motion/CubismMotionQueueEntry.hpp>
#include "Define.hpp"
#include "Pal.hpp"
#include "LAppTextureManager.hpp"
#include "VtuberDelegate.hpp"

#define ALLMOTIONDISPLAY true

using namespace Live2D::Cubism::Framework;
using namespace Live2D::Cubism::Framework::DefaultParameterId;
using namespace Define;

namespace {
    csmByte* CreateBuffer(const csmChar* path, csmSizeInt* size)
    {      
        return Pal::LoadFileAsBytes(path, size);
    }

    void DeleteBuffer(csmByte* buffer, const csmChar* path = "")
    {
        Pal::ReleaseBytes(buffer);
    }
}

Model::Model()
	: CubismUserModel(),
	  _modelSetting(NULL),
	  _userTimeSeconds(0.0f),
	  randomMotion(true),
	  delayTime(5.0f),
	  isBreath(true),
	  isEyeBlink(true),
	  s_currentFrame(0.0f),
	  s_lastFrame(0.0f),
	  s_deltaTime(0.0f),
	  rButton(false),
	  lButton(false),
	  isMouseHorizontalFlip(true),
	  IsMouseVerticalFlip(true)

{
    _idParamAngleX = CubismFramework::GetIdManager()->GetId(ParamAngleX);
    _idParamAngleY = CubismFramework::GetIdManager()->GetId(ParamAngleY);
    _idParamAngleZ = CubismFramework::GetIdManager()->GetId(ParamAngleZ);
    _idParamBodyAngleX = CubismFramework::GetIdManager()->GetId(ParamBodyAngleX);
    _idParamEyeBallX = CubismFramework::GetIdManager()->GetId(ParamEyeBallX);
    _idParamEyeBallY = CubismFramework::GetIdManager()->GetId(ParamEyeBallY);
    _idParamMouseX = CubismFramework::GetIdManager()->GetId(ParaMouseX); 
    _idParamMouseY = CubismFramework::GetIdManager()->GetId(ParaMouseY);
    _idParamLeftButton = CubismFramework::GetIdManager()->GetId(ParaLeftButton);
    _idParamRightButton =CubismFramework::GetIdManager()->GetId(ParaRightButton);
}

Model::~Model()
{
    _renderBuffer.DestroyOffscreenFrame();

    ReleaseMotions();
    ReleaseExpressions();

    for (csmInt32 i = 0; i < _modelSetting->GetMotionGroupCount(); i++)
    {
        const csmChar* group = _modelSetting->GetMotionGroupName(i);
        ReleaseMotionGroup(group);
    }
    delete(_modelSetting);
}

csmBool Model::LoadAssets(const csmChar* dir, const csmChar* fileName)
{
    _modelHomeDir = dir;

    csmSizeInt size;
    const csmString path = csmString(dir) + fileName;

    csmByte* buffer = CreateBuffer(path.GetRawString(), &size);
    if (size == 0) {
	DeleteBuffer(buffer, path.GetRawString());
	return false;
    }
	    
    ICubismModelSetting* setting = new CubismModelSettingJson(buffer, size);
    DeleteBuffer(buffer, path.GetRawString());

    if (!SetupModel(setting)) {
	    return false;
    }
	    

    CreateRenderer();

    SetupTextures();

    return true;
}


Csm::csmBool Model::SetupModel(ICubismModelSetting *setting)
{
    _updating = true;
    _initialized = false;

    _modelSetting = setting;

    csmByte* buffer;
    csmSizeInt size;

    //Cubism Model
    if (strcmp(_modelSetting->GetModelFileName(), "") != 0)
    {
        csmString path = _modelSetting->GetModelFileName();
        path = _modelHomeDir + path;
	if(!Pal::IsFileExist(path.GetRawString()))
		return false;

        buffer = CreateBuffer(path.GetRawString(), &size);
	if (size != 0) {
		LoadModel(buffer, size);
		DeleteBuffer(buffer, path.GetRawString());
	} else {
		DeleteBuffer(buffer, path.GetRawString());
		return false;
	}	
    } else
	    return false;

    //Expression
    if (_modelSetting->GetExpressionCount() > 0)
    {
        const csmInt32 count = _modelSetting->GetExpressionCount();
        for (csmInt32 i = 0; i < count; i++)
        {
            csmString name = _modelSetting->GetExpressionName(i);
            csmString path = _modelSetting->GetExpressionFileName(i);
            path = _modelHomeDir + path;

            buffer = CreateBuffer(path.GetRawString(), &size);
	    if (size != 0) {
		    ACubismMotion* motion = LoadExpression(buffer, size, name.GetRawString());

		    if (_expressions[name] != NULL)
		    {
			ACubismMotion::Delete(_expressions[name]);
			_expressions[name] = NULL;
		    }
		    _expressions[name] = motion;

		    DeleteBuffer(buffer, path.GetRawString());
	    }           
        }
    }

    //Physics
    if (strcmp(_modelSetting->GetPhysicsFileName(), "") != 0)
    {
        csmString path = _modelSetting->GetPhysicsFileName();
        path = _modelHomeDir + path;
	if (!Pal::IsFileExist(path.GetRawString()))
		return false;
        buffer = CreateBuffer(path.GetRawString(), &size);
	if (size != 0) {
		LoadPhysics(buffer, size);
		DeleteBuffer(buffer, path.GetRawString());
	}
    }

    //Pose
    if (strcmp(_modelSetting->GetPoseFileName(), "") != 0)
    {
        csmString path = _modelSetting->GetPoseFileName();
        path = _modelHomeDir + path;
	if (!Pal::IsFileExist(path.GetRawString()))
		return false;
        buffer = CreateBuffer(path.GetRawString(), &size);
	if (size != 0) {
		LoadPose(buffer, size);
		DeleteBuffer(buffer, path.GetRawString());
	}
    }

    //EyeBlink
    if (_modelSetting->GetEyeBlinkParameterCount() > 0)
    {
        _eyeBlink = CubismEyeBlink::Create(_modelSetting);
    }

    //Breath
    {
        _breath = CubismBreath::Create();

        csmVector<CubismBreath::BreathParameterData> breathParameters;

        breathParameters.PushBack(CubismBreath::BreathParameterData(_idParamAngleX, 0.0f, 15.0f, 6.5345f, 0.5f));
        breathParameters.PushBack(CubismBreath::BreathParameterData(_idParamAngleY, 0.0f, 8.0f, 3.5345f, 0.5f));
        breathParameters.PushBack(CubismBreath::BreathParameterData(_idParamAngleZ, 0.0f, 10.0f, 5.5345f, 0.5f));
        breathParameters.PushBack(CubismBreath::BreathParameterData(_idParamBodyAngleX, 0.0f, 4.0f, 15.5345f, 0.5f));
        breathParameters.PushBack(CubismBreath::BreathParameterData(CubismFramework::GetIdManager()->GetId(ParamBreath), 0.5f, 0.5f, 3.2345f, 0.5f));

        _breath->SetParameters(breathParameters);
    }

    //UserData
    if (strcmp(_modelSetting->GetUserDataFile(), "") != 0)
    {
        csmString path = _modelSetting->GetUserDataFile();
        path = _modelHomeDir + path;
	if(!Pal::IsFileExist(path.GetRawString()))
		return false;
        buffer = CreateBuffer(path.GetRawString(), &size);
	if (size != 0) {
		LoadUserData(buffer, size);
		DeleteBuffer(buffer, path.GetRawString());
	}     
    }

    // EyeBlinkIds
    {
        csmInt32 eyeBlinkIdCount = _modelSetting->GetEyeBlinkParameterCount();
        for (csmInt32 i = 0; i < eyeBlinkIdCount; ++i)
        {
            _eyeBlinkIds.PushBack(_modelSetting->GetEyeBlinkParameterId(i));
        }
    }

    // LipSyncIds
    {
        csmInt32 lipSyncIdCount = _modelSetting->GetLipSyncParameterCount();
        for (csmInt32 i = 0; i < lipSyncIdCount; ++i)
        {
            _lipSyncIds.PushBack(_modelSetting->GetLipSyncParameterId(i));
        }
    }

    //Layout
    csmMap<csmString, csmFloat32> layout;
    _modelSetting->GetLayoutMap(layout);
    _modelMatrix->SetupFromLayout(layout);

    _model->SaveParameters();

    
    for (csmInt32 i = 0; i < _modelSetting->GetMotionGroupCount(); i++)
    {
        const csmChar* group = _modelSetting->GetMotionGroupName(i);
        PreloadMotionGroup(group);
    }

    _motionManager->StopAllMotions();

    _updating = false;
    _initialized = true;

    return true;
}

csmBool Model::PreloadMotionGroup(const csmChar* group)
{
    const csmInt32 count = _modelSetting->GetMotionCount(group);

    for (csmInt32 i = 0; i < count; i++)
    {
        //ex) idle_0
	csmString name = _modelSetting->GetMotionFileName(group,i); //Utils::CubismString::GetFormatedString("%s_%d", group, i);
        csmString path = _modelSetting->GetMotionFileName(group, i);
        path = _modelHomeDir + path;

        if (_debugMode)
        {
            Pal::PrintLog("[APP]load motion: %s => [%s_%d] ", path.GetRawString(), group, i);
        }

        csmByte* buffer;
        csmSizeInt size;
        buffer = CreateBuffer(path.GetRawString(), &size);
	if (size == 0) {
		DeleteBuffer(buffer, path.GetRawString());
		return false;
	}
		

        CubismMotion* tmpMotion = static_cast<CubismMotion*>(LoadMotion(buffer, size, name.GetRawString()));
        csmFloat32 fadeTime = _modelSetting->GetMotionFadeInTimeValue(group, i);
        if (fadeTime >= 0.0f)
        {
            tmpMotion->SetFadeInTime(fadeTime);
        }

        fadeTime = _modelSetting->GetMotionFadeOutTimeValue(group, i);
        if (fadeTime >= 0.0f)
        {
            tmpMotion->SetFadeOutTime(fadeTime);
        }
        tmpMotion->SetEffectIds(_eyeBlinkIds, _lipSyncIds);

        if (_motions[name] != NULL)
        {
            ACubismMotion::Delete(_motions[name]);
        }
        _motions[name] = tmpMotion;

        DeleteBuffer(buffer, path.GetRawString());
    }

    return true;
}

void Model::ReleaseMotionGroup(const csmChar* group) const
{
    const csmInt32 count = _modelSetting->GetMotionCount(group);
    for (csmInt32 i = 0; i < count; i++)
    {
        csmString voice = _modelSetting->GetMotionSoundFileName(group, i);
        if (strcmp(voice.GetRawString(), "") != 0)
        {
            csmString path = voice;
            path = _modelHomeDir + path;
        }
    }
}

/**
* @brief すべてのモーションデータの解放
*
* すべてのモーションデータを解放する。
*/
void Model::ReleaseMotions()
{
    for (csmMap<csmString, ACubismMotion*>::const_iterator iter = _motions.Begin(); iter != _motions.End(); ++iter)
    {
        ACubismMotion::Delete(iter->Second);
    }

    _motions.Clear();
}

/**
* @brief すべての表情データの解放
*
* すべての表情データを解放する。
*/
void Model::ReleaseExpressions()
{
    for (csmMap<csmString, ACubismMotion*>::const_iterator iter = _expressions.Begin(); iter != _expressions.End(); ++iter)
    {
        ACubismMotion::Delete(iter->Second);
    }

    _expressions.Clear();
}

void Model::Update(Csm::csmUint16 _id)
{
    const csmFloat32 deltaTimeSeconds =delayTime * GetDeltaTime();
    _userTimeSeconds += deltaTimeSeconds;

    // モーションによるパラメータ更新の有無
    csmBool motionUpdated = false;

    //-----------------------------------------------------------------
    _model->LoadParameters(); // 前回セーブされた状態をロード
    if (_motionManager->IsFinished()) {
	    // モーションの再生がない場合、待機モーションの中からランダムで再生する
	    if(randomMotion)
		    StartRandomMotion(PriorityIdle);	 
    } else {
	    motionUpdated = _motionManager->UpdateMotion(
		    _model, deltaTimeSeconds); // モーションを更新
    }
    _model->SaveParameters(); // 状態を保存
    //-----------------------------------------------------------------

    // まばたき
    if (!motionUpdated)
    {
        if (_eyeBlink != NULL&&isEyeBlink)
        {
            // メインモーションの更新がないとき
            _eyeBlink->UpdateParameters(_model, deltaTimeSeconds); // 目パチ
        }
    }

    if (_expressionManager != NULL)
    {
        _expressionManager->UpdateMotion(_model, deltaTimeSeconds); // 表情でパラメータ更新（相対変化）
    }



    if (isTrack) {
	    _model->AddParameterValue(_idParamAngleX,(1-mouseX) * 30); // -30から30の値を加える
	    _model->AddParameterValue(_idParamAngleY, (1-mouseY) * 30);
	    _model->AddParameterValue(_idParamAngleZ,(1 - mouseX) * (1 - mouseY) * -30);
    
	    _model->AddParameterValue(_idParamEyeBallX,(1 - mouseX) * 30); ///< パラメータID: ParamEyeBallX
	    _model->AddParameterValue(_idParamEyeBallY, (1 - mouseY) * 30); ///< パラメータID: ParamEyeBallXY	   
    }

    if (isMouseHorizontalFlip)
	  _model->AddParameterValue(_idParamMouseX, mouseX * 60 - 30);
    else 
	  _model->AddParameterValue(_idParamMouseX, (1-mouseX) * 60 - 30);

    if (IsMouseVerticalFlip)
	_model->AddParameterValue(_idParamMouseY, (1 - mouseY) * 60 - 30);
    else 
	_model->AddParameterValue(_idParamMouseY, mouseY * 60 - 30);

    if (rButton)
	    _model->AddParameterValue(_idParamRightButton, 1);
    else
	    _model->AddParameterValue(_idParamRightButton, 0);

    if (lButton)
	    _model->AddParameterValue(_idParamLeftButton, 1);
    else
	    _model->AddParameterValue(_idParamLeftButton, 0);

    // 呼吸など
    if (_breath != NULL&&isBreath)
    {
        _breath->UpdateParameters(_model, deltaTimeSeconds);
    }
    

    // 物理演算の設定
    if (_physics != NULL)
    {
        _physics->Evaluate(_model, deltaTimeSeconds);
    }

    // リップシンクの設定
    if (_lipSync)
    {
        csmFloat32 value = 0; // リアルタイムでリップシンクを行う場合、システムから音量を取得して0〜1の範囲で値を入力します。

        for (csmUint32 i = 0; i < _lipSyncIds.GetSize(); ++i)
        {
            _model->AddParameterValue(_lipSyncIds[i], value, 0.8f);
        }
    }

    // ポーズの設定
    if (_pose != NULL)
    {
        _pose->UpdateParameters(_model, deltaTimeSeconds);
    }

    _model->Update();

}

CubismMotionQueueEntryHandle Model::StartMotion(const csmChar* group, csmInt32 no, csmInt32 priority, ACubismMotion::FinishedMotionCallback onFinishedMotionHandler)
{
    if (priority == PriorityForce)
    {
        _motionManager->SetReservePriority(priority);
    }
    else if (!_motionManager->ReserveMotion(priority))
    {
        if (_debugMode)
        {
            Pal::PrintLog("[APP]can't start motion.");
        }
    }

    const csmString fileName = _modelSetting->GetMotionFileName(group, no);

    //ex) idle_0
    csmString name = Utils::CubismString::GetFormatedString("%s_%d",group, no);
    CubismMotion* motion = static_cast<CubismMotion*>(_motions[name.GetRawString()]);
    csmBool autoDelete = false;

    if (motion == NULL)
    {
        csmString path = fileName;
        path = _modelHomeDir + path;

        csmByte* buffer;
        csmSizeInt size;
        buffer = CreateBuffer(path.GetRawString(), &size);
	if (size == 0)
		goto end;
	motion = static_cast<CubismMotion*>(LoadMotion(buffer, size, NULL, onFinishedMotionHandler));
        csmFloat32 fadeTime = _modelSetting->GetMotionFadeInTimeValue(group, no);
        if (fadeTime >= 0.0f)
        {
            motion->SetFadeInTime(fadeTime);
	} else {
		motion->SetFadeInTime(1.0f);
	}

        fadeTime = _modelSetting->GetMotionFadeOutTimeValue(group, no);
        if (fadeTime >= 0.0f)
        {
            motion->SetFadeOutTime(fadeTime);
	} else {
		motion->SetFadeOutTime(1.0f);
	}

	//motion->IsLoop(true);
        motion->SetEffectIds(_eyeBlinkIds, _lipSyncIds);
        autoDelete = true; // 終了時にメモリから削除
end:
        DeleteBuffer(buffer, path.GetRawString());
    }
    else
    {
        motion->SetFinishedMotionHandler(onFinishedMotionHandler);
    }

    //voice
    csmString voice = _modelSetting->GetMotionSoundFileName(group, no);
    if (strcmp(voice.GetRawString(), "") != 0)
    {
        csmString path = voice;
        path = _modelHomeDir + path;
    }

    if (_debugMode)
    {
        Pal::PrintLog("[APP]start motion: [%s_%d]", group, no);
    }
    return  _motionManager->StartMotionPriority(motion, autoDelete, priority);
}

CubismMotionQueueEntryHandle Model::StartRandomMotion(csmInt32 priority, ACubismMotion::FinishedMotionCallback onFinishedMotionHandler)
{
	csmInt32 nog;
	if (_modelSetting->GetMotionGroupCount() == 0) {
		return InvalidMotionQueueEntryHandleValue;
	}
	nog = rand() % _modelSetting->GetMotionGroupCount();
	const csmChar *group = _modelSetting->GetMotionGroupName(nog);

	csmInt32 no;
	if (_modelSetting->GetMotionCount(group) == 0)
	{
		return InvalidMotionQueueEntryHandleValue;
	}
	no = rand() % _modelSetting->GetMotionCount(group);

	return StartMotion(group, no, priority, onFinishedMotionHandler);
}

void Model::DoDraw()
{
    if (_model == NULL)
    {
        return;
    }

    GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->DrawModel();
}

void Model::Draw(CubismMatrix44& matrix)
{
    if (_model == NULL)
    {
        return;
    }

    matrix.MultiplyByMatrix(_modelMatrix);

    GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->SetMvpMatrix(&matrix);

    DoDraw();
}

void Model::SetExpression(const csmChar* expressionID)
{
    ACubismMotion* motion = _expressions[expressionID];
    if (_debugMode)
    {
        Pal::PrintLog("[APP]expression: [%s]", expressionID);
    }

    if (motion != NULL)
    {
        _expressionManager->StartMotionPriority(motion, false, PriorityForce);
    }
    else
    {
        if (_debugMode) Pal::PrintLog("[APP]expression[%s] is null ", expressionID);
    }
}

void Model::SetRandomExpression()
{
    if (_expressions.GetSize() == 0)
    {
        return;
    }

    csmInt32 no = rand() % _expressions.GetSize();
    csmMap<csmString, ACubismMotion*>::const_iterator map_ite;
    csmInt32 i = 0;
    for (map_ite = _expressions.Begin(); map_ite != _expressions.End(); map_ite++)
    {
        if (i == no)
        {
            csmString name = (*map_ite).First;
            SetExpression(name.GetRawString());
            return;
        }
        i++;
    }
}

void Model::ReloadRenderer()
{
    DeleteRenderer();

    CreateRenderer();

    SetupTextures();
}

void Model::SetupTextures()
{
    for (csmInt32 modelTextureNumber = 0; modelTextureNumber < _modelSetting->GetTextureCount(); modelTextureNumber++)
    {
        // テクスチャ名が空文字だった場合はロード・バインド処理をスキップ
        if (strcmp(_modelSetting->GetTextureFileName(modelTextureNumber), "") == 0)
        {
            continue;
        }

        //OpenGLのテクスチャユニットにテクスチャをロードする
        csmString texturePath = _modelSetting->GetTextureFileName(modelTextureNumber);
        texturePath = _modelHomeDir + texturePath;

        LAppTextureManager::TextureInfo *texture =VtuberDelegate::GetInstance()->GetTextureManager()->CreateTextureFromPngFile(texturePath.GetRawString());
        const csmInt32 glTextueNumber = texture->id;

        //OpenGL
        GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->BindTexture(modelTextureNumber, glTextueNumber);
    }

#ifdef PREMULTIPLIED_ALPHA_ENABLE
    GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->IsPremultipliedAlpha(true);
#else
    GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->IsPremultipliedAlpha(false);
#endif

}

void Model::MotionEventFired(const csmString& eventValue)
{
    CubismLogInfo("%s is fired on Model!!", eventValue.GetRawString());
}

Csm::Rendering::CubismOffscreenFrame_OpenGLES2& Model::GetRenderBuffer()
{
    return _renderBuffer;
}

void Model::UpdataSetting(Csm::csmBool _randomMotion,
			      Csm::csmFloat32 _delayTime,
			      Csm::csmBool _isBreath,
			      Csm::csmBool _isEyeBlink,
			      Csm::csmBool _isTrack,
			      Csm::csmBool _isMouseHorizontalFlip,
			      Csm::csmBool _IsMouseVerticalFlip)
{
	randomMotion = _randomMotion;
	delayTime = _delayTime;
	isBreath = _isBreath;
	isEyeBlink = _isEyeBlink;
	isTrack = _isTrack;
	isMouseHorizontalFlip =_isMouseHorizontalFlip;
	IsMouseVerticalFlip = _IsMouseVerticalFlip;
}

void Model::UpdateMouseState(Csm::csmFloat32 _mouseX, Csm::csmFloat32 _mouseY,
			     Csm::csmBool _lButton, Csm::csmBool _rButton)
{
	mouseX = _mouseX;
	mouseY = _mouseY;
	lButton = _lButton;
	rButton = _rButton;
}

csmFloat32 Model::GetDeltaTime()
{
	return static_cast<csmFloat32>(s_deltaTime);
}

void Model::UpdateTime() {
		s_currentFrame = glfwGetTime();
		s_deltaTime = s_currentFrame - s_lastFrame;
		s_lastFrame = s_currentFrame;
}





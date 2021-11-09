/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "Live2DManager.hpp"
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Rendering/CubismRenderer.hpp>
#include "Pal.hpp"
#include "Define.hpp"
#include "VtuberDelegate.hpp"
#include "Model.hpp"
#include "View.hpp"
#include "EventManager.hpp"

using namespace Csm;
using namespace Define;
using namespace std;

namespace {
    Live2DManager* s_instance = NULL;
}

Live2DManager* Live2DManager::GetInstance()
{
    if (s_instance == NULL)
    {
        s_instance = new Live2DManager();
    }

    return s_instance;
}

void Live2DManager::ReleaseInstance()
{
    if (s_instance != NULL)
    {
        delete s_instance;
    }

    s_instance = NULL;
}

Live2DManager::Live2DManager() :
	_modelCounter(0),
	_isUseRelativeMouse (false)
{

}

Live2DManager::~Live2DManager()
{
	for (size_t i = 0; i < MAXMODELDATA; i++) {
		ReleaseAllModel(i);
	}
    
}

void Live2DManager::ReleaseAllModel(Csm::csmUint16 id)
{
    for (csmUint32 i = 0; i < _modeldata[id]._models.GetSize(); i++)
    {
		delete _modeldata[id]._models[i];
    }
    _modeldata[id]._models.Clear();
    _modeldata[id]._modelPath.Clear();
}

Model *Live2DManager::GetModel(Csm::csmUint16 id) const
{
    if (0 < _modeldata[id]._models.GetSize())
    {
	return _modeldata[id]._models[0];
    }
    return NULL;
}

void Live2DManager::OnUpdate(Csm::csmUint16 id) const
{    
    CubismMatrix44 projection;

    //update mouse position
    EventManager *_eventmanager;
    _eventmanager = VtuberDelegate::GetInstance() ->GetView() ->GetEventManager();

    int width, height;
    Pal::GetDesktopResolution(width,height);
    int px, py;
    if (_isUseRelativeMouse) {
	    VtuberDelegate::GetInstance()
		    ->GetView()
		    ->GetEventManager()
		    ->GetRelativeMouse(px, py);

	    _eventmanager->MouseEventMoved(width,height,
					   px, py);
	    px = _eventmanager->GetCenterX() - _eventmanager->GetStartX()+width/2;
	    py = _eventmanager->GetCenterY() - _eventmanager->GetStartY() + height /2;

    } else {
	    VtuberDelegate::GetInstance()
		    ->GetView()
		    ->GetEventManager()
		    ->GetCurrentMousePosition(px, py);
    }
    float mousex = static_cast<float>(px) / static_cast<float>(width);
    float mousey = static_cast<float>(py) / static_cast<float>(height);

    //update model view matrix
     CubismMatrix44 *_viewMatrix;
    _viewMatrix= VtuberDelegate::GetInstance()->GetView()->GetViewMatrix(0);

    if (_viewMatrix != NULL)
    {
	    projection.MultiplyByMatrix(_viewMatrix);	
    }

        Model* model = GetModel(id);

	if (model) {
		VtuberDelegate::GetInstance()->GetView()->PreModelDraw(*model);

		model->UpdateTime();
		model->UpdataSetting(_randomMotion , _delayTime,_isBreath , _isEyeBlink ,_isTrack ,_isMouseHorizontalFlip,_IsMouseVerticalFlip);
		model->UpdateMouseState(mousex,mousey,_eventmanager->GetLeftButton(),_eventmanager->GetRightButton());
		model->Update(id);
		model->Draw(projection);

		VtuberDelegate::GetInstance()->GetView()->PostModelDraw(*model);
	}
}

Csm::csmInt32 Live2DManager::ChangeScene(const Csm::csmChar *_modelPath)
{
     int id = _modelCounter;
     if (strcmp(_modelPath, "") == 0)
	return -1;

    if (strcmp(_modeldata[id]._modelPath.GetRawString(),
		_modelPath) == 0)
	return id;

    //E:/obspl/build/rundir/Debug/bin/64bit/Resources/l2d03.u/l2d03.u.model3.json
    string modelFilePath = std::string(_modelPath);
    size_t pos = modelFilePath.rfind("/");
    string modelPath = modelFilePath.substr(0,pos+1);
    string modelJsonName = modelFilePath.substr(pos+1, modelFilePath.size()-pos-1);

    if (strcmp(modelJsonName.c_str(), "") == 0)
	    return -1;

    if (VtuberDelegate::GetInstance()->isLoadResource(id))
	    ReleaseAllModel(id);

    _modeldata[id]._models.PushBack(new Model());

    if (_modeldata[id]._models[0]->LoadAssets(modelPath.c_str(),
					      modelJsonName.c_str())) {
	    _modelCounter++;
	    _modeldata[id]._modelPath = _modelPath;
    }
    else {
	    _modeldata[id]._models.Clear();
	    return -1;
    }

    /*
     * モデル半透明表示を行うサンプルを提示する。
     * ここでUSE_RENDER_TARGET、USE_MODEL_RENDER_TARGETが定義されている場合
     * 別のレンダリングターゲットにモデルを描画し、描画結果をテクスチャとして別のスプライトに張り付ける。
     */
    {
#if defined(USE_RENDER_TARGET)
        // Viewの持つターゲットに描画を行う場合、こちらを選択
        SelectTarget useRenderTarget = SelectTarget_ViewFrameBuffer;
#elif defined(USE_MODEL_RENDER_TARGET)
        // 各Modelの持つターゲットに描画を行う場合、こちらを選択
        SelectTarget useRenderTarget = SelectTarget_ModelFrameBuffer;
#else
        // デフォルトのメインフレームバッファへレンダリングする(通常)
        SelectTarget useRenderTarget = SelectTarget_None;
#endif

#if defined(USE_RENDER_TARGET) || defined(USE_MODEL_RENDER_TARGET)
        // モデル個別にαを付けるサンプルとして、もう1体モデルを作成し、少し位置をずらす
        _models.PushBack(new Model());
        _models[1]->LoadAssets(modelPath.c_str(), modelJsonName.c_str());
        _models[1]->GetModelMatrix()->TranslateX(0.2f);
#endif

        VtuberDelegate::GetInstance()->GetView()->SwitchRenderingTarget(
		useRenderTarget, id);

        // 別レンダリング先を選択した際の背景クリア色
        float clearColor[3] = { 1.0f, 1.0f, 1.0f };
	VtuberDelegate::GetInstance()->GetView()->SetRenderTargetClearColor(
		clearColor[0], clearColor[1], clearColor[2]);

	return id;
    }
}

void Live2DManager::ChangeMouseMovement(Csm::csmBool _mouse) {
	if (_isUseRelativeMouse != _mouse) {
		_isUseRelativeMouse = _mouse;
		if (_mouse) {

			EventManager *_eventmanager;
			_eventmanager = VtuberDelegate::GetInstance()
						->GetView()
						->GetEventManager();

			int px, py;
			VtuberDelegate::GetInstance()
				->GetView()
				->GetEventManager()
				->GetRelativeMouse(px, py);
			_eventmanager->MouseEventBegan(px,py);
		}
		
	}

}

void Live2DManager::UpdateModelSetting(
	Csm::csmBool randomMotion,
	Csm::csmFloat32 delayTime, Csm::csmBool isBreath,
	Csm::csmBool isEyeBlink, Csm::csmBool isTrack,
	Csm::csmBool isMouseHorizontalFlip, Csm::csmBool IsMouseVerticalFlip)
{
	_randomMotion = randomMotion;
	_delayTime= delayTime;
	_isBreath = isBreath;
	_isEyeBlink = isEyeBlink;
	_isTrack = isTrack;
	_isMouseHorizontalFlip = isMouseHorizontalFlip;
	_IsMouseVerticalFlip = IsMouseVerticalFlip;
}







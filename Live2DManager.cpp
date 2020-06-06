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

Live2DManager::Live2DManager()
{

}

Live2DManager::~Live2DManager()
{
	for (size_t i = 0; i < 16; i++) {
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

    Csm::csmUint16 viewerCount =VtuberDelegate::GetInstance()->GetView()->GetTotalViewer();
    CubismMatrix44 *_viewMatrix;   
    _viewMatrix= VtuberDelegate::GetInstance()->GetView()->GetViewMatrix(id);

    if (_viewMatrix != NULL)
    {
	    projection.MultiplyByMatrix(_viewMatrix);	
    }

        Model* model = GetModel(id);

	if (model) {
		VtuberDelegate::GetInstance()->GetView()->PreModelDraw(*model,id);

		model->UpdateTime();
		model->UpdataSetting(
			VtuberDelegate::GetInstance()->GetRandomMotion(id),
			VtuberDelegate::GetInstance()->GetDelayTime(id),
			VtuberDelegate::GetInstance()->GetBreath(id),
			VtuberDelegate::GetInstance()->GetEyeBlink(id),
			VtuberDelegate::GetInstance()->GetView()->GetMouseX(),
			VtuberDelegate::GetInstance()->GetView()->GetMouseY(),
			VtuberDelegate::GetInstance()->GetView()->GetLButton(),
			VtuberDelegate::GetInstance()->GetView()->GetRButton(),
			VtuberDelegate::GetInstance()->GetTrack(id));
		model->Update(id);
		model->Draw(projection);

		VtuberDelegate::GetInstance()->GetView()->PostModelDraw(*model,id);
	}
}

Csm::csmBool Live2DManager::ChangeScene(const Csm::csmChar *_modelPath,
				Csm::csmInt16 _id)
{
     if (strcmp(_modelPath, "") == 0)
	return true;

    if (strcmp(_modeldata[_id]._modelPath.GetRawString(), _modelPath) == 0)
	 return true;

    //E:/obspl/build/rundir/Debug/bin/64bit/Resources/l2d03.u/l2d03.u.model3.json
    string modelFilePath = std::string(_modelPath);
    size_t pos = modelFilePath.rfind("/");
    string modelPath = modelFilePath.substr(0,pos+1);
    string modelJsonName = modelFilePath.substr(pos+1, modelFilePath.size()-pos-1);

    if (VtuberDelegate::GetInstance()->isLoadResource(_id))
		ReleaseAllModel(_id);

    _modeldata[_id]._models.PushBack(new Model());

    if (_modeldata[_id]._models[0]->LoadAssets(modelPath.c_str(),modelJsonName.c_str())) {
	    _modeldata[_id]._modelPath = _modelPath;
    }
    else {
	    _modeldata[_id]._models.Clear();
	    return false;
    }

	    
    /*
     * モデル半透明表示を行うサンプルを提示する。
     * ここでUSE_RENDER_TARGET、USE_MODEL_RENDER_TARGETが定義されている場合
     * 別のレンダリングターゲットにモデルを描画し、描画結果をテクスチャとして別のスプライトに張り付ける。
     */
    {
#if defined(USE_RENDER_TARGET)
        // Viewの持つターゲットに描画を行う場合、こちらを選択
        View::SelectTarget useRenderTarget = View::SelectTarget_ViewFrameBuffer;
#elif defined(USE_MODEL_RENDER_TARGET)
        // 各Modelの持つターゲットに描画を行う場合、こちらを選択
        View::SelectTarget useRenderTarget = View::SelectTarget_ModelFrameBuffer;
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
		useRenderTarget, _id);

        // 別レンダリング先を選択した際の背景クリア色
        float clearColor[3] = { 1.0f, 1.0f, 1.0f };
	VtuberDelegate::GetInstance()->GetView()->SetRenderTargetClearColor(
		clearColor[0], clearColor[1], clearColor[2]);

	return true;
    }
}





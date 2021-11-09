#include "View.hpp"
#include <math.h>
#include <string>
#include "Pal.hpp"
#include "VtuberDelegate.hpp"
#include "Live2DManager.hpp"
#include "LAppTextureManager.hpp"
#include "Define.hpp"
#include "Model.hpp"
#include "Sprite.hpp"
#include "EventManager.hpp"
#include "InfoReader.hpp"

using namespace std;
using namespace Define;

View::View() : _programId(0), _mod(0), isUseLive2d(true), isUseMask(false)
{
    _clearColor[0] = 1.0f;
    _clearColor[1] = 1.0f;
    _clearColor[2] = 1.0f;
    _clearColor[3] = 0.0f;

    eventManager = new EventManager();
    

    const string resourcesPath = ResourcesPath;
    const string modePath = resourcesPath + ModePath;
    const string maskPath = resourcesPath + MaskPath;

    _infoReader = new InfoReader();
    _infoReader->InitFaceFromConfig(maskPath.c_str());
    _infoReader->InitFromConfig(modePath.c_str());
}

View::~View()
{
	_renderBuffer.DestroyOffscreenFrame();
	delete eventManager;
	delete _infoReader;
}

void View::Initialize(int id)
{
    int width, height;
    width = VtuberDelegate::GetInstance()->getBufferWidth(id);
    height = VtuberDelegate::GetInstance()->getBufferHeight(id);

    if(width==0 || height==0)
    {
        return;
    }

    float ratio = static_cast<float>(height) / static_cast<float>(width);
    float left = ViewLogicalLeft;
    float right = ViewLogicalRight;
    float bottom = -ratio;
    float top = ratio;

    _viewData[id]._viewMatrix = new CubismViewMatrix();
    _viewData[id]._deviceToScreen = new CubismMatrix44();

    _viewData[id]._viewMatrix->SetScreenRect(
	    left, right, bottom,
	    top); // デバイスに対応する画面の範囲。 Xの左端, Xの右端, Yの下端, Yの上端

    float screenW = fabsf(left - right);
    _viewData[id]._deviceToScreen->LoadIdentity(); // サイズが変わった際などリセット必須
    _viewData[id]._deviceToScreen->ScaleRelative(screenW / width,
						 -screenW / width);
    _viewData[id]._deviceToScreen->TranslateRelative(-width ,-height);

    // 表示範囲の設定
    _viewData[id]._viewMatrix->SetMaxScale(ViewMaxScale); // 限界拡大率
    _viewData[id]._viewMatrix->SetMinScale(ViewMinScale); // 限界縮小率

    // 表示できる最大範囲
    _viewData[id]._viewMatrix->SetMaxScreenRect(
        ViewLogicalMaxLeft,
        ViewLogicalMaxRight,
        ViewLogicalMaxBottom,
        ViewLogicalMaxTop
    );
}

void View::Release(int id)
{
	if (_viewData[id]._viewMatrix)
		delete _viewData[id]._viewMatrix;
	if (_viewData[id]._deviceToScreen)
		delete _viewData[id]._deviceToScreen;

	for (int i = 0; i < _viewData[id]._faceCount; i++) {
		delete _viewData[id]._face[i];
	}
}

void View::UpdataViewData(int id) {

    int width, height;
    width = VtuberDelegate::GetInstance()->getBufferWidth(id);
    height = VtuberDelegate::GetInstance()->getBufferHeight(id);
    double x, y, scale;
    x = VtuberDelegate::GetInstance()->GetX(id);
    y = VtuberDelegate::GetInstance()->GetY(id);
    scale = VtuberDelegate::GetInstance()->getScale(id);
    double _x = -static_cast<float>(RenderTargetWidth - width) /static_cast<float>(RenderTargetWidth);
    double _y = -static_cast<float>(RenderTargetHeight-height) /static_cast<float>(RenderTargetHeight);
    int a = id;
    if (_viewData[id]._viewMatrix) {
	_viewData[id]._viewMatrix->Scale(0.615*scale, scale*1.1);
	_viewData[id]._viewMatrix->Translate(x + _x, y+_y);
    } else {
	    Initialize(id);
	    _viewData[id]._viewMatrix->Scale(0.615 * scale, scale * 1.1);
	    _viewData[id]._viewMatrix->Translate(x + _x, y + _y);
    }

    //SwitchRenderingTarget(SelectTarget_ViewFrameBuffer,id);
}

int View::TranslateKey(int key, int id)
{
	for (int j = 0; j < _viewData[id]._mode[_mod]._keysCount; j++) {
		if (strcmp(_infoReader->_modeInfo[_mod].KeyUse[j], KeyDefine[key]) == 0)
			return j;
	}
	return -1;
}

int View::TranslateKey2(int key, int id)
{
	for (int j = 0; j < _viewData[id]._faceCount; j++) {
		if (strcmp(_infoReader->_faceInfo[0].HotKey[j], KeyDefine[key]) == 0)
			return j;
	}
	return -1;
}

void View::RenderBackgroud(int id)
{
	
	//render backgroud
	if (isUseLive2d && _viewData[id]._mode[_mod]._haseModel) {

		if (_viewData[id]._mode[_mod]._back)
			_viewData[id]._mode[_mod]._back->Render(id);

	} else {
		if (_viewData[id]._mode[_mod]._catback)
			_viewData[id]._mode[_mod]._catback->Render(id);
	}
}

void View::RenderCat(int id) {
	Live2DManager *Live2DManager = Live2DManager::GetInstance();
	if (isUseLive2d && _viewData[id]._mode[_mod]._haseModel) {
		Live2DManager->OnUpdate(_viewData[id]._mode[_mod]._modelId);
	}
}

void View::RenderKeys(int id)
{

	for (int i = 0; i < KeyAmount; i++) {
		if (eventManager->GetKeySignal(i)) {
			int key = TranslateKey(i, id);
			if (key != -1) {
				if (_viewData[id]._mode[_mod]._keys[key])
					_viewData[id]._mode[_mod]._keys[key]->Render(id);
			}
		}
	}
}

bool View::RenderLeftHands(int id) {

	Live2DManager *Live2DManager = Live2DManager::GetInstance();
	bool isUp = true;
	if (_viewData[id]._mode[_mod]._leftHandsCount > 0) {	
		for (int i = 0; i < KeyAmount; i++) {
			if (eventManager->GetKeySignal(i)) {

				int key = TranslateKey(i, id);
				if (key != -1 && key < _viewData[id]._mode[_mod]._leftHandsCount) {

					if (_viewData[id]._mode[_mod] ._leftHands[key])
						_viewData[id]._mode[_mod]._leftHands[key]->Render(id);

					isUp = false;
					break; //cat only have one right hand
				}
			}
		}

	} else if (_viewData[id]._mode[_mod]._hasLeftHandModel) {
		if (!isUseLive2d) {	
			Live2DManager->OnUpdate(_viewData[id]._mode[_mod]._leftHandModelId);
		}
		isUp = false;		
	} else if (isUseLive2d)
		isUp = false;

	return isUp;
}

bool View::RenderRightHands(int id) {

	Live2DManager *Live2DManager = Live2DManager::GetInstance();
	bool isUp = true;
	if (_viewData[id]._mode[_mod]._rightHandsCount > 0) {
		

		for (int i = 0; i < KeyAmount; i++) {
			if (eventManager->GetKeySignal(i)) {
				int key = TranslateKey(i, id) - _viewData[id]._mode[_mod]._leftHandsCount;

				if (key >= 0 && key < _viewData[id]._mode[_mod]._rightHandsCount) {

					if (_viewData[id]._mode[_mod]._rightHands[key])
						_viewData[id]._mode[_mod]._rightHands[key]->Render(id);
					isUp = false;
					break; //cat only have one right hand
				}
			}
		}			
	} else if (_viewData[id]._mode[_mod]._hasRightHandModel) {	
		if (!isUseLive2d) {	
			Live2DManager->OnUpdate(_viewData[id]._mode[_mod]._rightHandModelId);
		}
		isUp = false;	
	} else if (isUseLive2d)
		isUp = false;

	return isUp;
}

void View::RenderUphands(bool leftup, bool righttup, int id) {
	if (leftup) {
		if (_viewData[id]._mode[_mod]._leftHandUp)
			_viewData[id]._mode[_mod]._leftHandUp->Render(id);
	}
	if (righttup) {
		if (_viewData[id]._mode[_mod]._rightHandUp)
			_viewData[id]._mode[_mod]._rightHandUp->Render(id);
	}

}

void View::ReanderMask(int id) {
	for (int i = 0; i < KeyAmount; i++) {
		if (eventManager->GetKeySignal(i)) {
			int key = TranslateKey2(i, id);
			if (key >= 0 &&key < _viewData[id]._faceCount) {
				_viewData[id]._curentface = key;
				break; 
			}
		}
	}
	if (isUseMask&&_viewData[id]._face[_viewData[id]._curentface])
		  _viewData[id]._face[_viewData[id]._curentface]->Render(id);
}

void View::ChangeMode(int mod, int id) {}

void View::Render(int id)
{
	
	UpdataViewData(id);

	//render backgroud & cat model
	RenderBackgroud(id);

	//render key
	RenderKeys(id);

	RenderCat(id);

	//render left hand & left hand model
	bool leftup = RenderLeftHands(id);

	//render right hand & right hand model
	bool righttup = RenderRightHands(id);

	//render mask
	ReanderMask(id);

	//render Uphand
	RenderUphands(leftup, righttup,id);

}

void View::InitializeSpirite(int id) {

	_programId = VtuberDelegate::GetInstance()->CreateShader();
	Live2DManager *Live2DManager = Live2DManager::GetInstance();

	int width, height;
	width = VtuberDelegate::GetInstance()->getBufferWidth(id);
	height = VtuberDelegate::GetInstance()->getBufferHeight(id);
	LAppTextureManager *textureManager =VtuberDelegate::GetInstance()->GetTextureManager();

	const string resourcesPath = ResourcesPath;
	const string modePath = resourcesPath+ModePath;
	const string maskPath = resourcesPath + MaskPath;

	_viewData[id]._faceCount = _infoReader->_faceInfo->Facecount;
	_viewData[id]._curentface = 0;
	for (int _facecount = 0; _facecount < _infoReader->_faceInfo->Facecount; _facecount++) {

			string imageName =_infoReader->_faceInfo->FaceImages[_facecount];
			LAppTextureManager::TextureInfo *FaceTexture =
				textureManager->CreateTextureFromPngFile(
					maskPath + imageName);
			float x = width * 0.5f;
			float y = height * 0.5f;
			float fWidth = static_cast<float>(width * 0.5);
			float fHeight = static_cast<float>(height * 0.5);
			_viewData[id]._face[_facecount] =new Sprite(x, y, fWidth, fHeight,FaceTexture->id, _programId);
	}

	
	_viewData[id]._modecount = _infoReader->ModeCount;
	for (int _modelCount = 0; _modelCount < _infoReader->ModeCount; _modelCount++) {
		MODE_INFO *_modeinfo = _infoReader->_modeInfo;

		string targetPath =modePath + _infoReader->ModePath[_modelCount] + "/";

		/***************************** backgroud ***************************************/
		if (strcmp(_modeinfo[_modelCount].BackgroundImageName, "") !=
		    0) {
			LAppTextureManager::TextureInfo *backgroundTexture =
				textureManager->CreateTextureFromPngFile(
					targetPath +
					_modeinfo[_modelCount]
						.BackgroundImageName);
			float x = width * 0.5;
			float y = height * 0.5;
			float fWidth = static_cast<float>(width * 0.5);
			float fHeight = static_cast<float>(height * 0.5);
			_viewData[id]._mode[_modelCount]._back =
				new Sprite(x, y, fWidth, fHeight,
					   backgroundTexture->id, _programId);
		} else
			_viewData[id]._mode[_modelCount]._back = NULL;

		if (strcmp(_modeinfo[_modelCount].CatBackgroundImageName, "") !=0) {
			LAppTextureManager::TextureInfo *catbackgroundTexture =
			textureManager->CreateTextureFromPngFile(
				targetPath +
				_modeinfo[_modelCount].CatBackgroundImageName);
			float x = width * 0.5;
			float y = height * 0.5;
			float fWidth = static_cast<float>(width * 0.5);
			float fHeight = static_cast<float>(height * 0.5);
			_viewData[id]._mode[_modelCount]._catback =
				new Sprite(x, y, fWidth, fHeight,catbackgroundTexture->id,_programId);
		} else
			_viewData[id]._mode[_modelCount]._catback=NULL;

		/***************************** keys ***************************************/
		string _keysImagePath =targetPath + _modeinfo[_modelCount].KeysImagePath + "/";
		_viewData[id]._mode[_modelCount]._keysCount =_modeinfo[_modelCount].KeysCount;

		//load keyboard
		for (int i = 0; i < _viewData[id]._mode[_modelCount]._keysCount; i++) {
			if (!_modeinfo[_modelCount].KeysImageName[i])
				break;

			string imageName =_modeinfo[_modelCount].KeysImageName[i];
			LAppTextureManager::TextureInfo *KeysTexture =
				textureManager->CreateTextureFromPngFile(
					_keysImagePath + imageName);
			float x = width * 0.5f;
			float y = height * 0.5f;
			float fWidth = static_cast<float>(width * 0.5);
			float fHeight = static_cast<float>(height * 0.5);
			_viewData[id]._mode[_modelCount]._keys[i] =
				new Sprite(x, y, fWidth, fHeight,KeysTexture->id, _programId);
		}

		/***************************** lefthand ***************************************/

		if (strcmp(_modeinfo[_modelCount].LeftHandImagePath, "") != 0) {

			string _leftHandsImagePath =targetPath + _modeinfo[_modelCount].LeftHandImagePath +"/";
			if (strcmp(_modeinfo[_modelCount].LeftHandUpImageName, "") != 0){
				//load left hand up
				LAppTextureManager::TextureInfo *LeftHandUpTexture =
					textureManager->CreateTextureFromPngFile(
						_leftHandsImagePath +
						_modeinfo[_modelCount].LeftHandUpImageName);
				float x = width * 0.5f;
				float y = height * 0.5f;
				float fWidth = static_cast<float>(width * 0.5);
				float fHeight = static_cast<float>(height * 0.5);
				_viewData[id]._mode[_modelCount]._leftHandUp =
					new Sprite(x, y, fWidth, fHeight, LeftHandUpTexture->id,_programId);
			} else
				_viewData[id]._mode[_modelCount]._leftHandUp = NULL;
		
			//load left hands
			_viewData[id]._mode[_modelCount]._leftHandsCount =_modeinfo[_modelCount].ModelLeftHandCount;
			for (int i = 0;i < _viewData[id]._mode[_modelCount]._leftHandsCount;i++) {
				string imageName =_modeinfo[_modelCount].LeftHandImageName[i];
				LAppTextureManager::TextureInfo *HandsTexture =
					textureManager->CreateTextureFromPngFile(
						_leftHandsImagePath +imageName);
				float x = width * 0.5f;
				float y = height * 0.5f;
				float fWidth = static_cast<float>(width * 0.5);
				float fHeight = static_cast<float>(height * 0.5);
				_viewData[id]._mode[_modelCount]._leftHands[i]=
					new Sprite(x, y, fWidth, fHeight,HandsTexture->id, _programId);
			}			

		} else {
			_viewData[id]._mode[_modelCount]._leftHandUp = NULL;
			_viewData[id]._mode[_modelCount]._leftHandsCount = 0;
		}
			
		/***************************** righthands ***************************************/

		if (strcmp(_modeinfo[_modelCount].RightHandImagePath ,"")!=0) {

			string _rightHandsImagePath =targetPath + _modeinfo[_modelCount].RightHandImagePath +"/";
			if (strcmp(_modeinfo[_modelCount].RightHandUpImageName, "") != 0)
			{

				//load right hand up
				LAppTextureManager::TextureInfo *RightHandUpTexture =
					textureManager->CreateTextureFromPngFile(
						_rightHandsImagePath +
						_modeinfo[_modelCount].RightHandUpImageName);
				float x = width * 0.5f;
				float y = height * 0.5f;
				float fWidth = static_cast<float>(width * 0.5);
				float fHeight = static_cast<float>(height * 0.5);
				_viewData[id]._mode[_modelCount]._rightHandUp =
					new Sprite(x, y, fWidth, fHeight,RightHandUpTexture->id, _programId);
			} else 
				_viewData[id]._mode[_modelCount]._rightHandUp =NULL;
			
		

			//load right hands
			_viewData[id]._mode[_modelCount]._rightHandsCount = _modeinfo[_modelCount].ModelRightHandCount;
			for (int i = 0; i < _viewData[id]._mode[_modelCount]._rightHandsCount; i++) {
				string imageName = _modeinfo[_modelCount].RightHandImageName[i];
				LAppTextureManager::TextureInfo *HandsTexture =
					textureManager->CreateTextureFromPngFile(
						_rightHandsImagePath + imageName);
				float x = width * 0.5f;
				float y = height * 0.5f;
				float fWidth = static_cast<float>(width * 0.5);
				float fHeight = static_cast<float>(height * 0.5);
				_viewData[id]._mode[_modelCount]._rightHands[i] =
					new Sprite(x, y, fWidth, fHeight, HandsTexture->id, _programId);
			}


		} else {
			_viewData[id]._mode[_modelCount]._rightHandUp = NULL;
			_viewData[id]._mode[_modelCount]._rightHandsCount = 0;
		}
			
		
	}

}

void View::InitializeModel(int id)
{
	MODE_INFO *_modeinfo = _infoReader->_modeInfo;
	Live2DManager *Live2DManager = Live2DManager::GetInstance();

	const string resourcesPath = ResourcesPath;
	const string modePath = resourcesPath + ModePath;
	for (int _modelCount = 0; _modelCount < _infoReader->ModeCount;_modelCount++) {

		string targetPath =modePath + _infoReader->ModePath[_modelCount] + "/";

		_viewData[id]._mode[_modelCount]._haseModel =_modeinfo[_modelCount].HasModel;
		string _catModelPath =targetPath + _modeinfo[_modelCount].CatModelPath;
		string _catModelName = Pal::GetModelName(_catModelPath.c_str());
		_viewData[id]._mode[_modelCount]._modelId =Live2DManager->ChangeScene((_catModelPath + "/" + _catModelName).c_str());

		_viewData[id]._mode[_modelCount]._hasLeftHandModel =_modeinfo[_modelCount].ModelHasLeftHandModel;
		string _LeftHandModelPath =targetPath +_modeinfo[_modelCount].ModelLeftHandModelPath;
		string _LeftHandModelName =Pal::GetModelName(_LeftHandModelPath.c_str());
		_viewData[id]._mode[_modelCount]._leftHandModelId =Live2DManager->ChangeScene((_LeftHandModelPath + "/" + _LeftHandModelName).c_str());

		_viewData[id]._mode[_modelCount]._hasRightHandModel =_modeinfo[_modelCount].ModelHasRightHandModel;
		string _RightHandModelPath =targetPath +_modeinfo[_modelCount].ModelRightHandModelPath;
		string _RightHandModelName =Pal::GetModelName(_RightHandModelPath.c_str());
		_viewData[id]._mode[_modelCount]._rightHandModelId =Live2DManager->ChangeScene((_RightHandModelPath + "/" +_RightHandModelName).c_str());

	}
}

void View::PreModelDraw(Model& refModel)
{
    Csm::Rendering::CubismOffscreenFrame_OpenGLES2* useTarget = NULL;

    if (target != SelectTarget_None)
    {
	    useTarget = (target == SelectTarget_ViewFrameBuffer)
				? &_renderBuffer
				: &refModel.GetRenderBuffer();

        if (!useTarget->IsValid())
        {
            int width, height;
	    width = VtuberDelegate::GetInstance()->getBufferWidth(0);
	    height = VtuberDelegate::GetInstance()->getBufferHeight(0);
            if (width != 0 && height != 0)
            {
                
                useTarget->CreateOffscreenFrame(static_cast<csmUint32>(width), static_cast<csmUint32>(height));
            }
        }

        useTarget->BeginDraw();
        useTarget->Clear(_clearColor[0], _clearColor[1], _clearColor[2], _clearColor[3]); 
    }
}

void View::PostModelDraw(Model& refModel)
{
    Csm::Rendering::CubismOffscreenFrame_OpenGLES2* useTarget = NULL;

    if (target != SelectTarget_None)
    {
	    useTarget = (target == SelectTarget_ViewFrameBuffer)
				? &_renderBuffer
				: &refModel.GetRenderBuffer();
     
        useTarget->EndDraw();
    } 
}

void View::SwitchRenderingTarget(SelectTarget targetType,int id)
{
    target = targetType;
}

void View::SetRenderTargetClearColor(float r, float g, float b)
{
    _clearColor[0] = r;
    _clearColor[1] = g;
    _clearColor[2] = b;
}

Csm::CubismViewMatrix * View::GetViewMatrix(int id)
{
	return _viewData[id]._viewMatrix;
}

Csm::CubismMatrix44 *View::GetDeviceToScreenMatrix(int id)
{
	return _viewData[id]._deviceToScreen;
}

InfoReader *View::GetInfoReader()
{
	return _infoReader;
}

float View::TransformViewX(float deviceX,int id) const
{
	float screenX = _viewData[id]._deviceToScreen->TransformX(deviceX); // 論理座標変換した座標を取得。
	return _viewData[id]._viewMatrix->InvertTransformX(screenX); // 拡大、縮小、移動後の値。
}

float View::TransformViewY(float deviceY,int id) const
{
	float screenY = _viewData[id]._deviceToScreen->TransformY(deviceY); // 論理座標変換した座標を取得。
	return _viewData[id]._viewMatrix->InvertTransformY(screenY); // 拡大、縮小、移動後の値。
}

EventManager *View::GetEventManager()
{
	return eventManager;
}

void View::OnMouseMoved(float pointX, float pointY,int id) const
{
	float viewX =this->TransformViewX(eventManager->GetX(), id);
	float viewY =this->TransformViewY(eventManager->GetY(), id);
	eventManager->MouseEventMoved(pointX, pointY);
}

void View::Update( bool _isLive2D, bool _isUseMask) {
	
	isUseLive2d = _isLive2D;
	isUseMask = _isUseMask;
}

void View::setMod(uint16_t i)
{
	_mod = i;
}



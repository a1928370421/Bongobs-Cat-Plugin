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

using namespace std;
using namespace Define;

View::View() : _programId(0), dataCount(0), _mod(0), isUseLive2d(true)
{
    _clearColor[0] = 1.0f;
    _clearColor[1] = 1.0f;
    _clearColor[2] = 1.0f;
    _clearColor[3] = 0.0f;
}

View::~View()
{
    _renderBuffer.DestroyOffscreenFrame();
	delete eventManager;
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

    dataCount++;

	eventManager = new EventManager();
}

void View::Release(int id) {
	delete _viewData[id]._viewMatrix;
	delete _viewData[id]._deviceToScreen;

	/*
	delete _viewData[id]._back;
	delete _viewData[id]._leftHandUp;	
	for (int i = 0; i < 15; i++) {
		delete _viewData[id]._hands[i];
		delete _viewData[id]._Keys[i];
	}*/
	dataCount--;
}

void View::UpdataViewData(int id) {

    int width, height;
    width = VtuberDelegate::GetInstance()->getBufferWidth(id);
    height = VtuberDelegate::GetInstance()->getBufferHeight(id);
    double x, y, scale;
    x = VtuberDelegate::GetInstance()->GetX(id);
    y = VtuberDelegate::GetInstance()->GetY(id);
    scale = VtuberDelegate::GetInstance()->getScale(id);

    _viewData[id]._viewMatrix->Scale(0.65*scale, scale);
    double _x = -static_cast<float>(RenderTargetWidth - width) /static_cast<float>(RenderTargetWidth);
    double _y = -static_cast<float>(RenderTargetHeight-height) /static_cast<float>(RenderTargetHeight);
    _viewData[id]._viewMatrix->Translate(x + _x, y+_y);
}

int View::TranslateKey(int key, int id)
{
	for (int i = 0; i < KeyAmount; i++) {
		for (int j = 0; j < _viewData[id]._mode[_mod]._keysCount; j++) {
			if (_mod == 0) {
				if (strcmp(Mod1KeyUse[j], KeyDefine[key]) == 0)
					return j;
			} else {
				if (strcmp(Mod2KeyUse[j],KeyDefine[key]) == 0)
					return j;
			}
		}
	}
	return -1;
}

void View::Render(int id)
{
	Live2DManager* Live2DManager = Live2DManager::GetInstance();

	UpdataViewData(id);

	//render cat
	if (isUseLive2d) {
		Live2DManager->OnUpdate(id);
		_viewData[id]._mode[_mod]._back->Render(id);
	} else {
		_viewData[id]._mode[_mod]._catback->Render(id);
	}

	//render key
	for (int i = 0; i < KeyAmount; i++) {
		if (eventManager->GetKeySignal(i)) {
			int key = TranslateKey(i, id);
			if (key != -1) {
				_viewData[id]._mode[_mod]._keys[key]->Render(id);
			}	
		}
	}

	//render left hand
	bool isUp = true;
	for (int i = 0; i < KeyAmount; i++) {
		if (eventManager->GetKeySignal(i)) {
			int key = TranslateKey(i, id);
			if (key != -1 && key < _viewData[id]._mode[_mod]._leftHandsCount) {
				_viewData[id]._mode[_mod]._leftHands[key]->Render(id);
				isUp = false;
				break; //cat only have one right hand
			}	
		}			
	}
	if (isUp)
		_viewData[id]._mode[_mod]._leftHandUp->Render(id);

	//render right hand
	if (_viewData[id]._mode[_mod].isUseRightHandModel) {
		UpdataViewData(1);
		Live2DManager->OnUpdate(1);
	} else {
		bool isUp = true;
		for (int i = 0; i < KeyAmount; i++) {
			if (eventManager->GetKeySignal(i)) {
				int key = TranslateKey(i, id);
				key -= _viewData[id]._mode[_mod]._leftHandsCount;
				if (key > -1) {
					_viewData[id]._mode[_mod]._rightHands[key]->Render(id);
					isUp = false;
					break; //cat only have one right hand
				}				
			}
		}
		if (isUp)
			_viewData[id]._mode[_mod]._rightHandUp->Render(id);
	}

	
}

void View::InitializeSpirite(int id) {

	_programId = VtuberDelegate::GetInstance()->CreateShader();

	int width, height;
	width = VtuberDelegate::GetInstance()->getBufferWidth(id);
	height = VtuberDelegate::GetInstance()->getBufferHeight(id);
	LAppTextureManager *textureManager =VtuberDelegate::GetInstance()->GetTextureManager();

	const string resourcesPath = ResourcesPath;

	const string backImageName = BackImageName;
	const string catBackImageName = CatBackImageName;
	const string leftHandImagePath = LeftHandImagePath;
	const string leftHandUpImageName = LeftHandUpImageName;
	const string rightHandImagePath = RightHandImagePath;
	const string rightHandUpImageName = RightHandUpImageName;
	const string keyboardimagepath = KeyboardImagePath;

	for (int _modelCount = 0; _modelCount < ModeCount; _modelCount++) {

		string targetPath = resourcesPath + ModeImagePath+ModeDefine[_modelCount];

		/***************************** backgroud ***************************************/
		LAppTextureManager::TextureInfo *backgroundTexture =
			textureManager->CreateTextureFromPngFile(targetPath +backImageName);
		float x = width*0.5;
		float y = height * 0.45;
		float fWidth = static_cast<float>(backgroundTexture->width);
		float fHeight = static_cast<float>(backgroundTexture->height);
		_viewData[id]._mode[_modelCount]._back =
			new Sprite(x, y, fWidth, fHeight, backgroundTexture->id, _programId);


		LAppTextureManager::TextureInfo *catbackgroundTexture =
			textureManager->CreateTextureFromPngFile(targetPath + catBackImageName);
		x = width * 0.5;
		y = height * 0.45;
		fWidth = static_cast<float>(catbackgroundTexture->width);
		fHeight =static_cast<float>(catbackgroundTexture->height);
		_viewData[id]._mode[_modelCount]._catback =
			new Sprite(x, y, fWidth, fHeight,catbackgroundTexture->id,_programId);

		/***************************** lefthand ***************************************/
		//load left hand up
		LAppTextureManager::TextureInfo *LeftHandUpTexture =
			textureManager->CreateTextureFromPngFile(
				targetPath + leftHandImagePath +
				leftHandUpImageName);
		x = width * 0.5f;
		y = height * 0.45f;
		fWidth = static_cast<float>(LeftHandUpTexture->width);
		fHeight = static_cast<float>(LeftHandUpTexture->height);
		_viewData[id]._mode[_modelCount]._leftHandUp =
			new Sprite(x, y, fWidth, fHeight, LeftHandUpTexture->id,_programId);

		//load left hands
		_viewData[id]._mode[_modelCount]._leftHandsCount =ModelLeftHandCount[_modelCount];
		for (int i = 0; i < ModelLeftHandCount[_modelCount]; i++) {
			string imageName = LeftHandImageName[i];
			LAppTextureManager::TextureInfo *HandsTexture =
				textureManager->CreateTextureFromPngFile(
					targetPath +
					leftHandImagePath +
					imageName);
			x = width * 0.5f;
			y = height * 0.5f;
			fWidth = static_cast<float>(HandsTexture->width);
			fHeight = static_cast<float>(HandsTexture->height);
			_viewData[id]._mode[_modelCount]._leftHands[i]=
				new Sprite(x, y, fWidth, fHeight,HandsTexture->id, _programId);
		}

		
		/***************************** righthands ***************************************/
		_viewData[id]._mode[_modelCount].isUseRightHandModel = ModelRightHandModel[_modelCount];		
		_viewData[id]._mode[_modelCount]._rightHandsCount = ModelRightHandCount[_modelCount];
		if (!ModelRightHandModel[_modelCount]) {
			//load righthandsup
			LAppTextureManager::TextureInfo *RightHandUpTexture =
				textureManager->CreateTextureFromPngFile(
					targetPath +
					rightHandImagePath +
					rightHandUpImageName);
			x = width * 0.5f;
			y = height * 0.45f;
			fWidth = static_cast<float>(RightHandUpTexture->width);
			fHeight =static_cast<float>(RightHandUpTexture->height);
			_viewData[id]._mode[_modelCount]._rightHandUp =
				new Sprite(x, y, fWidth, fHeight,RightHandUpTexture->id, _programId);

			//Load right hands
			for (int i = 0; i < ModelRightHandCount[_modelCount];
			     i++) {
				string imageName = RightHandImageName[i];
				LAppTextureManager::TextureInfo *HandsTexture =
					textureManager->CreateTextureFromPngFile(
						targetPath + rightHandImagePath +imageName);
				x = width * 0.5f;
				y = height * 0.45f;
				fWidth =static_cast<float>(HandsTexture->width);
				fHeight = static_cast<float>(HandsTexture->height);
				_viewData[id]._mode[_modelCount]._rightHands[i] =
					new Sprite(x, y, fWidth, fHeight,
						   HandsTexture->id,
						   _programId);
			}

		}

		/***************************** keys ***************************************/
		_viewData[id]._mode[_modelCount]._keysCount = ModelKeyCount[_modelCount];
		//load keyboard
		for (int i = 0; i < ModelKeyCount[_modelCount]; i++) {
			string imageName = KeyImageName[i];
			LAppTextureManager::TextureInfo *KeysTexture =
				textureManager->CreateTextureFromPngFile(
					targetPath + keyboardimagepath +
					imageName);
			x = width * 0.5f;
			y = height * 0.45f;
			fWidth = static_cast<float>(KeysTexture->width);
			fHeight = static_cast<float>(KeysTexture->height);
			_viewData[id]._mode[_modelCount]._keys[i]=
				new Sprite(x, y, fWidth, fHeight, KeysTexture->id, _programId);
		}

	}

}

void View::PreModelDraw(Model& refModel,int id)
{
    Csm::Rendering::CubismOffscreenFrame_OpenGLES2* useTarget = NULL;

    if (_viewData[id].target != SelectTarget_None)
    {
	    useTarget = (_viewData[id].target == SelectTarget_ViewFrameBuffer)
				? &_renderBuffer
				: &refModel.GetRenderBuffer();

        if (!useTarget->IsValid())
        {
            int width, height;
	    width = VtuberDelegate::GetInstance()->getBufferWidth(id);
	    height = VtuberDelegate::GetInstance()->getBufferHeight(id);
            if (width != 0 && height != 0)
            {
                
                useTarget->CreateOffscreenFrame(static_cast<csmUint32>(width), static_cast<csmUint32>(height));
            }
        }

        useTarget->BeginDraw();
        useTarget->Clear(_clearColor[0], _clearColor[1], _clearColor[2], _clearColor[3]); 
    }
}

void View::PostModelDraw(Model& refModel,int id)
{
    Csm::Rendering::CubismOffscreenFrame_OpenGLES2* useTarget = NULL;

    if (_viewData[id].target != SelectTarget_None)
    {
	    useTarget = (_viewData[id].target == SelectTarget_ViewFrameBuffer)
				? &_renderBuffer
				: &refModel.GetRenderBuffer();
     
        useTarget->EndDraw();
    } 
}

void View::SwitchRenderingTarget(SelectTarget targetType,int id)
{
    _viewData[id].target = targetType;
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

uint16_t View::GetTotalViewer()
{
	return dataCount;
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

void View::OnMouseMoved(float pointX, float pointY,int id) const
{
	float viewX =this->TransformViewX(eventManager->GetX(), id);
	float viewY =this->TransformViewY(eventManager->GetY(), id);
	eventManager->MouseEventMoved(pointX, pointY);
}

void View::OnKeyDown(int _key) const {
	eventManager->KeyEventDown(_key);
}

void View::OnKeyUp(int _key) const {
	eventManager->KeyEventUp(_key);
}

double View::GetMouseX()
{
	int width, height;
	Pal::GetDesktopResolution(width,height);
	return static_cast<double>(eventManager->GetCurrentMouseX()) /
	       static_cast<double>(width);
}

double View::GetMouseY()
{
	int width, height;
	Pal::GetDesktopResolution(width, height);
	return static_cast<double>(eventManager->GetCurrentMouseY()) /
	       static_cast<double>(height);
}

void View::OnRButtonDown() const {
	eventManager->RightButtonDown();
}

void View::OnRButtonUp() const {
	eventManager->RightButtonUp();
}

void View::OnLButtonDown() const {
	eventManager->LeftButtonDown();
}

void View::OnLButtonUp() const {
	eventManager->LeftButtonUp();
}

bool View::GetLButton()
{
	return eventManager->GetLeftButton();
}

bool View::GetRButton()
{
	return eventManager->GetRightButton();
}

void View::setMod(uint16_t i)
{
	_mod = i;
}

void View::setLive2D(bool _isLive2D) {
	isUseLive2d = _isLive2D;
}


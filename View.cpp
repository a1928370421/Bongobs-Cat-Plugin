#include "View.hpp"
#include <math.h>
#include <string>
#include "Pal.hpp"
#include "VtuberDelegate.hpp"
#include "Live2DManager.hpp"
#include "LAppTextureManager.hpp"
#include "Define.hpp"
#include "LAppModel.hpp"
#include "Sprite.hpp"
#include "EventManager.hpp"

using namespace std;
using namespace Define;

View::View() : _programId(0), dataCount(0)
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

    _viewData[id]._viewMatrix->Scale(0.6*scale, scale);
    double _x = -static_cast<float>(RenderTargetWidth - width) /static_cast<float>(RenderTargetWidth);
    double _y = -static_cast<float>(RenderTargetHeight-height) /static_cast<float>(RenderTargetHeight);
    _viewData[id]._viewMatrix->Translate(x + _x, y+_y);
}

void View::Render(int id)
{
	Live2DManager* Live2DManager = Live2DManager::GetInstance();
	//render cat
	UpdataViewData(id);
	Live2DManager->OnUpdate(id);

	_viewData[id]._back->Render(id);

	//render right hand
	UpdataViewData(1);
	Live2DManager->OnUpdate(1);

	for (int i = 0; i < 15; i++) {
		if (eventManager->GetKeySignal(i)) {
			_viewData[id]._Keys[i]->Render(id);
		}
	}

	bool isUp = true;
	for (int i = 0; i < 15; i++) {
		if (eventManager->GetKeySignal(i)) {	
			_viewData[id]._hands[i]->Render(id);
			isUp = false;
			break;//cat only have one left hand
		}			
	}
	if (isUp)
		_viewData[id]._leftHandUp->Render(id);
}

void View::InitializeSpirite(int id) {

	_programId = VtuberDelegate::GetInstance()->CreateShader();

	int width, height;
	width = VtuberDelegate::GetInstance()->getBufferWidth(id);
	height = VtuberDelegate::GetInstance()->getBufferHeight(id);
	LAppTextureManager *textureManager =VtuberDelegate::GetInstance()->GetTextureManager();

	const string resourcesPath = ResourcesPath;
	const string imagepath = ImagePath;
	const string handimagepath = HandImagePath;
	const string keyboardimagepath = KeyBoardImagePath;

	string imageName = BackImageName;	
	LAppTextureManager::TextureInfo *backgroundTexture =
		textureManager->CreateTextureFromPngFile(resourcesPath +imagepath + imageName);

	float x = width*0.5;
	float y = height * 0.5;
	float fWidth = static_cast<float>(backgroundTexture->width);
	float fHeight = static_cast<float>(backgroundTexture->height);
	_viewData[id]._back = new Sprite(x, y, fWidth, fHeight,backgroundTexture->id,_programId);

	imageName = LeftHandUpImageName;
	LAppTextureManager::TextureInfo *LeftHandUpTexture =textureManager->CreateTextureFromPngFile(resourcesPath +imagepath + imageName);
	x = width * 0.5f;
	y = height * 0.5f;
	fWidth = static_cast<float>(LeftHandUpTexture->width);
	fHeight = static_cast<float>(LeftHandUpTexture->height);
	_viewData[id]._leftHandUp = new Sprite(x, y, fWidth, fHeight, LeftHandUpTexture->id, _programId);

	//Load left hands
	for (int i = 0; i < 15; i++) {
		imageName = HandImageName[i];
		LAppTextureManager::TextureInfo *HandsTexture =textureManager->CreateTextureFromPngFile(resourcesPath + imagepath + handimagepath +imageName);
		x = width * 0.5f;
		y = height * 0.5f;
		fWidth = static_cast<float>(HandsTexture->width );
		fHeight = static_cast<float>(HandsTexture->height);
		_viewData[id]._hands[i] = new Sprite(x, y, fWidth, fHeight, HandsTexture->id,_programId);
	}
	//load keyboard
	for (int i = 0; i < 15; i++) {
		imageName = HandImageName[i];
		LAppTextureManager::TextureInfo *KeysTexture =
			textureManager->CreateTextureFromPngFile(resourcesPath + imagepath + keyboardimagepath +
				imageName);
		x = width * 0.5f;
		y = height * 0.5f;
		fWidth = static_cast<float>(KeysTexture->width);
		fHeight = static_cast<float>(KeysTexture->height);
		_viewData[id]._Keys[i] = new Sprite(
			x, y, fWidth, fHeight, KeysTexture->id, _programId);
	}

}

void View::PreModelDraw(LAppModel& refModel,int id)
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

void View::PostModelDraw(LAppModel& refModel,int id)
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


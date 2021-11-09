
#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Math/CubismMatrix44.hpp>
#include <Math/CubismViewMatrix.hpp>
#include "CubismFramework.hpp"
#include <Rendering/OpenGL/CubismOffscreenSurface_OpenGLES2.hpp>


#define MAXVIEWDATA 1024

#define MAXMODECOUNT 24

#define MAXKEYCOUNT 128

#define MAXFACECOUNT 64

class Model;
class Sprite;
class InfoReader;
class EventManager;

enum SelectTarget
    {
        SelectTarget_None,                ///< デフォルトのフレームバッファにレンダリング
        SelectTarget_ModelFrameBuffer,    ///< Modelが各自持つフレームバッファにレンダリング
        SelectTarget_ViewFrameBuffer,     ///< Viewの持つフレームバッファにレンダリング
    };

/**
* @brief 描画クラス
*/
struct Mode {
	Sprite *_back;       ///< 背景画像
	Sprite *_catback; ///< 背景画像&猫

	bool _haseModel;
	int _modelId;

	bool _hasRightHandModel;
	int _leftHandModelId;

	Sprite *_rightHandUp; ///< 右爪
	uint16_t _rightHandsCount;
	Sprite *_rightHands[MAXKEYCOUNT];

	bool _hasLeftHandModel;
	int _rightHandModelId;

	Sprite *_leftHandUp; ///< 左爪
	uint16_t _leftHandsCount;
	Sprite *_leftHands[MAXKEYCOUNT];

	uint16_t _keysCount;
	Sprite *_keys[MAXKEYCOUNT];
};

struct ViewData {
	Csm::CubismViewMatrix *_viewMatrix;
	Csm::CubismMatrix44 *_deviceToScreen;

	int _modecount;
	Mode _mode[MAXMODECOUNT];

	int _curentface;
	int _faceCount;
	Sprite *_face[MAXFACECOUNT];
};



class View
{
public:

    View();

    ~View();

    void Initialize(int id);

    void Release(int id);

    void Render(int id);

    void InitializeSpirite(int id);

    void InitializeModel(int id);

    void PreModelDraw(Model& refModel);

    void PostModelDraw(Model &refModel);

    void SwitchRenderingTarget(SelectTarget targetType,int id);

    void SetRenderTargetClearColor(float r, float g, float b);

     Csm::CubismViewMatrix *GetViewMatrix(int id);

     Csm::CubismMatrix44 *GetDeviceToScreenMatrix(int id);

     InfoReader *GetInfoReader();

     /**
    * @brief X座標をView座標に変換する。
    *
    * @param[in]       deviceX            デバイスX座標
    */
     float TransformViewX(float deviceX,int id) const;

     /**
    * @brief Y座標をView座標に変換する。
    *
    * @param[in]       deviceY            デバイスY座標
    */
     float TransformViewY(float deviceY,int id) const;

     EventManager *GetEventManager();

     void OnMouseMoved(float pointX, float pointY,int id) const;

     void Update(bool _isLive2D,bool _isUseMask);

     void setMod(uint16_t i);

private:
    void UpdataViewData(int id);

    int TranslateKey(int key,int id);

    int TranslateKey2(int key, int id);

    void RenderBackgroud(int id);

    void RenderCat(int id);

    void RenderKeys(int id);

    bool RenderLeftHands(int id);

    bool RenderRightHands(int id);

    void RenderUphands(bool leftup,bool righttup,int id);

    void ReanderMask(int id);

    void ChangeMode(int mod, int id);

    ViewData _viewData[MAXVIEWDATA];

    uint16_t _mod;
    bool isUseLive2d;
    bool isUseMask;

    GLuint _programId;                       ///< シェーダID

    Csm::Rendering::CubismOffscreenFrame_OpenGLES2 _renderBuffer;   ///< モードによってはCubismモデル結果をこっちにレンダリング
    float _clearColor[4];           ///< レンダリングターゲットのクリアカラー

    SelectTarget target;
    EventManager *eventManager;
    InfoReader *_infoReader;
};

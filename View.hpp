
#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Math/CubismMatrix44.hpp>
#include <Math/CubismViewMatrix.hpp>
#include "CubismFramework.hpp"
#include <Rendering/OpenGL/CubismOffscreenSurface_OpenGLES2.hpp>
#include "EventManager.hpp"


#define MAXVIEWDATA 1024

#define MAXMODECOUNT 8

class Model;
class Sprite;

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

	bool isUseRightHandModel;
	Sprite *_rightHandUp; ///< 右爪
	uint16_t _rightHandsCount;
	Sprite *_rightHands[15];

	Sprite *_leftHandUp; ///< 左爪
	uint16_t _leftHandsCount;
	Sprite *_leftHands[15];

	uint16_t _keysCount;
	Sprite *_keys[15];
};
struct ViewData {
	Csm::CubismViewMatrix *_viewMatrix;
	Csm::CubismMatrix44 *_deviceToScreen;
	SelectTarget target;

	Mode _mode[MAXMODECOUNT];
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

    void PreModelDraw(Model& refModel,int id);

    void PostModelDraw(Model &refModel, int id);

    void SwitchRenderingTarget(SelectTarget targetType,int id);

    void SetRenderTargetClearColor(float r, float g, float b);

     Csm::CubismViewMatrix *GetViewMatrix(int id);

     Csm::CubismMatrix44 *GetDeviceToScreenMatrix(int id);

     uint16_t GetTotalViewer();

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

     void OnMouseMoved(float pointX, float pointY,int id) const;

     void OnKeyDown(int _key) const;

     void OnKeyUp(int _key) const;

     double GetMouseX();

     double GetMouseY();

     void OnRButtonDown() const;

     void OnRButtonUp() const;

     void OnLButtonDown() const;

     void OnLButtonUp() const;

      bool GetLButton();

      bool GetRButton();

      void setMod(uint16_t i);

      void setLive2D(bool _isLive2D);

private:
    void UpdataViewData(int id);

    int TranslateKey(int key,int id);

    ViewData _viewData[MAXVIEWDATA];

    uint16_t _mod;
    bool isUseLive2d;

    uint16_t dataCount;
    GLuint _programId;                       ///< シェーダID

    Csm::Rendering::CubismOffscreenFrame_OpenGLES2 _renderBuffer;   ///< モードによってはCubismモデル結果をこっちにレンダリング
    float _clearColor[4];           ///< レンダリングターゲットのクリアカラー

    EventManager *eventManager;
};

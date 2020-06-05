
#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Math/CubismMatrix44.hpp>
#include <Math/CubismViewMatrix.hpp>
#include "CubismFramework.hpp"
#include <Rendering/OpenGL/CubismOffscreenSurface_OpenGLES2.hpp>
#include "EventManager.hpp"


#define MAXVIEWDATA 1024

class LAppModel;
class Sprite;

enum SelectTarget
    {
        SelectTarget_None,                ///< デフォルトのフレームバッファにレンダリング
        SelectTarget_ModelFrameBuffer,    ///< LAppModelが各自持つフレームバッファにレンダリング
        SelectTarget_ViewFrameBuffer,     ///< Viewの持つフレームバッファにレンダリング
    };

/**
* @brief 描画クラス
*/
struct ViewData {
	Csm::CubismViewMatrix *_viewMatrix;
	Csm::CubismMatrix44 *_deviceToScreen;
	SelectTarget target;

	Sprite *_back; ///< 背景画像
	Sprite *_leftHandUp; ///< 左爪
	Sprite *_hands[15];
	Sprite *_Keys[15];
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

    void PreModelDraw(LAppModel& refModel,int id);

    void PostModelDraw(LAppModel &refModel, int id);

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

private:
    void UpdataViewData(int id);

    ViewData _viewData[MAXVIEWDATA];

    uint16_t dataCount;
    GLuint _programId;                       ///< シェーダID

    Csm::Rendering::CubismOffscreenFrame_OpenGLES2 _renderBuffer;   ///< モードによってはCubismモデル結果をこっちにレンダリング
    float _clearColor[4];           ///< レンダリングターゲットのクリアカラー

    EventManager *eventManager;
};

/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "Define.hpp"
#include <CubismFramework.hpp>

namespace Define {

    using namespace Csm;

    // 画面
    const csmFloat32 ViewMaxScale = 10.0f;
    const csmFloat32 ViewMinScale = 0.8f;

    const csmFloat32 ViewLogicalLeft = -1.0f;
    const csmFloat32 ViewLogicalRight = 1.0f;

    const csmFloat32 ViewLogicalMaxLeft = -2.0f;
    const csmFloat32 ViewLogicalMaxRight = 2.0f;
    const csmFloat32 ViewLogicalMaxBottom = -2.0f;
    const csmFloat32 ViewLogicalMaxTop = 2.0f;

    // モーションの優先度定数
    const csmInt32 PriorityNone = 0;
    const csmInt32 PriorityIdle = 1;
    const csmInt32 PriorityNormal = 2;
    const csmInt32 PriorityForce = 3;

    // Frameworkから出力するログのレベル設定
    const CubismFramework::Option::LogLevel CubismLoggingLevel = CubismFramework::Option::LogLevel_Verbose;
    const csmBool DebugMod = false;

    // デフォルトのレンダーターゲットサイズ
    const csmInt32 RenderTargetWidth = 1280;
    const csmInt32 RenderTargetHeight = 768;

    const csmChar *ParaMouseX= "ParamMouseX";
    const csmChar *ParaMouseY = "ParamMouseY";
    const csmChar *ParaLeftButton = "ParamMouseLeftDown";
    const csmChar *ParaRightButton = "ParamMouseRightDown";

    const csmInt32 KeyAmount = 61;
    const csmChar *KeyDefine[] = {
	    "a","b","c","d","e","f","g","h","i","j","k","l",
	    "m","n","o","p","q","r","s","t","u","v","w","x","y","z",
	    "0","1", "2","3","4","5","6", "7", "8","9",
	    "space","lshift","lctrl",
	    "f1","f2","f3","f4","f5","f6","f7","f8","f9","f10","f11","f12",
	    "up","down","left","right","<",">","[","]","rshift","rctrl"};

    const csmChar *ResourcesPath = "Bango Cat/";

    const csmChar *ModePath= "mode/";
    const csmChar *MaskPath = "face/";
    const csmChar *KPSPath = "kps/";

    }

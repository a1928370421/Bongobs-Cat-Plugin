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
    const csmInt32 RenderTargetWidth = 1200;
    const csmInt32 RenderTargetHeight = 780;

    const csmChar *ParaMouseX= "Mousex";
    const csmChar *ParaMouseY = "Mousey";
    const csmChar *ParaLeftButton = "LeftButton";
    const csmChar *ParaRightButton = "RightButton";

    const csmInt32 KeyAmount = 21;
    const csmChar *KeyDefine[] = {"1", "2", "3", "4", "5", "6", "7",
				  "q", "w", "e", "r", "a", "s","d",
				  "space","shift","ctrl","up","down",
				  "left","right"};

    const csmChar *ResourcesPath = "Bango Cat/";
    const csmChar *CatModelPath = "Resources/cat/cat.model3.json";
    const csmChar *RightHandModelPath = "Resources/right hand/RightHand.model3.json";

    const csmChar *ModeImagePath= "img/";
    const csmChar *ModeDefine[] = {"standard/","keyboard/"};
    const csmInt32 ModeCount = 2;

    const csmBool ModelRightHandModel[] = {true,false};
    const csmInt32 ModelRightHandCount[] = {0,4};
    const csmInt32 ModelLeftHandCount[] = {15,3};
    const csmInt32 ModelKeyCount[] = {15,7};

    const csmChar *Mod1KeyUse[] = {"1","2", "3", "4", "5","6","7", "q",
				    "e", "r", "space", "a", "d", "s", "w"};

    const csmChar *Mod2KeyUse[] = {"ctrl", "shift", "r",
				    "down",  "right", "left","up"};
  
    const csmChar *BackImageName = "bg.png";
    const csmChar *CatBackImageName = "catbg.png";

    const csmChar *LeftHandImagePath = "lefthand/";
    const csmChar *LeftHandUpImageName = "leftup.png";
    const csmChar *LeftHandImageName[] = {"0.png",  "1.png",  "2.png",
					  "3.png","4.png",  "5.png",  "6.png",  "7.png",
				      "8.png",  "9.png",  "10.png", "11.png",
				      "12.png", "13.png", "14.png"};

    const csmChar *RightHandImagePath = "righthand/";
    const csmChar *RightHandUpImageName ="rightup.png";
    const csmChar *RightHandImageName[] = {"0.png",  "1.png",  "2.png",
					   "3.png",  "4.png",  "5.png",
					   "6.png",  "7.png",  "8.png",
					   "9.png",  "10.png", "11.png",
					   "12.png", "13.png", "14.png"};

    
    const csmChar *KeyboardImagePath = "KeyBoard/";
    const csmChar *KeyImageName[] = {"0.png",  "1.png",  "2.png",  "3.png",
				     "4.png",  "5.png",  "6.png",  "7.png",
				     "8.png",  "9.png",  "10.png", "11.png",
				     "12.png", "13.png", "14.png"};

    }

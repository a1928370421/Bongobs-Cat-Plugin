/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */
#pragma once

#include <CubismFramework.hpp>

/**
* @brief  Sample Appで使用する定数
*
*/
namespace Define {

    using namespace Csm;

    extern const csmFloat32 ViewMaxScale;           ///< 拡大縮小率の最大値
    extern const csmFloat32 ViewMinScale;           ///< 拡大縮小率の最小値

    extern const csmFloat32 ViewLogicalLeft;        ///< 論理的なビュー座標系の左端の値
    extern const csmFloat32 ViewLogicalRight;       ///< 論理的なビュー座標系の右端の値

    extern const csmFloat32 ViewLogicalMaxLeft;     ///< 論理的なビュー座標系の左端の最大値
    extern const csmFloat32 ViewLogicalMaxRight;    ///< 論理的なビュー座標系の右端の最大値
    extern const csmFloat32 ViewLogicalMaxBottom;   ///< 論理的なビュー座標系の下端の最大値
    extern const csmFloat32 ViewLogicalMaxTop;      ///< 論理的なビュー座標系の上端の最大値
                                                    // 外部定義ファイル(json)と合わせる
                                                    // モーションの優先度定数
    extern const csmInt32 PriorityNone;             ///< モーションの優先度定数: 0
    extern const csmInt32 PriorityIdle;             ///< モーションの優先度定数: 1
    extern const csmInt32 PriorityNormal;           ///< モーションの優先度定数: 2
    extern const csmInt32 PriorityForce;            ///< モーションの優先度定数: 3

    // Frameworkから出力するログのレベル設定
    extern const CubismFramework::Option::LogLevel CubismLoggingLevel;
    extern const csmBool DebugMod;
        // デフォルトのレンダーターゲットサイズ
    extern const csmInt32 RenderTargetWidth;
    extern const csmInt32 RenderTargetHeight;

    extern const csmChar *ParaMouseX;
    extern const csmChar *ParaMouseY;
    extern const csmChar *ParaLeftButton;
    extern const csmChar *ParaRightButton;

    extern const csmChar *ResourcesPath;
    extern const csmChar *CatModelPath;
    extern const csmChar *RightHandModelPath;
    extern const csmChar *ImagePath;
    extern const csmChar *BackImageName;
    extern const csmChar *LeftHandUpImageName;
    extern const csmChar *HandImagePath;
    extern const csmChar *KeyBoardImagePath;
    extern const csmChar *HandImageName[];

    }

/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#define MAXMODELDATA 1024

#include <CubismFramework.hpp>
#include <Math/CubismMatrix44.hpp>
#include <Math/CubismViewMatrix.hpp>
#include <Type/csmVector.hpp>

class LAppModel;

/**
* @brief サンプルアプリケーションにおいてCubismModelを管理するクラス<br>
*         モデル生成と破棄、タップイベントの処理、モデル切り替えを行う。
*
*/
class Live2DManager
{
	struct ModelData {
		Csm::csmVector<LAppModel *>_models; ///< モデルインスタンスのコンテナ
		Csm::csmString _modelPath;
	};

public:
    /**
    * @brief   クラスのインスタンス（シングルトン）を返す。<br>
    *           インスタンスが生成されていない場合は内部でインスタンを生成する。
    *
    * @return  クラスのインスタンス
    */
    static Live2DManager* GetInstance();

    /**
    * @brief   クラスのインスタンス（シングルトン）を解放する。
    *
    */
    static void ReleaseInstance();

    /**
    * @brief   現在のシーンで保持しているモデルを返す
    *
    * @param[in]   no  モデルリストのインデックス値
    * @return      モデルのインスタンスを返す。インデックス値が範囲外の場合はNULLを返す。
    */
    LAppModel *GetModel(Csm::csmUint16 id) const;

    /**
    * @brief   現在のシーンで保持しているすべてのモデルを解放する
    *
    */
    void ReleaseAllModel(Csm::csmUint16 id);

    void ReleaseModel(Csm::csmUint16 id);

    /**
    * @brief   画面を更新するときの処理
    *          モデルの更新処理および描画処理を行う
    */
    void OnUpdate(Csm::csmUint16 id) const;

    /**
    * @brief   シーンを切り替える<br>
    *           サンプルアプリケーションではモデルセットの切り替えを行う。
    */
    Csm::csmBool ChangeScene(const Csm::csmChar * _modelPath,Csm::csmInt16 _id);

private:
    /**
    * @brief  コンストラクタ
    */
    Live2DManager();

    /**
    * @brief  デストラクタ
    */
    virtual ~Live2DManager();

    ModelData _modeldata[MAXMODELDATA];
};

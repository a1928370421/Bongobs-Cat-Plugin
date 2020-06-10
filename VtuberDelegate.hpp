/**
  Created by Weng Y on 2020/05/25.
  Copyright © 2020 Weng Y. Under GNU General Public License v2.0.
*/

#pragma once

#include <GL/glew.h>
#include "LAppAllocator.hpp"
#include <GLFW/glfw3.h>

#define MAXMODELCOUNT 1024

class View;
class LAppTextureManager;
class Hook;
/**
* @brief   アプリケーションクラス。
*   Cubism SDK の管理を行う。
*/
class VtuberDelegate {

	struct RenderInfo {
		bool isLoadResource;
		//view
		double viewPoint_x;
		double viewPoint_y;
		int windowWidth;
		int windowHeight; 				
		double Scale;
	};

public:

	static VtuberDelegate *GetInstance();

	static void ReleaseInstance();

    /*
    *@breif 不处理opengl相关的初始化
    */
    bool LoadResource(int id);

    void ReleaseResource(int id);

    bool Initialize(int id);

    void Release();

    bool isLoadResource(int id);

    int getBufferWidth(int id);

    int getBufferHeight(int id);

    double getScale(int id);

    double GetX(int id);

    double GetY(int id);

    void UpdataViewWindow(double _x,double _y,int _width, int _height,double _scale, int _id);


    View* GetView() { return _view; }

    LAppTextureManager* GetTextureManager() { return _textureManager; }


    /**
    * @brief 渲染一帧画面到指定缓冲
    */
    void Reader(int targatid, char *data, int bufferWidth, int bufferheight);


    void ChangeModel(const char *ModelName, int id);


    void updataModelSetting(bool _randomMotion, double _delayTime, bool _breath,
			    bool _eyeBlink, bool _track,
			    bool _isMouseHorizontalFlip,bool _isMouseVerticalFlip,
		            int id);

    const char ** GetModeDefine(int &size);

    void ChangeMode(const char *_mode, bool _live2D,bool _isUseMask, int id);

    void ChangeMouseMovement(bool _mouse);

    GLuint CreateShader();

    bool CheckShader(GLuint shaderId);
private:
    /**
    * @brief   コンストラクタ
    */
	VtuberDelegate();

    /**
    * @brief   デストラクタ
    */
	~VtuberDelegate();

    /**
    * @brief   Cubism SDK の初期化
    */
	void InitializeCubism();

	


    LAppAllocator _cubismAllocator;              ///< Cubism SDK Allocator
    Csm::CubismFramework::Option _cubismOption;  ///< Cubism SDK Option
    GLFWwindow* _window;                         ///< OpenGL ウィンドウ
    View* _view;                             ///< View情報
    Hook *_hook;
    LAppTextureManager* _textureManager;         ///< テクスチャマネージャー

    char *ModelFileName[MAXMODELCOUNT];		///模型文件夹的名称集合
    int ModelFileCount;				///模型数量

    RenderInfo _renderInfo[MAXMODELCOUNT];
    };

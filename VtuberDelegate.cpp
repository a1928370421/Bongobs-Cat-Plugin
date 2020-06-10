/**
  Created by Weng Y on 2020/05/25.
  Copyright © 2020 Weng Y. Under GNU General Public License v2.0.
*/

#include "VtuberDelegate.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "View.hpp"
#include "Pal.hpp"
#include "Define.hpp"
#include "Live2DManager.hpp"
#include "LAppTextureManager.hpp"
#include "Hook.hpp"
#include "InfoReader.hpp"
#include "EventManager.hpp"

using namespace Csm;
using namespace std;
using namespace Define;

namespace {
VtuberDelegate *s_instance = NULL;

    static uint16_t VtuberCount = 0;

    static bool isFirst = true;

    static GLFWwindow *_window;
    }

VtuberDelegate *VtuberDelegate::GetInstance()
{
    if (s_instance == NULL)
    {
		s_instance = new VtuberDelegate();
    }

    return s_instance;
}

void VtuberDelegate::ReleaseInstance()
{
    if (s_instance != NULL)
    {
        delete s_instance;
    }

    s_instance = NULL;
}

bool VtuberDelegate::LoadResource(int id)
{
	_view->Initialize(id);
	return true;
}

void VtuberDelegate::ReleaseResource(int id) {
	_renderInfo[id].isLoadResource = false;
	_view->Release(id);
}

bool VtuberDelegate::Initialize(int id)
{
   
    //Gl Init
    if (isFirst) {
	   
	    isFirst = false;
	    // GLFWの初期化
	    if (glfwInit() == GL_FALSE)
	    {
		//return GL_FALSE;
	    }
	    
	    // Windowの生成_
	    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
	    glfwWindowHint(GLFW_SAMPLES, 16);
	    _window = glfwCreateWindow(RenderTargetWidth, RenderTargetHeight, "bongo cat", NULL,NULL);
	    if (_window == NULL)
	    {
		glfwTerminate();
		return GL_FALSE;
	    }

	    if (glfwRawMouseMotionSupported())
		    glfwSetInputMode(_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	    glfwMakeContextCurrent(_window);

	    glewExperimental = GL_TRUE;
	    if (glewInit() != GLEW_OK) {
		    glfwTerminate();
		    return GL_FALSE;
	    }
	    _hook = new Hook();
	    _hook->Strat();
    
    }
    
    
    // Cubism SDK の初期化
    InitializeCubism();

    _view->InitializeSpirite(id);
    _view->InitializeModel(id);

    return GL_TRUE;
}

void VtuberDelegate::Release()
{
        //glfwDestroyWindow(_window);

        //glfwTerminate();

        delete _textureManager;

        delete _view;

        Live2DManager::ReleaseInstance();

	CubismFramework::CleanUp();   

        CubismFramework::Dispose();
}

void VtuberDelegate::Reader(int id,char *buffer,int bufferWidth, int bufferheight)
{
	glEnable(GL_MULTISAMPLE);

	glClearColor(0.0, 0.0, 0.0,0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//描画更新
	_view->Render(id);

	//写入缓冲
	glReadPixels(0, 0, bufferWidth, bufferheight, GL_RGBA, GL_UNSIGNED_BYTE,buffer);

}

void VtuberDelegate::UpdataViewWindow(double _x, double _y, int _width,
				      int _height, double _scale, int _id)
{
	_renderInfo[_id].viewPoint_x = _x;
	_renderInfo[_id].viewPoint_y = _y;
	_renderInfo[_id].windowWidth = _width/32*32;
	_renderInfo[_id].windowHeight = _height;
	_renderInfo[_id].Scale = _scale;
}

void VtuberDelegate::updataModelSetting(bool _randomMotion, double _delayTime,
					bool _breath, bool _eyeBlink,bool _istrack,
					bool _isMouseHorizontalFlip,bool _isMouseVerticalFlip,
					int id)
{
	Live2DManager::GetInstance()->UpdateModelSetting(
		_randomMotion, _delayTime, _breath, _eyeBlink, _istrack,
		_isMouseHorizontalFlip, _isMouseVerticalFlip);
}

const char **VtuberDelegate::GetModeDefine(int &size) {
	InfoReader *_infoReader = _view->GetInfoReader();

	size = _infoReader->ModeCount;
	return (const char**)_infoReader->ModePath;
}

void VtuberDelegate::ChangeMode(const char *_mode,bool _live2d,bool _isUseMask, int id)
{
	InfoReader *_infoReader = _view->GetInfoReader();
	for (int i = 0; i < _infoReader->ModeCount; i++) {
		const char *a = _infoReader->ModePath[i];
		if (strcmp(_mode, _infoReader->ModePath[i]) == 0) {
			_view->setMod(i);
			
		}			
	}
	_view->Update(_live2d,_isUseMask);
}

void VtuberDelegate::ChangeMouseMovement(bool _mouse) {
	Live2DManager::GetInstance()->ChangeMouseMovement(_mouse);
}


void VtuberDelegate::ChangeModel(const char *ModelName, int id)
{
	
}

VtuberDelegate::VtuberDelegate()
	: _cubismOption(),
	  _window(NULL),
	  ModelFileCount(0)
{
    _view = new View();
    _textureManager = new LAppTextureManager();
}

VtuberDelegate::~VtuberDelegate() {}

void VtuberDelegate::InitializeCubism()
{
    _cubismOption.LogFunction = Pal::PrintMessage;
    _cubismOption.LoggingLevel = Define::CubismLoggingLevel;
    Csm::CubismFramework::StartUp(&_cubismAllocator, &_cubismOption);

    //Initialize cubism
    CubismFramework::Initialize();

    Live2DManager::GetInstance();

    //default proj
    CubismMatrix44 projection;
}

GLuint VtuberDelegate::CreateShader()
{
	//バーテックスシェーダのコンパイル
	GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	const char *vertexShader = "#version 120\n"
				   "attribute vec3 position;"
				   "attribute vec2 uv;"
				   "varying vec2 vuv;"
				   "void main(void){"
				   "    gl_Position = vec4(position, 1.0);"
				   "    vuv = uv;"
				   "}";
	glShaderSource(vertexShaderId, 1, &vertexShader, NULL);
	glCompileShader(vertexShaderId);
	if (!CheckShader(vertexShaderId)) {
		return 0;
	}

	//フラグメントシェーダのコンパイル
	GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	const char *fragmentShader =
		"#version 120\n"
		"varying vec2 vuv;"
		"uniform sampler2D texture;"
		"uniform vec4 baseColor;"
		"void main(void){"
		"    gl_FragColor = texture2D(texture, vuv) * baseColor;"
		"}";
	glShaderSource(fragmentShaderId, 1, &fragmentShader, NULL);
	glCompileShader(fragmentShaderId);
	if (!CheckShader(fragmentShaderId)) {
		return 0;
	}

	//プログラムオブジェクトの作成
	GLuint programId = glCreateProgram();
	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);

	// リンク
	glLinkProgram(programId);

	glUseProgram(programId);

	return programId;
}

bool VtuberDelegate::CheckShader(GLuint shaderId)
{
	GLint status;
	GLint logLength;
	glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 0) {
		GLchar *log = reinterpret_cast<GLchar *>(CSM_MALLOC(logLength));
		glGetShaderInfoLog(shaderId, logLength, &logLength, log);
		CubismLogError("Shader compile log: %s", log);
		CSM_FREE(log);
	}

	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		glDeleteShader(shaderId);
		return false;
	}

	return true;
}

bool VtuberDelegate::isLoadResource(int id)
{
	return _renderInfo[id].isLoadResource;
}

int VtuberDelegate::getBufferWidth(int id)
{
	return _renderInfo[id].windowWidth;
}

int VtuberDelegate::getBufferHeight(int id)
{
	return _renderInfo[id].windowHeight;
}

double VtuberDelegate::getScale(int id)
{
	return _renderInfo[id].Scale;
}

double VtuberDelegate::GetX(int id)
{
	return _renderInfo[id].viewPoint_x;
}

double VtuberDelegate::GetY(int id)
{
	return _renderInfo[id].viewPoint_y;
}

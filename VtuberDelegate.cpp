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
	 
	    _hook->Strat();
	    isFirst = false;
	 // GLFWの初期化
	    if (glfwInit() == GL_FALSE)
	    {
		return GL_FALSE;
	    }
    
	    // Windowの生成_
	    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
	    _window = glfwCreateWindow(RenderTargetWidth, RenderTargetHeight, "vtuber", NULL,NULL);
	    if (_window == NULL)
	    {
		glfwTerminate();
		return GL_FALSE;
	    }

	    glfwMakeContextCurrent(_window);

	    glewExperimental = GL_TRUE;
	    if (glewInit() != GLEW_OK) {
		    glfwTerminate();
		    return GL_FALSE;
	    }
    }

    // Cubism SDK の初期化
    InitializeCubism();

    _view->InitializeSpirite(id);

    return GL_TRUE;
}

void VtuberDelegate::Release()
{
        //glfwDestroyWindow(_window);

        //glfwTerminate();
	_hook->Stop();

        delete _textureManager;

        delete _view;

        Live2DManager::ReleaseInstance();

	CubismFramework::CleanUp();   

        CubismFramework::Dispose();

        ReleaseInstance();
}

void VtuberDelegate::Reader(int id,char *buffer,int bufferWidth, int bufferheight)
{
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glClearColor(0.0, 0.0, 0.0,0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//描画更新
	_view->Render(id);

	//写入缓冲
	glReadPixels(0, 0, bufferWidth, bufferheight, GL_RGBA, GL_UNSIGNED_BYTE,buffer);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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
					bool _breath, bool _eyeBlink,bool _istrack,int id)
{
	_renderInfo[id].randomMotion = _randomMotion;
	_renderInfo[id].delayTime = _delayTime;
	_renderInfo[id].isBreath = _breath;
	_renderInfo[id].isEyeBlink = _eyeBlink;
	_renderInfo[id].isTrack = _istrack;
}

bool VtuberDelegate::GetRandomMotion(int _id)
{
	return _renderInfo[_id].randomMotion;
}

double VtuberDelegate::GetDelayTime(int _id)
{
	return _renderInfo[_id].delayTime;
}

bool VtuberDelegate::GetBreath(int id)
{
	return _renderInfo[id].isBreath;
}

bool VtuberDelegate::GetEyeBlink(int id)
{
	return _renderInfo[id].isEyeBlink;
}

bool VtuberDelegate::GetTrack(int id)
{
	return _renderInfo[id].isTrack;
}

void VtuberDelegate::ChangeModel(const char *ModelName, int id)
{
	if (ModelName == NULL) {
		_renderInfo[id].isLoadResource = false;
		return;
	}		
	if (Live2DManager::GetInstance()->ChangeScene(ModelName, id))
		_renderInfo[id].isLoadResource = true;
	else 
		_renderInfo[id].isLoadResource = false;		
}

VtuberDelegate::VtuberDelegate()
	: _cubismOption(),
	  _window(NULL),
	  ModelFileCount(0)
{
    _view = new View();
    _textureManager = new LAppTextureManager();
    _hook = new Hook();
}

VtuberDelegate::~VtuberDelegate() {}

void VtuberDelegate::InitializeCubism()
{
    _cubismOption.LogFunction = Pal::PrintMessage;
    _cubismOption.LoggingLevel = Define::CubismLoggingLevel;
    Csm::CubismFramework::StartUp(&_cubismAllocator, &_cubismOption);

    //Initialize cubism
    CubismFramework::Initialize();

    string resourcesFilePath = std::string(ResourcesPath);
    string catModelFilePath = resourcesFilePath+CatModelPath;
    string rightHandModelFilePath = resourcesFilePath + RightHandModelPath;
    //load model
    ChangeModel(catModelFilePath.c_str(), 0);
    ChangeModel(rightHandModelFilePath.c_str(), 1);

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

# Set app name.
set(APP_NAME bongobs-cat)

# Set directory paths.
set(SDK_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(CORE_PATH ${SDK_ROOT_PATH}/CubismSdk/Core)
set(FRAMEWORK_PATH ${SDK_ROOT_PATH}/CubismSdk/Framework)
set(THIRD_PARTY_PATH ${SDK_ROOT_PATH}/thirdParty)
set(STB_PATH ${THIRD_PARTY_PATH}/stb)
set(GLEW_PATH ${THIRD_PARTY_PATH}/glew)
set(GLFW_PATH ${THIRD_PARTY_PATH}/glfw)
set(RES_PATH ${SDK_ROOT_PATH}/Resources)

project(${APP_NAME})

# Detect architecture.
if(CMAKE_EXE_LINKER_FLAGS STREQUAL "/machine:x64 /OPT:REF")
  set(ARCH x86_64)
elseif(CMAKE_EXE_LINKER_FLAGS STREQUAL "/machine:x86 /OPT:REF")
  set(ARCH x86)
else()
  message(FATAL_ERROR "[${APP_NAME}] Invalid linker flag ${CMAKE_EXE_LINKER_FLAGS}.")
endif()

# Detect compiler.
if(MSVC_VERSION MATCHES 1800)
  # Visual Studio 2013
  set(COMPILER 120)
elseif(MSVC_VERSION MATCHES 1900)
  # Visual Studio 2015
  set(COMPILER 140)
elseif(MSVC_VERSION GREATER_EQUAL 1910 AND MSVC_VERSION LESS 1920)
  # Visual Studio 2017
  set(COMPILER 141)
elseif(MSVC_VERSION GREATER_EQUAL 1920 AND MSVC_VERSION LESS 1930)
  # Visual Studio 2019
  set(COMPILER 142)
elseif(MSVC)
  message(FATAL_ERROR "[${APP_NAME}] Unsupported Visual C++ compiler used.")
else()
  message(FATAL_ERROR "[${APP_NAME}] Unsupported compiler used.")
endif()
# Detect core crt.
if(CORE_CRL_MD)
  set(CRT MD)
else()
  set(CRT MT)
endif()

# Add Cubism Core.
# Import as static library.
add_library(Live2DCubismCore STATIC IMPORTED)
# Find library path.
set(CORE_LIB_SUFFIX ${CORE_PATH}/lib/windows/${ARCH}/${COMPILER})
set_target_properties(Live2DCubismCore
  PROPERTIES
    IMPORTED_LOCATION_DEBUG
      ${CORE_LIB_SUFFIX}/Live2DCubismCore_${CRT}d.lib
    IMPORTED_LOCATION_RELWITHDEBINFO
      ${CORE_LIB_SUFFIX}/Live2DCubismCore_${CRT}d.lib
    IMPORTED_LOCATION_RELEASE
      ${CORE_LIB_SUFFIX}/Live2DCubismCore_${CRT}.lib
    IMPORTED_LOCATION_MINSIZEREL
      ${CORE_LIB_SUFFIX}/Live2DCubismCore_${CRT}.lib
    INTERFACE_INCLUDE_DIRECTORIES ${CORE_PATH}/include
)

# Add GLEW ,GLFW.
add_subdirectory(${GLEW_PATH}/build/cmake ${CMAKE_CURRENT_SOURCE_DIR}/gl/glew)
add_subdirectory(${GLFW_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/gl/glfw)
set_target_properties(glew PROPERTIES FOLDER "plugins/${APP_NAME}")
set_target_properties(glew_s PROPERTIES FOLDER "plugins/${APP_NAME}")

# Specify Cubism Framework rendering.
set(FRAMEWORK_SOURCE OpenGL)
# Add Cubism Native Framework.
add_subdirectory(${FRAMEWORK_PATH} ${CMAKE_CURRENT_BINARY_DIR}/Framework)
# Add rendering definition to framework.
target_compile_definitions(Framework PUBLIC CSM_TARGET_WIN_GL)
# Add include path of GLEW to framework.
target_include_directories(Framework PUBLIC ${GLEW_PATH}/include)
# Link libraries to framework.
target_link_libraries(Framework Live2DCubismCore glew_s)

set_target_properties(Framework PROPERTIES FOLDER "plugins/${APP_NAME}")
# Find opengl libraries.
find_package(OpenGL REQUIRED)

set(${APP_NAME}_SOURCES
	vtuber_plugin.cpp
 	VtuberPlugin.cpp
	VtuberFrameWork.cpp  
	VtuberDelegate.cpp
	View.cpp	
	Live2DManager.cpp
	Model.cpp	
	Pal.cpp	
	LAppTextureManager.cpp			    
	LAppAllocator.cpp
        Sprite.cpp
        Hook.cpp
	EventManager.cpp
	Define.cpp
	json.cpp
	InfoReader.cpp
	)

set(${APP_NAME}_HEADERS	
	LAppAllocator.hpp
	Define.hpp
	VtuberDelegate.hpp
	Live2DManager.hpp
	LAppTextureManager.hpp
	Model.hpp
	Pal.hpp
	View.hpp
	VtuberFrameWork.hpp
	VtuberPlugin.hpp
        Sprite.hpp
        Hook.hpp
        EventManager.hpp
	InfoReader.hpp
	json.h
	json-forwards.h
)

#find_package(Thread REQUIRED)

add_library(${APP_NAME} MODULE
	${${APP_NAME}_SOURCES}
	${${APP_NAME}_HEADERS}    	
)

# Link libraries to app.
target_link_libraries(${APP_NAME}
 	libobs
	${LIBFDK_LIBRARIES}
	Framework
	glfw
	${OPENGL_LIBRARIES}
)

# Specify include directories.
target_include_directories(${APP_NAME} PRIVATE ${STB_PATH})

set_target_properties(${APP_NAME} PROPERTIES FOLDER "plugins/${APP_NAME}")

install_obs_plugin_with_data(${APP_NAME} data)


@echo off

set GLEW_VERSION=2.1.0
set GLFW_VERSION=3.3.2

set SCRIPT_PATH=%~dp0

cd %SCRIPT_PATH%/..

::::::::::::::::
:: Setup GLEW ::
::::::::::::::::

:: Download and extract the archive.
echo "- Setup GLEW %GLEW_VERSION%"
echo "Downloading..."
curl -fsSL -o glew.zip ^
  "https://github.com/nigels-com/glew/releases/download/glew-%GLEW_VERSION%/glew-%GLEW_VERSION%.zip"
if %errorlevel% neq 0 pause & exit /b %errorlevel%
echo "Extracting..."
powershell "$progressPreference = 'silentlyContinue'; expand-archive -force glew.zip ."
if %errorlevel% neq 0 pause & exit /b %errorlevel%
mkdir glew >nul 2>&1
pushd .\glew-* && xcopy /y /e /q * ..\glew\ >nul && popd
del glew.zip
for /f %%a in ('dir /ad /b /w glew-*') do (rd /s /q %%a)

::::::::::::::::
:: Setup GLFW ::
::::::::::::::::

echo.

:: Download and extract the archive.
echo "- Setup GLFW %GLFW_VERSION%"
echo Downloading...
curl -fsSL -o glfw.zip ^
  https://github.com/glfw/glfw/releases/download/%GLFW_VERSION%/glfw-%GLFW_VERSION%.zip
if %errorlevel% neq 0 pause & exit /b %errorlevel%
echo "Extracting..."
powershell "$progressPreference = 'silentlyContinue'; expand-archive -force glfw.zip ."
if %errorlevel% neq 0 pause & exit /b %errorlevel%
mkdir glfw >nul 2>&1
pushd glfw-* && xcopy /y /e /q * ..\glfw\ >nul && popd
del glfw.zip
for /f %%a in ('dir /ad /b /w glfw-*') do (rd /s /q %%a)

echo.
pause

#include "Pal.hpp"
#include <cstdio>
#include <stdarg.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Model/CubismMoc.hpp>
#include "Define.hpp"
#include <Windows.h>
#include <io.h>
#include <codecvt>

using std::endl;
using namespace Csm;
using namespace std;
using namespace Define;

csmByte* Pal::LoadFileAsBytes(const string filePath, csmSizeInt* outSize)
{
    //filePath;//
    const char* path = filePath.c_str();

#if _WIN64

    //fix open wchar file path
    int wchars_num = MultiByteToWideChar(CP_UTF8, 0, path, -1, NULL, 0);
    wchar_t *wstr = new wchar_t[wchars_num];
    MultiByteToWideChar(CP_UTF8, 0, path, -1, wstr, wchars_num);

    int size = 0;
    struct _stat64 statBuf;
    if (_wstat64(wstr, &statBuf) == 0)
    {
        size = statBuf.st_size;
    } else {
        size = 0;
    }
    *outSize = size;

#else
    int wchars_num = MultiByteToWideChar(CP_UTF8, 0, path, -1, NULL, 0);
    wchar_t *wstr = new wchar_t[wchars_num];
    MultiByteToWideChar(CP_UTF8, 0, path, -1, wstr, wchars_num);

    int size = 0;
    struct _stat32 statBuf;
    if (_wstat32(wstr, &statBuf) == 0) {
	    size = statBuf.st_size;
    } else {
	    size = 0;
    }
#endif // X64
    std::fstream file;
    char* buf = new char[size];

    file.open(wstr, std::ios::in | std::ios::binary);
    if (!file.is_open())
    {
        return NULL;
    }
    file.read(buf, size);
    file.close();
    delete [] wstr;
    return reinterpret_cast<csmByte*>(buf);
}

void Pal::ReleaseBytes(csmByte* byteData)
{
    delete[] byteData;
}

void Pal::PrintLog(const csmChar *format, ...)
{
    va_list args;
    csmChar buf[256];
    va_start(args, format);
    vsnprintf_s(buf, sizeof(buf), format, args); // 標準出力でレンダリング
#ifdef CSM_DEBUG_MEMORY_LEAKING
// メモリリークチェック時は大量の標準出力がはしり重いのでprintfを利用する
    std::printf(buf);
#else
    std::cerr << buf << std::endl;
#endif
    va_end(args);
}

void Pal::PrintMessage(const csmChar* message)
{
    PrintLog("%s", message);
}

bool Pal::IsFileExist(const char *csDir)
{
	bool re;

	int wchars_num = MultiByteToWideChar(CP_UTF8, 0, csDir, -1, NULL, 0);
	wchar_t *wstr = new wchar_t[wchars_num];
	MultiByteToWideChar(CP_UTF8, 0, csDir, -1, wstr, wchars_num);
	
	int size = 0;
	struct _stat64 statBuf;
	if (_wstat64(wstr, &statBuf) == 0) {
		re = true;
	} else {
		re = false;
	}
	delete[] wstr;

	return re;
}

int Pal::GetAllDirName(const char *csDir, char **Files)
{
	int i = 0;

	//setup converter
	using convert_type =codecvt_utf8<wchar_t>;
	wstring_convert<convert_type,wchar_t>converter;

	int wchars_num = MultiByteToWideChar(CP_UTF8, 0, csDir, -1, NULL, 0);
	wchar_t *wstr = new wchar_t[wchars_num];
	MultiByteToWideChar(CP_UTF8, 0, csDir, -1, wstr, wchars_num);

	wstring Path = wstring(wstr) + L"*";

	HANDLE hFile;
	WIN32_FIND_DATA findFileData;
	string p;
	hFile = FindFirstFile(Path.c_str(), &findFileData);
	if (hFile){
		
		do {
			if ((findFileData.dwFileAttributes == _A_SUBDIR)) {
				if (strcmp((const char *)findFileData.cFileName, ".") != 0 &&
				    strcmp((const char *)findFileData.cFileName,"..") !=0) {
					wstring temp(findFileData.cFileName);
					string converted_str =converter.to_bytes(temp);
					Files[i] = (char*)malloc(converted_str.size()+1);
					Files[i][converted_str.size()] = 0;
					memcpy(Files[i],(const char*)converted_str.c_str(),converted_str.size());
					i++;
				}
			}
		
		} while (FindNextFile(hFile, &findFileData));

		FindClose(hFile);
	}
	return i;
}

void Pal::GetDesktopResolution(int &horizontal, int &vertical) {
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	horizontal = desktop.right;
	vertical = desktop.bottom;
}

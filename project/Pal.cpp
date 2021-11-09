#include "Pal.hpp"
#include <iostream>
#include <fstream>
#include <Model/CubismMoc.hpp>
#include "Define.hpp"
#include <Windows.h>
#include <io.h>
#include <codecvt>
#include <filesystem>

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

bool Pal::IsFileExist(const Csm::csmChar *csDir)
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

int Pal::GetAllDirName(const Csm::csmChar *csDir, Csm::csmChar **Files)
{
	return 0;
}

const char *Pal::GetModelName(const char *filePath)
{
	string _filepath = filePath;
	_filepath += "/*.model3.json";

	struct _finddata_t fileInfo;
	long long findResult = _findfirst(_filepath.c_str(), &fileInfo);
	if (findResult == -1) {
		_findclose(findResult);
		return "";
	}
	_findclose(findResult);

	string _filename = fileInfo.name;
	char *buf = new char[_filename.size()+1];
	buf[_filename.size()] = 0x00;
	memcpy(buf, _filename.c_str(), _filename.size());

	return buf;
}

void Pal::GetDesktopResolution(int &horizontal, int &vertical) {
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	horizontal = desktop.right;
	vertical = desktop.bottom;
}

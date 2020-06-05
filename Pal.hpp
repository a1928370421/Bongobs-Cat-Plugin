#pragma once

#include <CubismFramework.hpp>
#include <string>


class Pal
{
public:
    static Csm::csmByte *LoadFileAsBytes(const std::string filePath,Csm::csmSizeInt *outSize);

    static void ReleaseBytes(Csm::csmByte* byteData);

    static void PrintLog(const Csm::csmChar *format, ...);

    static void PrintMessage(const Csm::csmChar* message);

    static bool IsFileExist(const char *csDir);

    static int GetAllDirName(const char *csDir,char **Files);

    static void GetDesktopResolution(int &horizontal, int &vertical);
};


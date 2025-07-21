#include <Windows.h>
#include "Debug.h"

void Debug::Log(int value, bool isLineFeed)
{
    char str[256];
    sprintf_s(str, "%d", value);
    Debug::Log(str, isLineFeed);
}

void Debug::Log(float value, bool isLineFeed)
{
    char str[256];
    sprintf_s(str, "%f", value);
    Debug::Log(str, isLineFeed);
}

void Debug::Log(const char* value, bool isLineFeed)
{
    int len = MultiByteToWideChar(CP_ACP, 0, value, -1, NULL, 0);
    std::wstring wvalue(len - 1, L'\0');
    MultiByteToWideChar(CP_ACP, 0, value, -1, &wvalue[0], len - 1);
    Debug::Log(wvalue, isLineFeed);
}

void Debug::Log(std::wstring value, bool isLineFeed)
{
    if (isLineFeed) value += L"\n";
    OutputDebugStringW(value.c_str());
}
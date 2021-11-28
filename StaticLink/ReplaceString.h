#ifndef REPLACE_STRING_DLL
#define REPLACE_STRING_DLL

#include "pch.h"
#include <vector>

#ifdef EXPORTING_DLL
	extern "C" __declspec(dllexport) void replace_string(LPCTSTR searchString, LPCSTR replaceString);
#else
	extern "C" __declspec(dllimport) void replace_string(LPCSTR searchStr, LPCSTR replaceStr);
#endif

#endif
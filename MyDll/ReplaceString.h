#ifndef REPLACE_STRING_DLL
#define REPLACE_STRING_DLL

#include "pch.h"

#ifdef EXPORTING_DLL
	extern "C" __declspec(dllexport) void replace_string(LPCTSTR searchString, LPCSTR replaceString);
	extern "C" __declspec(dllimport) void replace_strings(LPVOID strings);
#else
	extern "C" __declspec(dllimport) void replace_string(LPCSTR searchStr, LPCSTR replaceStr);
	extern "C" __declspec(dllimport) void replace_strings(LPVOID strings);
#endif

#endif
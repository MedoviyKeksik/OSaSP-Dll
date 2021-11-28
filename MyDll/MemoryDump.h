#ifndef MEMORY_DUMP_DLL
#define MEMORY_DUMP_DLL

#include "pch.h"

#ifdef EXPORTING_DLL
	extern "C" __declspec(dllexport) void memory_dump(LPCSTR filename);
#else
	extern "C" __declspec(dllimport) void memory_dump(LPCSTR filename);
#endif

#endif
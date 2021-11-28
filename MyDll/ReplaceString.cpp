#include "pch.h"
#define EXPORTING_DLL
#include "ReplaceString.h"
#include <iostream>
#include <exception>
#include <vector>

void replace_string(LPCSTR searchString, LPCSTR replaceString) {
	HANDLE hProcess = GetCurrentProcess();
	if (hProcess) {
		SYSTEM_INFO systemInfo;
		GetSystemInfo(&systemInfo);
		MEMORY_BASIC_INFORMATION memInfo;
		std::vector<char> buffer;
		char* pointer = 0;
		while (pointer < systemInfo.lpMaximumApplicationAddress) {
			if (VirtualQueryEx(hProcess, pointer, &memInfo, sizeof(memInfo)) == sizeof(memInfo)) {
				if (memInfo.State == MEM_COMMIT && memInfo.AllocationProtect == PAGE_READWRITE) {
					pointer = (char*)memInfo.BaseAddress;
					buffer.resize(memInfo.RegionSize);
					SIZE_T read;
					try {
						if (ReadProcessMemory(hProcess, pointer, &buffer[0], memInfo.RegionSize, &read)) {
							for (size_t i = 0; i < (read - strlen(searchString)); i++) {
								if (searchString == (LPCSTR)&buffer[i]) continue;
								if (memcmp(searchString, &buffer[i], strlen(searchString)) == 0) {
									char* tmp = (char*)pointer + i;
									for (int j = 0; j < strlen(replaceString); j++) {
										tmp[j] = replaceString[j];
									}
									tmp[strlen(replaceString)] = 0;
								}
							}
						} else {
							DWORD lastError = GetLastError();
							std::cerr << "Error: " << lastError << std::endl;
						}
					} catch (std::bad_alloc& e)
					{
					}
				}
			} else {
				DWORD lastError = GetLastError();
				std::cerr << "Error: " << lastError << std::endl;
			}
			pointer += memInfo.RegionSize;
		}
	}
}

void replace_strings(LPVOID strings) {
	char* src_str = (char*)strings;
	char* res_str = (char*)strings + strlen((char*)strings) + 1;
	replace_string(src_str, res_str);
}
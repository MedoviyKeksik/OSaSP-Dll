#define EXPORTING_DLL
#include "pch.h"
#include "MemoryDump.h"
#include <iostream>
#include <exception>
#include <vector>
#include <fstream>

void memory_dump(LPCSTR filename) {
	HANDLE hProcess = GetCurrentProcess();
	if (hProcess) {
		SYSTEM_INFO systemInfo;
		GetSystemInfo(&systemInfo);
		MEMORY_BASIC_INFORMATION memInfo;
		std::vector<char> buffer;
		std::ofstream fout(filename);
		char* pointer = 0;
		while (pointer < systemInfo.lpMaximumApplicationAddress) {
			if (VirtualQueryEx(hProcess, pointer, &memInfo, sizeof(memInfo)) == sizeof(memInfo)) {
				if (memInfo.State == MEM_COMMIT && memInfo.AllocationProtect == PAGE_READWRITE) {
					pointer = (char*)memInfo.BaseAddress;
					buffer.resize(memInfo.RegionSize);
					SIZE_T read;
					try {
						if (ReadProcessMemory(hProcess, pointer, &buffer[0], memInfo.RegionSize, &read)) {
							for (size_t i = 0; i < read; i++) {
								fout << buffer[i];
							}
						} else {
							DWORD lastError = GetLastError();
							std::cerr << "Error: " << lastError << std::endl;
						}
					} catch (std::bad_alloc) {
					}
				}
			} else {
				DWORD lastError = GetLastError();
				std::cerr << "Error: " << lastError << std::endl;
			}
			pointer += memInfo.RegionSize;
		}
		fout.flush();
		fout.close();
	}
}
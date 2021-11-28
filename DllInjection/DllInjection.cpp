#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <io.h>
#include <string>
#include <fstream>

struct arguments {
    std::wstring arg1;
    std::wstring arg2;
};

DWORD GetProcessByName(wchar_t* processName) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    PROCESSENTRY32W process;
    process.dwSize = sizeof(PROCESSENTRY32W);
    DWORD procId = 0;
    if (Process32First(snapshot, &process))
    {
        while (Process32Next(snapshot, &process))
        {
            if (wcscmp(process.szExeFile, processName) == 0)
            {
                procId = process.th32ProcessID;
                break;
            }
        }
    }
    CloseHandle(snapshot);
    return procId;
}

bool FileExist(wchar_t* name)
{
    return _waccess(name, 0) != -1;
}

bool Inject(DWORD pid, wchar_t* path, LPVOID args, DWORD argsSize)
{
    HANDLE hProcess;
    LPVOID LibraryToLoad;
    LPCVOID LoadLibraryFunc;
    if (pid == 0)
        return false;

    hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
    if (hProcess == 0)
        return false;

    LoadLibraryFunc = GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryW");
    LibraryToLoad = VirtualAllocEx(hProcess, NULL, wcslen(path) * sizeof(wchar_t), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    SIZE_T written;
    WriteProcessMemory(hProcess, LibraryToLoad, path, wcslen(path) * sizeof(wchar_t), &written);
    HANDLE threadId = CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibraryFunc, LibraryToLoad, NULL, NULL);
    if (threadId == 0) {
        DWORD error = GetLastError();
        std::cerr << "ERROR #" << error << " OCCURED";
        CloseHandle(hProcess);
        return false;
    } else {
        DWORD result = 0;
        WaitForSingleObject(threadId, INFINITE);
        GetExitCodeThread(threadId, &result);
        CloseHandle(threadId);
        if (result != 0) {
            HMODULE hMyLib = LoadLibraryW(path);
            if (hMyLib != NULL) {
                auto functionAddr = GetProcAddress(hMyLib, "replace_strings");
                if (functionAddr != NULL) {
                    auto remoteFuntion = result + ((DWORD)functionAddr - (DWORD)hMyLib);
                    LPVOID argumentsAddr = (LPVOID)VirtualAllocEx(hProcess, NULL, argsSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
                    WriteProcessMemory(hProcess, argumentsAddr, args, argsSize, NULL);
                    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)remoteFuntion, (LPVOID)argumentsAddr, NULL, NULL);
                    WaitForSingleObject(hThread, INFINITE);
                }
            }
            FreeLibrary(hMyLib);
        }
        else {
            DWORD error = GetLastError();
            std::cerr << "ERROR #" << error << " OCCURED";
            CloseHandle(hProcess);
            return false;
        }
    }
    CloseHandle(hProcess);
    return true;
}

int main() {
    wchar_t process_name[32];
    wchar_t dll_name[32];
    wchar_t path[256];
    // FORMAT "<SEARCH_STRING>\0<REPLACE_STRING>"
    const char* args_str = "string\0STRING";
    DWORD args_size = 14;
    LPVOID args = malloc(sizeof(char) * args_size);
    memcpy(args, args_str, args_size);
    std::wcout << L"Enter process name: "; // SomeApplication.exe
    std::wcin >> process_name;
    DWORD pID = GetProcessByName(process_name);
    std::wcout << L"Waiting " << process_name << L" for start..." << std::endl;
    for (;; Sleep(50))
    {
        if (pID == 0)
            pID = GetProcessByName(process_name);
        if (pID != 0) break;
    }

    std::wcout << process_name << L" found(pid = " << pID << L")" << std::endl;
    while (FileExist(path) == false)
    {
        std::cout << "Enter DLL name: "; // MyDll.dll
        std::wcin >> dll_name;
        GetFullPathNameW(dll_name, sizeof(path) / sizeof(wchar_t), path, NULL);
        if (FileExist(path))
        {
            break;
        }
        else
            std::cout << "DLL not found" << std::endl;
    }

    std::cout << "Preparing DLL for injection" << std::endl;
    if (Inject(pID, path, args, args_size)) {
        std::cout << "DLL injected" << std::endl;
    } else {
        std::cout << "error ocurred" << std::endl;
    }
    std::cin.get();
    return 0;
}

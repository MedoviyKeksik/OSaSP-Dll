#include <iostream>
#include <vector>
#include <Windows.h>

typedef void (__cdecl *DumpMemory) (LPCSTR filename);
typedef void (__cdecl *ReplaceStringProc)(LPCSTR searchString, LPCSTR replaceString);

int main() {
    HINSTANCE dll = LoadLibrary("MyDll.dll");
    if (dll != NULL) {
        char search_str[] =  "Some string";
        char replace_str[] = "STRING REPL";
        char some_str[] = "Some string  and Some string  Some another text";
        int len = strlen(some_str);

        ReplaceStringProc replace_string = (ReplaceStringProc)GetProcAddress(dll, "replace_string");
        DumpMemory dump_memory = (DumpMemory)GetProcAddress(dll, "memory_dump");
        dump_memory("D:\\before.txt");
        replace_string(search_str, replace_str);
        dump_memory("D:\\after.txt");
        std::cout << std::string(some_str, some_str + len);
        FreeLibrary(dll);
    }
}

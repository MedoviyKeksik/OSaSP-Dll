#include <iostream>
#include "ReplaceString.h"

int main() {
    std::string search_str = "some string";
    std::string replace_str = "string repl";
    replace_string(search_str.c_str(), replace_str.c_str());
    std::cout << search_str;    
    return 0;
}
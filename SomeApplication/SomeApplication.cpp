#include <iostream>
#include <conio.h>

int main() {
	char srcString[] = "old string for injection";
	int len = strlen(srcString);
	_getch();
	std::cout << std::string(srcString, srcString + len) << "\n";
	_getch();
}
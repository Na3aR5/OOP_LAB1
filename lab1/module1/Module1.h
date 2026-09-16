#ifndef LAB1_MODULE1_HEADER
#define LAB1_MODULE1_HEADER

#include <Windows.h>

namespace lab {
	struct ModuleWorkResult1 {
		bool wasChosen;
		char result[16];
	};
	int ModuleWork1(HWND hwnd, ModuleWorkResult1* result);
} // namespace lab
#endif // !LAB1_MODULE1_HEADER
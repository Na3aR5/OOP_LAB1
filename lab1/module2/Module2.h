#ifndef LAB1_MODULE2_HEADER
#define LAB1_MODULE2_HEADER

#include <Windows.h>
#include <string>

namespace lab {
	struct ModuleWorkResult2 {
		std::string written;
	};

	int ModuleWork2(HWND hwnd, ModuleWorkResult2* result);
} // namespace lab
#endif // !LAB1_MODULE2_HEADER
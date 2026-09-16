#include <module2/Module2.h>
#include <module2/ModuleResource2.h>

struct DialogResult {
public:
	int exitCode = 0;
	std::string written;
}; // struct DialogResult

namespace {
	void HandleOnOK(HWND hdlg, DialogResult* result);
	void HandleOnCancel(HWND hdlg, DialogResult* result);
	INT_PTR CALLBACK CallbackWork2(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam);
}

int lab::ModuleWork2(HWND hwnd, ModuleWorkResult2* result) {
	DialogResult dialogResult = {};

	DialogBoxParamA(
		GetModuleHandleA(nullptr),
		MAKEINTRESOURCEA(IDR_MODULE2_DIALOG),
		hwnd,
		CallbackWork2,
		(LPARAM)&dialogResult
	);

	result->written = std::move(dialogResult.written);

	return dialogResult.exitCode;
}

namespace {
	void HandleOnOK(HWND hdlg, DialogResult* result) {
		char buffer[128];
		GetDlgItemTextA(hdlg, IDD_MODULE2_TEXT_INPUT, buffer, sizeof(buffer));
		result->written = buffer;
		result->exitCode = 1;
	}

	void HandleOnCancel(HWND hdlg, DialogResult* result) {
		result->exitCode = 0;
		result->written = {};
	}

	INT_PTR CALLBACK CallbackWork2(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam) {
		switch (msg) {
		case WM_INITDIALOG: {
			DialogResult* result = (DialogResult*)lParam;
			SetWindowLongPtrA(hdlg, DWLP_USER, (LONG_PTR)result);
			return (INT_PTR)TRUE;
		}

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
			case IDOK:
				HandleOnOK(hdlg, (DialogResult*)GetWindowLongPtrA(hdlg, DWLP_USER));
				EndDialog(hdlg, IDOK);
				return (INT_PTR)TRUE;

			case IDCANCEL: {
				HandleOnCancel(hdlg, (DialogResult*)GetWindowLongPtrA(hdlg, DWLP_USER));
				EndDialog(hdlg, IDCANCEL);
				return (INT_PTR)TRUE;
			}
			}
			break;
		}
		return (INT_PTR)FALSE;
	}
}
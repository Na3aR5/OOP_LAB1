#include <module1/Module1.h>
#include <module1/ModuleResource1.h>

#include <vector>
#include <string>

struct DialogResult {
public:
	bool wasChosen		= false;
	int  exitCode		= 0;
	char chosenName[16] = {};
}; // struct DialogResult

namespace {
	void HandleOnOK(HWND hdlg, DialogResult* result);
	void HandleOnCancel(HWND hdlg, DialogResult* result);
	INT_PTR CALLBACK CallbackWork1(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam);

	std::vector<std::string_view> GroupNames = {
		"IM-51",
		"IM-52",
		"IM-53",
		"IM-54",
		"IM-55"
	};
}

int lab::ModuleWork1(HWND hwnd, ModuleWorkResult1* result) {
	DialogResult dialogResult = {};

	DialogBoxParamA(
		GetModuleHandleA(nullptr),
		MAKEINTRESOURCEA(IDR_MODULE1_DIALOG),
		hwnd,
		CallbackWork1,
		(LPARAM)&dialogResult
	);

	result->wasChosen = dialogResult.wasChosen;
	std::memcpy(result->result, dialogResult.chosenName, std::strlen(dialogResult.chosenName) + 1);
	return dialogResult.exitCode;
}

namespace {
	void HandleOnOK(HWND hdlg, DialogResult* result) {
		result->exitCode = 1;

		LRESULT index = SendDlgItemMessageA(hdlg, IDD_MODULE1_LISTBOX, LB_GETCURSEL, 0, 0);
		SendDlgItemMessageA(hdlg, IDD_MODULE1_LISTBOX, LB_GETTEXT, index, (LPARAM)result->chosenName);

		result->wasChosen = true;
	}

	void HandleOnCancel(HWND hdlg, DialogResult* result) {
		result->exitCode = 0;
		result->wasChosen = false;
	}

	INT_PTR CALLBACK CallbackWork1(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam) {
		switch (msg) {
			case WM_INITDIALOG: {
				DialogResult* result = (DialogResult*)lParam;
				SetWindowLongPtrA(hdlg, DWLP_USER, (LONG_PTR)result);

				for (std::string_view groupName : GroupNames) {
					SendDlgItemMessageA(hdlg, IDD_MODULE1_LISTBOX, LB_ADDSTRING, 0, (LPARAM)groupName.data());
				}
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
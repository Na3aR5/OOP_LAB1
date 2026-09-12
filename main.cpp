#include <string>

#include <mainres.h>
#include <module1/Module1.h>
#include <module2/Module2.h>

struct State {
public:
	bool shouldExecuteWorkResult1 = false;
	bool shouldExecuteWorkResult2 = false;
	lab::ModuleWorkResult1 workResultPayload1 = {};
	lab::ModuleWorkResult2 workResultPayload2 = {};
}; // struct State

namespace {
	std::string_view LAB_WINDOW_CLASSNAME = "LabWindowClass";

	ATOM LabRegisterClass(HINSTANCE hInstance);
	bool LabCreateWindow(HINSTANCE hInstance, int nCmdShow, State* state);
	LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
}

int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ PWSTR pCmdLine,
	_In_ int nCmdShow)
{
	State state = {};

	if (!LabRegisterClass(hInstance)) {
		return -1;
	}
	if (!LabCreateWindow(hInstance, nCmdShow, &state)) {
		return -1;
	}

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

namespace {
	ATOM LabRegisterClass(HINSTANCE hInstance) {
		WNDCLASSEXA windowClass   = {};
		windowClass.cbSize		  = sizeof(windowClass);
		windowClass.hInstance     = hInstance;
		windowClass.lpszClassName = LAB_WINDOW_CLASSNAME.data();
		windowClass.lpfnWndProc   = WndProc;
		windowClass.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));

		return RegisterClassExA(&windowClass);
	}

	bool LabCreateWindow(HINSTANCE hInstance, int nCmdShow, State* state) {
		HWND hwnd = CreateWindowExA(
			0,
			LAB_WINDOW_CLASSNAME.data(),
			"Lab1",
			WS_OVERLAPPEDWINDOW,
			200, 200,
			720, 480,
			NULL,
			LoadMenuA(hInstance, MAKEINTRESOURCEA(IDR_MAIN_MENU)),
			hInstance,
			state
		);
		ShowWindow(hwnd, nCmdShow);

		return hwnd != NULL;
	}

	LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		switch (msg) {
			case WM_NCCREATE: {
				CREATESTRUCTW* createStruct = (CREATESTRUCTW*)lParam;
				State* state = (State*)createStruct->lpCreateParams;

				SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)state);
				return DefWindowProcA(hwnd, msg, wParam, lParam);
			}

			case WM_COMMAND:
				switch (LOWORD(wParam)) {
					case IDM_WORK_1: {
						State* state = (State*)GetWindowLongPtrA(hwnd, GWLP_USERDATA);
						lab::ModuleWork1(hwnd, &state->workResultPayload1);
						state->shouldExecuteWorkResult1 = true;

						if (state->workResultPayload1.wasChosen && std::strlen(state->workResultPayload1.result) > 0) {
							InvalidateRect(hwnd, nullptr, TRUE);
						}

						break;
					}

					case IDM_WORK_2: {
						State* state = (State*)GetWindowLongPtrA(hwnd, GWLP_USERDATA);
						lab::ModuleWork2(hwnd, &state->workResultPayload2);
						state->shouldExecuteWorkResult2 = true;

						if (!state->workResultPayload2.written.empty()) {
							InvalidateRect(hwnd, nullptr, TRUE);
						}

						break;
					}

					case IDM_EXIT:
						DestroyWindow(hwnd);
						break;
				}
				break;

			case WM_PAINT: {
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hwnd, &ps);

				State* state = (State*)GetWindowLongPtrA(hwnd, GWLP_USERDATA);

				if (state->shouldExecuteWorkResult1) {
					std::string message = std::string("Work1: Chosen group is ") + state->workResultPayload1.result;
					TextOutA(hdc, 200, 200, message.data(), message.length());
					state->shouldExecuteWorkResult1 = false;
				}
				if (state->shouldExecuteWorkResult2) {
					std::string message = std::string("Work2: Entered text is \"") + state->workResultPayload2.written + "\"";
					TextOutA(hdc, 200, 200, message.data(), message.length());
					state->shouldExecuteWorkResult2 = false;
				}

				EndPaint(hwnd, &ps);
				break;
			}

			case WM_CLOSE:
				DestroyWindow(hwnd);
				break;

			case WM_DESTROY:
				PostQuitMessage(0);
				break;

			default:
				return DefWindowProc(hwnd, msg, wParam, lParam);
		}
		return 0;
	}
}
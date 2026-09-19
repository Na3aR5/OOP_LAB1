#include <Window.h>

lab::Window::Window(const WindowCreateInfo& info, WindowError& error) {
	WNDCLASSEXW windowClass = {};
	windowClass.cbSize		  = sizeof(windowClass);
	windowClass.hInstance	  = info.hInstance;
	windowClass.lpszClassName = info.windowClassName.data();
	windowClass.lpfnWndProc   = _WndProc;

	if (!RegisterClassExW(&windowClass)) {
		error.hasError = true;
		error.errorInfo = "Failed to register window class";
		return;
	}
	HWND hwnd = CreateWindowExW(
		0,
		info.windowClassName.data(),
		info.windowTitle.data(),
		WS_OVERLAPPEDWINDOW,
		info.pos.x,
		info.pos.y,
		info.size.cx,
		info.size.cy,
		NULL,
		info.intMenu == UINT_MAX ? NULL : LoadMenuW(info.hInstance, MAKEINTRESOURCEW(info.intMenu)),
		info.hInstance,
		this
	);
	if (hwnd == NULL) {
		error.hasError = true;
		error.errorInfo = "Failed to create window instance";
		return;
	}

	m_backBuffer.Create(hwnd);
	m_clearColor = CreateSolidBrush(info.clearColor);

	m_windowCloseListener = info.eventSystem->StartListen<WindowCloseEvent>(this,
		[](void* ctx, const WindowCloseEvent& e) -> EventResult {
			Window* window = (Window*)ctx;
			window->m_state &= ~_State::Started;
			return EventResult::Propagate;
		}
	);

	ShowWindow(hwnd, info.nCmdShow);

	m_windowHandle = hwnd;
	m_state |= _State::Started;
	m_eventSystem = info.eventSystem;

	error.hasError = false;
	error.errorInfo = {};
}

lab::Window::~Window() {
	if (m_windowCloseListener != EventSystem::EventListenerHandle::Null()) {
		m_eventSystem->StopListen<WindowCloseEvent>(m_windowCloseListener);
	}

	DeleteObject(m_clearColor);
	m_backBuffer.Destroy();

	if (m_windowHandle != NULL) {
		DestroyWindow(m_windowHandle);
	}
}

void lab::Window::Clear() {
	m_backBuffer.Clear(m_clearColor);
}

void lab::Window::Present() {
	HDC frontDC = GetDC(m_windowHandle);
	m_backBuffer.Present(frontDC);
	ReleaseDC(m_windowHandle, frontDC);
}

void lab::Window::PollEvents() {
	MSG msg;
	while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
}

void lab::Window::ChangeTitle(std::wstring_view newTitle) {
	SetWindowTextW(m_windowHandle, newTitle.data());
}

LRESULT lab::Window::_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (msg == WM_NCCREATE) {
		CREATESTRUCTW* createStruct = (CREATESTRUCTW*)lParam;
		SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)createStruct->lpCreateParams);
		return DefWindowProcW(hwnd, msg, wParam, lParam);
	}

	Window* window = (Window*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);

	switch (msg) {
		case WM_COMMAND:
			window->m_eventSystem->RegisterEvent<WindowWMCommandEvent>(window, LOWORD(wParam));
			break;

		case WM_MOUSEMOVE: {
			POINT pos = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			window->m_eventSystem->RegisterEvent<WindowCursorMoveEvent>(window, pos);
			break;
		}

		case WM_LBUTTONDOWN: {
			POINT pos = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			window->m_eventSystem->RegisterEvent<WindowMouseButtonEvent>(window, pos, MouseButton::Left, true);
			break;
		}

		case WM_LBUTTONUP: {
			POINT pos = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			window->m_eventSystem->RegisterEvent<WindowMouseButtonEvent>(window, pos, MouseButton::Left, false);
			break;
		}

		case WM_RBUTTONDOWN: {
			POINT pos = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			window->m_eventSystem->RegisterEvent<WindowMouseButtonEvent>(window, pos, MouseButton::Right, true);
			break;
		}

		case WM_RBUTTONUP: {
			POINT pos = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			window->m_eventSystem->RegisterEvent<WindowMouseButtonEvent>(window, pos, MouseButton::Right, false);
			break;
		}

		case WM_CLOSE:
			window->m_eventSystem->RegisterEvent<WindowCloseEvent>(window);
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;
	}
	return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void lab::Window::BackBuffer::Create(HWND hwnd) {
	HDC hdc = GetDC(hwnd);
	m_hdc = CreateCompatibleDC(hdc);

	RECT clientRect;
	GetClientRect(hwnd, &clientRect);

	m_size = SIZE{
		clientRect.right - clientRect.left,
		clientRect.bottom - clientRect.top
	};

	m_bitmap = CreateCompatibleBitmap(hdc, m_size.cx, m_size.cy);
	m_oldBitmap = (HBITMAP)SelectObject(m_hdc, m_bitmap);

	ReleaseDC(hwnd, hdc);
}

void lab::Window::BackBuffer::Destroy() {
	if (m_hdc != NULL) {
		SelectObject(m_hdc, m_oldBitmap);
		DeleteDC(m_hdc);
	}
	if (m_bitmap != NULL) {
		DeleteObject(m_bitmap);
	}
}

void lab::Window::BackBuffer::Clear(HBRUSH color) {
	RECT fillRect = { 0, 0, m_size.cx, m_size.cy };
	FillRect(m_hdc, &fillRect, color);
}

void lab::Window::BackBuffer::Present(HDC frontDC) {
	BitBlt(frontDC, 0, 0, m_size.cx, m_size.cy, m_hdc, 0, 0, SRCCOPY);
}
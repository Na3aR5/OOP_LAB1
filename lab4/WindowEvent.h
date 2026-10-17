#ifndef LAB2_WINDOW_EVENT_HEADER
#define LAB2_WINDOW_EVENT_HEADER

#include <Framework.h>

namespace lab {
	class Window;

	class WindowCloseEvent {
	public:
		WindowCloseEvent(Window* window) : m_window(window) {}

	private:
		Window* m_window;
	}; // class WindowCloseEvent

	class WindowResizeEvent {
	public:
		WindowResizeEvent(Window* window, SIZE size) : m_window(window), m_size(size) {}
		inline SIZE GetSize() const noexcept { return m_size; }

	private:
		Window* m_window;
		SIZE    m_size;
	}; // class WindowResizeEvent

	class WindowWMCommandEvent {
	public:
		WindowWMCommandEvent(Window* window, WORD loword) : m_window(window), m_loword(loword) {}
		inline Window* GetWindow() const noexcept { return m_window; }
		inline WORD GetLOWORD() const noexcept { return m_loword; }

	private:
		WORD m_loword;
		Window* m_window;
	}; // class WindowWMCommandEvent
	
	enum class MouseButton {
		Left,
		Right
	}; // enum class MouseButton

	class WindowMouseButtonEvent {
	public:
		WindowMouseButtonEvent(Window* window, POINT pos, MouseButton mb, bool pressed) :
			m_window(window), m_mouseButton(mb), m_pressed(pressed), m_pos(pos) {}

		inline MouseButton GetMouseButton() const noexcept { return m_mouseButton; }
		inline bool IsPressed() const noexcept { return m_pressed; }
		inline POINT GetPos() const noexcept { return m_pos; }

	private:
		bool	    m_pressed;
		MouseButton m_mouseButton;
		POINT		m_pos;
		Window*		m_window;
	}; // class WindowMouseButtonEvent

	class WindowCursorMoveEvent {
	public:
		WindowCursorMoveEvent(Window* window, POINT pos) : m_window(window), m_pos(pos) {}
		inline POINT GetPos() const noexcept { return m_pos; }

	private:
		POINT   m_pos;
		Window* m_window;
	}; // class WindowCursorMoveEvent

	class WindowCursorLeaveEvent {
	public:
		WindowCursorLeaveEvent(Window* window) : m_window(window) {}

	private:
		Window* m_window;
	}; // class WindowCursorLeaveEvent
} // namespace lab
#endif // !LAB2_WINDOW_EVENT_HEADER
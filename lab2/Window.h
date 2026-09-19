#ifndef LAB2_WINDOW_HEADER
#define	LAB2_WINDOW_HEADER

#include <Framework.h>
#include <WindowEvent.h>
#include <EventSystem.h>

#include <string>

namespace lab {
	struct WindowError {
		bool		hasError  = false;
		std::string errorInfo = {};
	}; // struct WindowError

	struct WindowCreateInfo {
		int				  nCmdShow		  = 0;
		UINT			  intMenu		  = UINT_MAX;
		COLORREF          clearColor	  = RGB(255, 255, 255);
		HINSTANCE		  hInstance		  = NULL;
		POINT			  pos			  = { 0, 0 };
		SIZE              size			  = {};
		EventSystem*	  eventSystem	  = nullptr;
		std::wstring_view windowClassName = {};
		std::wstring_view windowTitle	  = {};
	}; // struct WindowCreateInfo

	class Window {
	public:
		class BackBuffer {
		public:
			BackBuffer() = default;
			BackBuffer(const BackBuffer&) = delete;
			BackBuffer(BackBuffer&&) noexcept = delete;
			~BackBuffer() { Destroy(); }

			BackBuffer& operator=(const BackBuffer&) = delete;
			BackBuffer& operator=(BackBuffer&&) noexcept = delete;

		public:
			inline HDC GetHDC() const noexcept { return m_hdc; }
			void Create(HWND hwnd);
			void Destroy();
			void Clear(HBRUSH color);
			void Present(HDC frontDC);

		private:
			SIZE	m_size	    = {};
			HDC		m_hdc       = NULL;
			HBITMAP m_bitmap    = NULL;
			HBITMAP m_oldBitmap = NULL;
		}; // class BackBuffer

	public:
		Window(const WindowCreateInfo& info, WindowError& error);
		~Window();

	public:
		void Clear();
		void Present();
		void PollEvents();
		void ChangeTitle(std::wstring_view newTitle);

		inline HDC GetHDC() const noexcept { return m_backBuffer.GetHDC(); }
		inline bool IsStillActive() const noexcept { return m_state & _State::Started; }

	private:
		static LRESULT CALLBACK _WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	private:
		enum _State : uint8_t {
			Started = 0x1
		};
		std::underlying_type_t<_State>   m_state = 0;

		HWND							 m_windowHandle		   = NULL;
		HBRUSH                           m_clearColor		   = NULL;
		EventSystem*					 m_eventSystem		   = nullptr;
		EventSystem::EventListenerHandle m_windowCloseListener = EventSystem::EventListenerHandle::Null();
		BackBuffer						 m_backBuffer		   = {};
	}; // class Window
} // namespace lab
#endif // !LAB2_WINDOW_HEADER
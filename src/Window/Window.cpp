#include <Window/Window.hpp>

#include <Window/WindowHandle.hpp>
#include <Window/Keyboard.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#include <stdexcept>

namespace Nth {
	Window::Window() :
		m_handle(),
		m_size(0, 0),
		m_is_running(false) {}

	Window::Window(const std::string& title, unsigned x, unsigned y, unsigned w, unsigned h, uint32_t flags) :
		m_handle(SDL_CreateWindow(title.data(), x, y, w, h, flags)),
		m_size(w, h),
		m_is_running(true) {
		if (!m_handle) {
			throw std::runtime_error("Failed to create SDL Window" + std::string(SDL_GetError()));
		}
	}

	Window::~Window() {
		if (m_handle) {
			SDL_DestroyWindow(m_handle.get());
		}
	}

	WindowHandle Window::handle() {
		assert(m_handle);

		SDL_SysWMinfo info;
		SDL_VERSION(&info.version);
		if (!SDL_GetWindowWMInfo(m_handle.get(), &info)) {
			throw std::runtime_error("Can't retrived window infos");
		}

		WindowHandle handle;
		switch (info.subsystem) {
		#if defined(SDL_VIDEO_DRIVER_X11)
		case SDL_SYSWM_X11:
			handle.subsystem = WindowProtocol::X11;
			handle.x11.dpy = info.info.x11.display;
			handle.x11.window = info.info.x11.window;
			break;

		#elif defined(SDL)
		case SDL_SYSWM_COCOA: // MacOSX Not supported
			handle.subsystem = WindowProtocol::Unknow;
			break;

		#elif defined(SDL_VIDEO_DRIVER_WAYLAND)
		case SDL_SYSWM_WAYLAND: // Wayland not supported yet
			handle.subsystem = WindowProtocol::Unknow;
			break;

		#elif defined(SDL_VIDEO_DRIVER_WINDOWS)
		case SDL_SYSWM_WINDOWS:
			handle.subsystem = WindowProtocol::Windows;
			handle.windows.hinstance = info.info.win.hinstance;
			handle.windows.hwnd = info.info.win.window;
			break;

		#endif
		default:
			handle.subsystem = WindowProtocol::Unknow;
			break;
		}

		return handle;
	}

	bool Window::is_open() const {
		return m_is_running;
	}

	void Window::poll_event() {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				m_is_running = false;
				break;
			case SDL_WINDOWEVENT:
				if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
					m_size = Vector2ui(event.window.data1, event.window.data2);
				}
				break;
			case SDL_KEYDOWN:
				m_input_handler.handle(key_from_sdl(event.key.keysym.sym));
				break;
			default:
				break;
			}
		}
	}

	void Window::set_resizable() {
		SDL_SetWindowResizable(m_handle.get(), SDL_TRUE);
	}

	InputHandler& Window::input_handler() {
		return m_input_handler;
	}

	const Vector2ui& Window::size() const {
		return m_size;
	}

	void Window::Init() {
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			throw std::runtime_error("Failed to init SDL VIDEO : " + std::string(SDL_GetError()));
		}
	}
}
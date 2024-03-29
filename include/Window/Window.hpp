#ifndef NTH_WINDOW_WINDOW_HPP
#define NTH_WINDOW_WINDOW_HPP

#include <string>

#include <SDL2/SDL_video.h>

#include <Maths/Vector2.hpp>
#include <Utils/MovablePtr.hpp>
#include <Window/InputHandler.hpp>

namespace Nth {
	struct WindowHandle;

	class Window {
	public:
		Window();
		Window(const std::string& title, unsigned x, unsigned y, unsigned w, unsigned h, uint32_t flags);
		Window(Window&&) = default;
		Window(const Window&) = delete;
		~Window();

		WindowHandle handle();
		bool is_open() const;
		void poll_event();
		void set_resizable();
		InputHandler& input_handler();
		const Vector2ui& size() const;

		Window& operator=(Window&&) = default;
		Window& operator=(const Window&) = delete;

		static void Init();
	private:
		MovablePtr<SDL_Window> m_handle;
		InputHandler m_input_handler;
		bool m_is_running;
		Vector2ui m_size;
		
	};
}

#endif
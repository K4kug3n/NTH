#ifndef NTH_WINDOW_WINDOW_HPP
#define NTH_WINDOW_WINDOW_HPP

#include <Window/VideoMode.hpp>
#include <Window/WindowEvent.hpp>
#include <Window/EventHandler.hpp>
#include <Window/WindowHandle.hpp>

#include <Math/Vector2.hpp>

#include <SDL2/SDL_video.h>
#include <SDL2/SDL_events.h>

#include <string_view>

namespace Nth {

	class Window
	{
	public:
		Window();
		Window(VideoMode const& mode, const std::string_view title);
		~Window();

		static bool initialize();
		static void uninitialize();
		static void setRelativeMouseMode(bool enabled);

		bool create(VideoMode const& mode, const std::string_view title);
		void close();
		void setTitle(std::string const& title);

		bool isOpen();

		void processEvent() const;

		Vector2ui size() const;
		EventHandler& getEventHandler();
		WindowHandle getHandle() const;

	private:
		static int handleInput(void* userdata, SDL_Event* event);

		void handleEvent(WindowEvent const& event);
		void destroy();

		EventHandler m_eventHandler;
		SDL_Window * m_handle;
		bool m_closed;
		Vector2ui m_size;
	};

}

#endif
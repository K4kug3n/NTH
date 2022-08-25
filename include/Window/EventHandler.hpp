#ifndef NTH_WINDOW_EVENTHANDLER_HPP
#define NTH_WINDOW_EVENTHANDLER_HPP

#include <Util/Signal.hpp>
#include <Window/WindowEvent.hpp>

namespace Nth {
	class EventHandler {
	public:
		EventHandler() = default;
		~EventHandler() = default;

		void dispatch(WindowEvent const& event);

		Signal<> onQuit;
		Signal<> onResize;
		Signal<SDL_KeyboardEvent> onKeyDown;
		Signal<SDL_KeyboardEvent> onKeyUp;
	};
}

#endif
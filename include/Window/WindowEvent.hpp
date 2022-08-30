#ifndef NTH_WINDOW_WINDOWEVENT_HPP
#define NTH_WINDOW_WINDOWEVENT_HPP

#include <SDL2/SDL_events.h>

namespace Nth {
	enum class WindowEventType {
		Quit,
		Resized,
		KeyDown,
		KeyUp,
		MouseMove,

		Max
	};

	struct WindowEvent {
		WindowEventType type;
		SDL_KeyboardEvent key;
		Sint32 x;
		Sint32 y;
		Sint32 xrel;
		Sint32 yrel;
	};
}

#endif
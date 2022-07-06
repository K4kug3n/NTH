#pragma once

#ifndef NTH_WINDOWEVENT_HPP
#define NTH_WINDOWEVENT_HPP

#include <SDL2/SDL_events.h>

namespace Nth {
	enum class WindowEventType {
		Quit,
		Resized,
		KeyDown,
		KeyUp,

		Max
	};

	struct WindowEvent {
		WindowEventType type;
		SDL_KeyboardEvent key;
	};
}

#endif
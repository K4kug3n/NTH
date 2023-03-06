#ifndef NTH_WINDOW_KEYBOARD_HPP
#define NTH_WINDOW_KEYBOARD_HPP

#include <cstdint>

namespace Nth {
	enum class Keyboard {
		A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
		Space, Return, Backspace, Escape,
		Max
	};

	Keyboard key_from_sdl(uint32_t key);
}

#endif
#include <Window/Keyboard.hpp>

#include <SDL2/SDL_keycode.h>

namespace Nth {
	Keyboard key_from_sdl(uint32_t key) {
		switch (key) {
		case SDLK_a:
			return Keyboard::A;
		case SDLK_b:
			return Keyboard::B;
		case SDLK_c:
			return Keyboard::C;
		case SDLK_d:
			return Keyboard::D;
		case SDLK_e:
			return Keyboard::E;
		case SDLK_f:
			return Keyboard::F;
		case SDLK_g:
			return Keyboard::G;
		case SDLK_h:
			return Keyboard::H;
		case SDLK_i:
			return Keyboard::I;
		case SDLK_j:
			return Keyboard::J;
		case SDLK_k:
			return Keyboard::K;
		case SDLK_l:
			return Keyboard::L;
		case SDLK_m:
			return Keyboard::M;
		case SDLK_n:
			return Keyboard::N;
		case SDLK_o:
			return Keyboard::O;
		case SDLK_p:
			return Keyboard::P;
		case SDLK_q:
			return Keyboard::Q;
		case SDLK_r:
			return Keyboard::R;
		case SDLK_s:
			return Keyboard::S;
		case SDLK_t:
			return Keyboard::T;
		case SDLK_u:
			return Keyboard::U;
		case SDLK_v:
			return Keyboard::V;
		case SDLK_w:
			return Keyboard::W;
		case SDLK_x:
			return Keyboard::X;
		case SDLK_y:
			return Keyboard::Y;
		case SDLK_z:
			return Keyboard::Z;
		case SDLK_ESCAPE:
			return Keyboard::Escape;
		case SDLK_SPACE:
			return Keyboard::Space;
		case SDLK_RETURN:
			return Keyboard::Return;
		case SDLK_BACKSPACE:
			return Keyboard::Backspace;
		default:
			return Keyboard::Max;
		}
	}
}
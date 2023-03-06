#ifndef NTH_WINDOW_INPUTHANDLER_HPP
#define NTH_WINDOW_INPUTHANDLER_HPP

#include <Window/Keyboard.hpp>

#include <functional>
#include <array>

namespace Nth {
	class InputHandler {
	public:
		InputHandler()= default;
		InputHandler(InputHandler&&) = default;
		InputHandler(const InputHandler&) = delete;
		~InputHandler()= default;

		void handle(Keyboard key);
		void set_callback(Keyboard key, std::function<void()> callback);

		InputHandler& operator=(InputHandler&&) = default;
		InputHandler& operator=(const InputHandler&) = delete;
	private:
		std::array<std::function<void()>, static_cast<size_t>(Keyboard::Max)> m_callbacks;
	};
}

#endif
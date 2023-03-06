#include <Window/InputHandler.hpp>


namespace Nth {
	InputHandler::InputHandler() :
		m_callbacks() {
		m_callbacks.fill([]() {});
	}

	void InputHandler::handle(Keyboard key) {
		if (key != Keyboard::Max) {
			m_callbacks[static_cast<size_t>(key)]();
		}
	}

	void InputHandler::set_callback(Keyboard key, std::function<void()> callback) {
		m_callbacks[static_cast<size_t>(key)] = std::move(callback);
	}
}
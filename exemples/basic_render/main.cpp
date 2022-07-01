#include "Window/Window.hpp"

int main() {
	Nth::Window::initialize();

	Nth::Window window{ Nth::VideoMode{ 860, 480 }, "Hello world" };

	Nth::EventHandler& eventHandler{ window.getEventHandler() };
	eventHandler.onQuit.connect([&window]() {
		window.close();
	});

	while (window.isOpen()) {
		window.processEvent();
	}

	return 0;
}
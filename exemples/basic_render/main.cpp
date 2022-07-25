#include "Renderer/Renderer.hpp"
#include "Renderer/RenderWindow.hpp"

#include <iostream>

int main() {
	Nth::Window::initialize();

	Nth::Renderer renderer;

	Nth::RenderWindow& window{ renderer.getWindow(Nth::VideoMode{ 860, 480 }, "Hello world") };

	Nth::EventHandler& eventHandler{ window.getEventHandler() };
	eventHandler.onQuit.connect([&window]() {
		window.close();
	});

	while (window.isOpen()) {
		window.processEvent();

		window.draw();
	}

	return 0;
}
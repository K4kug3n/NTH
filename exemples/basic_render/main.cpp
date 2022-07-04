#include "Renderer/VulkanInstance.hpp"
#include "Renderer/RenderWindow.hpp"

#include <iostream>

int main() {
	Nth::VulkanInstance instance{};

	Nth::Window::initialize();

	Nth::RenderWindow window{ instance, Nth::VideoMode{ 860, 480 }, "Hello world" };

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
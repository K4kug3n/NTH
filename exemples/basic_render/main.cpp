#include "Window/Window.hpp"
#include "Renderer/VulkanInstance.hpp"
#include "Renderer/Surface.hpp"

int main() {
	Nth::Window::initialize();

	Nth::Window window{ Nth::VideoMode{ 860, 480 }, "Hello world" };

	Nth::VulkanInstance instance{};

	Nth::Surface surface{ instance.getInstance() };
	surface.create(window.getHandle());

	std::shared_ptr<Nth::Device> device{ instance.createDevice(surface) };

	Nth::EventHandler& eventHandler{ window.getEventHandler() };
	eventHandler.onQuit.connect([&window]() {
		window.close();
	});

	while (window.isOpen()) {
		window.processEvent();
	}

	return 0;
}
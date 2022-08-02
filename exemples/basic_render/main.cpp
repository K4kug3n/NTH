#include "Renderer/Renderer.hpp"
#include "Renderer/RenderWindow.hpp"
#include "Renderer/RenderObject.hpp"
#include "Renderer/Mesh.hpp"

#include <iostream>

int main() {
	Nth::Window::initialize();

	Nth::Renderer renderer;

	Nth::RenderWindow& window{ renderer.getWindow(Nth::VideoMode{ 860, 480 }, "Hello world") };

	Nth::Material& basicMaterial = renderer.createMaterial("vert.spv", "frag.spv");

	Nth::Mesh vikingRoomMesh = Nth::Mesh::fromOBJ("viking_room.obj");

	// TODO: Review this API
	renderer.createMesh(vikingRoomMesh);

	Nth::RenderObject vikingRoom {
		&vikingRoomMesh,
		&basicMaterial,
		glm::rotate(glm::mat4(1.f), glm::radians(45.f), glm::vec3(0.f, 0.f, 1.f))
	};

	Nth::EventHandler& eventHandler{ window.getEventHandler() };
	eventHandler.onQuit.connect([&window]() {
		window.close();
	});

	while (window.isOpen()) {
		window.processEvent();

		window.draw({ vikingRoom });
	}

	renderer.waitIdle();

	return 0;
}
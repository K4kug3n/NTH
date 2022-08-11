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
	Nth::VulkanTexture& vikingRoomTexture = renderer.createTexture("viking_room.png");

	Nth::Mesh vikingRoomMesh = Nth::Mesh::fromOBJ("viking_room.obj");
	

	// TODO: Review this API
	renderer.createMesh(vikingRoomMesh);

	Nth::RenderObject vikingRoom {
		&vikingRoomMesh,
		&basicMaterial,
		&vikingRoomTexture,
		glm::rotate(glm::mat4(1.f), glm::radians(45.f), glm::vec3(0.f, 0.f, 1.f)) * glm::translate(glm::mat4(1.f), glm::vec3(0.f, 1.f, 0.f))
	};

	Nth::RenderObject vikingRoom2 {
		&vikingRoomMesh,
		&basicMaterial,
		&vikingRoomTexture,
		glm::rotate(glm::mat4(1.f), glm::radians(45.f), glm::vec3(0.f, 0.f, 1.f)) * glm::translate(glm::mat4(1.f), glm::vec3(0.f, -1.f, 0.f))
	};

	Nth::EventHandler& eventHandler{ window.getEventHandler() };
	eventHandler.onQuit.connect([&window]() {
		window.close();
	});

	while (window.isOpen()) {
		window.processEvent();

		renderer.draw({ vikingRoom, vikingRoom2 });
	}

	renderer.waitIdle();

	return 0;
}
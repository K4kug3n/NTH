#include "Renderer/Renderer.hpp"
#include "Renderer/RenderWindow.hpp"
#include "Renderer/RenderObject.hpp"
#include "Renderer/Mesh.hpp"

#include <iostream>
#include <chrono>

int main() {
	Nth::Window::initialize();

	Nth::Renderer renderer;

	Nth::RenderWindow& window{ renderer.getWindow(Nth::VideoMode{ 860, 480 }, "Hello world") };

	Nth::MaterialInfos basicMaterialInfos = {
		"vert.spv",
		"frag.spv"
	};

	Nth::Material basicMaterial = renderer.createMaterial(basicMaterialInfos);
	Nth::VulkanTexture vikingRoomTexture = renderer.createTexture("viking_room.png");

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

	glm::vec3 camPos{ glm::vec3(0.f, 0.f, 0.f) };
	eventHandler.onKeyDown.connect([&camPos](SDL_KeyboardEvent key) {
		float stepSize{ 0.1f };

		switch (key.keysym.sym) {
		case SDLK_z:
			camPos.z = camPos.z + stepSize;
			break;
		case SDLK_s:
			camPos.z = camPos.z - stepSize;
			break;
		case SDLK_q:
			camPos.x = camPos.x + stepSize;
			break;
		case SDLK_d:
			camPos.x = camPos.x - stepSize;
			break;
		case SDLK_SPACE:
			camPos.y = camPos.y + stepSize;
			break;
		case SDLK_LSHIFT:
			camPos.y = camPos.y - stepSize;
			break;
		default:
			break;
		}
	});

	auto start = std::chrono::high_resolution_clock::now();
	while (window.isOpen()) {

		renderer.light = {
			glm::vec4(1.f, 1.f, 1.f, 1.f),
			0.1f,
			camPos,
			glm::vec4(1.f, 1.f, 1.f, 1.f)
		};

		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> diff = end - start;
		start = end;

		window.setTitle(std::to_string(1 / diff.count()) + " FPS");
		window.processEvent();

		renderer.draw({ vikingRoom, vikingRoom2 });
	}

	renderer.waitIdle();

	return 0;
}
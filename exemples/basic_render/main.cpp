#include <Renderer/Renderer.hpp>
#include <Renderer/RenderWindow.hpp>
#include <Renderer/RenderObject.hpp>
#include <Renderer/Mesh.hpp>

#include <Math/Angle.hpp>

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

	Nth::RenderObject vikingRoom{
		&vikingRoomMesh,
		&basicMaterial,
		&vikingRoomTexture,
		Nth::Matrix4f::Rotation(Nth::toRadians(45.f), { 0.f, 0.f, 1.f }) * Nth::Matrix4f::Translation({ 0.f, 1.f, 0.f })
	};

	Nth::RenderObject vikingRoom2 {
		&vikingRoomMesh,
		&basicMaterial,
		&vikingRoomTexture,
		Nth::Matrix4f::Rotation(Nth::toRadians(45.f), { 0.f, 0.f, 1.f })* Nth::Matrix4f::Translation({ 0.f, -1.f, 0.f })
	};

	Nth::EventHandler& eventHandler{ window.getEventHandler() };
	eventHandler.onQuit.connect([&window]() {
		window.close();
	});


	Nth::Vector3f lightPos{ 0.f, 0.f, 0.f };
	eventHandler.onKeyDown.connect([&lightPos](SDL_KeyboardEvent key) {
		float stepSize{ 0.1f };

		switch (key.keysym.sym) {
		case SDLK_z:
			lightPos.z += stepSize;
			break;
		case SDLK_s:
			lightPos.z -= stepSize;
			break;
		case SDLK_q:
			lightPos.x += stepSize;
			break;
		case SDLK_d:
			lightPos.x -= stepSize;
			break;
		case SDLK_SPACE:
			lightPos.y += stepSize;
			break;
		case SDLK_LSHIFT:
			lightPos.y -= stepSize;
			break;
		default:
			break;
		}
	});

	auto start = std::chrono::high_resolution_clock::now();
	while (window.isOpen()) {

		renderer.light = {
			{ 2.f, 2.f, 2.f },
			{ 1.f, 1.f, 1.f, 1.f },
			lightPos,
			0.1f,
			0.5f,
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
#include <Renderer/Renderer.hpp>
#include <Renderer/RenderWindow.hpp>
#include <Renderer/RenderObject.hpp>
#include <Renderer/Mesh.hpp>
#include <Renderer/Model.hpp>

#include <Renderer/Texture.hpp>

#include <Math/Angle.hpp>

#include <iostream>
#include <chrono>

int main() {
	Nth::Window::initialize();
	Nth::Window::setRelativeMouseMode(true);

	Nth::Renderer renderer;

	Nth::RenderWindow& window{ renderer.getWindow(Nth::VideoMode{ 860, 480 }, "Hello world") };

	Nth::MaterialInfos basicMaterialInfos = {
		"vert.spv",
		"frag.spv"
	};

	Nth::Material basicMaterial = renderer.createMaterial(basicMaterialInfos);

	Nth::Model model;
	try {
		model.loadFromFile("./guitare/scene.gltf");
	}
	catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	size_t modelIndex = renderer.registerModel(model);
	
	Nth::RenderObject obj1{
		modelIndex,
		&basicMaterial,
		Nth::Matrix4f::Translation({ 0.f, -1.f, 0.f }) * Nth::Matrix4f::Scale({ 0.01f, 0.01f, 0.01f })
	};

	Nth::EventHandler& eventHandler{ window.getEventHandler() };
	eventHandler.onQuit.connect([&window]() {
		window.close();
	}); 

	renderer.camera.position = Nth::Vector3f{ 0.f, 0.f, -3.f };
	renderer.camera.direction = Nth::EulerAngle(0.f, 0.f, 0.f);

	eventHandler.onKeyDown.connect([&renderer, &window](SDL_KeyboardEvent key) {
		float stepSize{ 0.1f };

		switch (key.keysym.sym) {
		case SDLK_z:
			renderer.camera.position.z += stepSize;
			break;
		case SDLK_s:
			renderer.camera.position.z -= stepSize;
			break;
		case SDLK_q:
			renderer.camera.position.x += stepSize;
			break;
		case SDLK_d:
			renderer.camera.position.x -= stepSize;
			break;
		case SDLK_SPACE:
			renderer.camera.position.y -= stepSize;
			break;
		case SDLK_LSHIFT:
			renderer.camera.position.y += stepSize;
			break;
		case SDLK_ESCAPE:
			window.close();
			break;
		default:
			break;
		}
	});

	eventHandler.onMouseMove.connect([&renderer](Sint32 x, Sint32 y, Sint32 xrel, Sint32 yrel) {
		float sensitivity{ 0.1f };

		renderer.camera.direction.yaw += xrel * sensitivity;
		renderer.camera.direction.pitch += yrel * sensitivity;
	});

	renderer.light = {
		renderer.camera.position,
		{ 1.f, 1.f, 1.f, 1.f },
		{ 0.f, 0.f, 0.f },
		0.3f,
		0.5f,
	};

	auto start = std::chrono::high_resolution_clock::now();
	while (window.isOpen()) {
		renderer.light.viewPos = renderer.camera.position;

		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> diff = end - start;
		start = end;

		window.setTitle(std::to_string(1 / diff.count()) + " FPS");
		window.processEvent();

		renderer.draw({ obj1 });
	}

	renderer.waitIdle();

	return 0;
}
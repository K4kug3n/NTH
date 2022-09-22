#include <Renderer/Renderer.hpp>
#include <Renderer/RenderWindow.hpp>
#include <Renderer/RenderObject.hpp>
#include <Renderer/Mesh.hpp>
#include <Renderer/Model.hpp>
#include <Renderer/Texture.hpp>

#include <Math/Angle.hpp>

#include <Util/Color.hpp>

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

	Nth::Model model{ "./guitare/scene.gltf" };

	Nth::Texture uniformTexture{ Nth::uniformTexture(Nth::Color{ 255, 0, 0 }) };
	
	Nth::Model planeModel;
	size_t textureIndex = planeModel.addTexture(std::move(uniformTexture));
	Nth::Mesh plane{ Nth::Mesh::Plane() };
	plane.addTextureIndex(textureIndex);
	planeModel.addMesh(std::move(plane));

	size_t planeIndex = renderer.registerModel(planeModel);

	size_t modelIndex = renderer.registerModel(model);
	
	Nth::RenderObject obj1{
		modelIndex,
		&basicMaterial,
		Nth::Matrix4f::Translation({ 0.f, -1.f, 0.f }) * Nth::Matrix4f::Scale({ 0.01f, 0.01f, 0.01f })
	};

	Nth::RenderObject obj2{
		planeIndex,
		&basicMaterial,
		Nth::Matrix4f::Rotation(Nth::toRadians(90.f), {1.f, 0.f, 0.f}) * Nth::Matrix4f::Translation({ -1.f, 0.f, 0.f }) * Nth::Matrix4f::Scale({ 2.f, 2.f, 2.f })
	};

	Nth::EventHandler& eventHandler{ window.getEventHandler() };
	eventHandler.onQuit.connect([&window]() {
		window.close();
	}); 

	renderer.camera.position = Nth::Vector3f{ 0.f, -1.f, -3.f };
	renderer.camera.direction = Nth::EulerAngle(0.f, 0.f, 0.f);

	renderer.light = {
		renderer.camera.position,
		{ 1.f, 1.f, 1.f, 1.f },
		{ 0.f, -0.2f, 1.f },
		0.3f,
		0.5f,
	};

	eventHandler.onKeyDown.connect([&renderer, &window](SDL_KeyboardEvent key) {
		float stepSize{ 0.1f };

		switch (key.keysym.sym) {
		case SDLK_z:
			renderer.light.lightPos.z += stepSize;
			break;
		case SDLK_s:
			renderer.light.lightPos.z -= stepSize;
			break;
		case SDLK_q:
			renderer.light.lightPos.x += stepSize;
			break;
		case SDLK_d:
			renderer.light.lightPos.x -= stepSize;
			break;
		case SDLK_SPACE:
			renderer.light.lightPos.y -= stepSize;
			break;
		case SDLK_LSHIFT:
			renderer.light.lightPos.y += stepSize;
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

	unsigned nbFrames = 0;

	auto start = std::chrono::high_resolution_clock::now();
	while (window.isOpen()) {
		renderer.light.viewPos = renderer.camera.position;

		auto end = std::chrono::high_resolution_clock::now();
		nbFrames += 1;
		std::chrono::duration<float> diff = end - start;
		if (diff.count() >= 1.f) {
			window.setTitle(std::to_string(1000.f / (float)(nbFrames)) + " ms/frame");
			nbFrames = 0;
			start += std::chrono::seconds(1);
		}

		window.processEvent();

		renderer.draw({ obj2 });
	}

	renderer.waitIdle();

	return 0;
}
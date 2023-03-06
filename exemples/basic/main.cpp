#include <Renderer/Renderer.hpp>
#include <Renderer/RenderObject.hpp>
#include <Renderer/Mesh.hpp>
#include <Renderer/Model.hpp>
#include <Renderer/Texture.hpp>

#include <Window/Window.hpp>

#include <Maths/Angle.hpp>

#include <Utils/Color.hpp>

#include <iostream>
#include <chrono>

int main() {
	Nth::Window::Init();
	Nth::Window window{ "Hello World", 100, 100, 640, 480, 0 };
	window.set_resizable();

	Nth::Renderer renderer;
	renderer.set_render_on(window);

	Nth::MaterialInfos basic_material_infos = {
		"vert.spv",
		"frag.spv"
	};

	Nth::Material basic_material = renderer.create_material(basic_material_infos);

	//Nth::Texture uniform_texture{ Nth::uniform_texture(Nth::Color{ 255, 0, 0 }) };
	//
	//Nth::Model plane_model;
	//size_t texture_index = plane_model.add_texture(std::move(uniform_texture));
	//Nth::Mesh plane{ Nth::Mesh::Plane() };
	//plane.add_texture_index(texture_index);
	//plane_model.add_mesh(std::move(plane));

	//size_t plane_index = renderer.register_model(plane_model);

	//Nth::RenderObject obj{
	//	plane_index,
	//	&basic_material,
	//	Nth::Matrix4f::Rotation(Nth::to_radians(90.f), {1.f, 0.f, 0.f}) * Nth::Matrix4f::Translation({ -1.f, 0.f, 0.f }) * Nth::Matrix4f::Scale({ 2.f, 2.f, 2.f })
	//}; 

	Nth::Model model{ "./boxs/scene.gltf" };
	size_t model_index = renderer.register_model(model);

	Nth::RenderObject obj{
		model_index,
		&basic_material,
		Nth::Matrix4f::Identity()
	};

	renderer.camera.position = Nth::Vector3f{ 0.f, 0.f, 0.f }; 

	Nth::InputHandler& input_handler = window.input_handler();
	input_handler.set_callback(Nth::Keyboard::Z, [&renderer]() { renderer.camera.position.z += 0.1f; });
	input_handler.set_callback(Nth::Keyboard::Q, [&renderer]() { renderer.camera.position.x += 0.1f; });
	input_handler.set_callback(Nth::Keyboard::S, [&renderer]() { renderer.camera.position.z -= 0.1f; });
	input_handler.set_callback(Nth::Keyboard::D, [&renderer]() { renderer.camera.position.x -= 0.1f; });
	input_handler.set_callback(Nth::Keyboard::Space, [&renderer]() { renderer.camera.position.y -= 0.1f; });
	input_handler.set_callback(Nth::Keyboard::C, [&renderer]() { renderer.camera.position.y += 0.1f; });

	renderer.light = {
		renderer.camera.position,
		{ 1.f, 1.f, 1.f, 1.f },
		{ 0.f, -0.2f, 1.f },
		0.3f,
		0.5f,
	};

	while (window.is_open()) {
		window.poll_event();

		renderer.draw({ obj });
	}

	renderer.wait_idle();

	return 0;
}
#ifndef NTH_RENDERER_RENDERER_HPP
#define NTH_RENDERER_RENDERER_HPP

#include <Renderer/RenderInstance.hpp>
#include <Renderer/Vulkan/DescriptorSetLayout.hpp>
#include <Renderer/RenderSurface.hpp>
#include <Renderer/Material.hpp>
#include <Renderer/DescriptorAllocator.hpp>
#include <Renderer/Mesh.hpp>
#include <Renderer/RenderTexture.hpp>
#include <Renderer/Camera.hpp>
#include <Renderer/Vulkan/DescriptorSet.hpp>
#include <Renderer/RenderBuffer.hpp>
#include <Renderer/RenderModel.hpp>
#include <Renderer/ShaderBinding.hpp>

#include <vector>
#include <array>
#include <string_view>

namespace Nth {
	class RenderObject;
	class Model;
	struct Texture;
	struct BindingInfo;
	class Window;

	class Renderer {
	public:
		Renderer();
		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) = default;
		~Renderer() = default;

		void set_render_on(Window& window);

		// TODO: Review this API
		Material create_material(const MaterialInfos& infos);

		size_t register_model(const Model& model);

		void draw(const std::vector<RenderObject>& objects);

		// TODO: Move out, used for sync destructor
		void wait_idle() const;

		// TODO: Move out
		LightGpuObject light;
		Camera camera;

		static constexpr uint32_t resource_count = 3;

		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) = default;

	private:
		ViewerGpuObject get_viewer_data() const;
		void update_descriptor_set();

		RenderInstance m_vulkan;
		RenderSurface m_render_surface;

		DescriptorAllocator m_descriptor_allocator;

		// TODO: May move it in dedicated class
		size_t add_descriptor_set_layout(const std::vector<BindingInfo>& bindings);
		std::vector<Vk::DescriptorSetLayout> m_descriptor_set_layouts;
		ShaderBinding allocate_shader_binding(size_t index);

		size_t m_resource_index;

		std::array<ShaderBinding, Renderer::resource_count> m_model_bindings;
		std::array<RenderBuffer, Renderer::resource_count> m_model_buffers;

		std::array<ShaderBinding, Renderer::resource_count> m_viewer_bindings;
		std::array<RenderBuffer, Renderer::resource_count> m_viewer_buffers;

		std::array<ShaderBinding, Renderer::resource_count> m_light_bindings;
		std::array<RenderBuffer, Renderer::resource_count> m_light_buffers;

		// TODO: Review this
		RenderMesh register_mesh(const Mesh& mesh) const;
		RenderTexture register_texture(const Texture& texture);
		Window* m_window;

		std::vector<RenderModel> m_renders;
	};
}

#endif
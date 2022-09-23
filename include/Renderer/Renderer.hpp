#ifndef NTH_RENDERER_RENDERER_HPP
#define NTH_RENDERER_RENDERER_HPP

#include <Renderer/RenderInstance.hpp>
#include <Renderer/Vulkan/DescriptorSetLayout.hpp>
#include <Renderer/RenderWindow.hpp>
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

	class Renderer {
	public:
		Renderer();
		Renderer(Renderer const&) = delete;
		Renderer(Renderer&&) = default;
		~Renderer() = default;

		RenderWindow& getWindow(VideoMode const& mode, const std::string_view title);

		// TODO: Review this API
		Material createMaterial(MaterialInfos const& infos);

		size_t registerModel(Model const& model);

		void draw(std::vector<RenderObject> const& objects);

		// TODO: Move out, used for sync destructor
		void waitIdle() const;

		// TODO: Move out
		LightGpuObject light;
		Camera camera;

		static constexpr uint32_t resourceCount = 3;

		Renderer& operator=(Renderer const&) = delete;
		Renderer& operator=(Renderer&&) = default;

	private:
		ViewerGpuObject getViewerData() const;
		bool updateDescriptorSet();

		RenderInstance m_vulkan;
		RenderWindow m_renderWindow;

		DescriptorAllocator m_descriptorAllocator;

		// TODO: May move it in dedicated class
		size_t addDescriptorSetLayout(std::vector<BindingInfo> const& bindings);
		std::vector<Vk::DescriptorSetLayout> m_descriptorSetLayouts;
		ShaderBinding allocateShaderBinding(size_t index);

		size_t m_resourceIndex;

		std::array<ShaderBinding, Renderer::resourceCount> m_modelBindings;
		std::array<RenderBuffer, Renderer::resourceCount> m_modelBuffers;

		std::array<ShaderBinding, Renderer::resourceCount> m_viewerBindings;
		std::array<RenderBuffer, Renderer::resourceCount> m_viewerBuffers;

		std::array<ShaderBinding, Renderer::resourceCount> m_lightBindings;
		std::array<RenderBuffer, Renderer::resourceCount> m_lightBuffers;

		// TODO: Review this
		RenderMesh registerMesh(Mesh const& mesh) const;
		RenderTexture registerTexture(Texture const& texture);

		std::vector<RenderModel> m_Renders;
	};
}

#endif
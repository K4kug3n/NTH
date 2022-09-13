#ifndef NTH_RENDERER_RENDERER_HPP
#define NTH_RENDERER_RENDERER_HPP

#include <Renderer/VulkanInstance.hpp>
#include <Renderer/Vulkan/DescriptorSetLayout.hpp>
#include <Renderer/RenderWindow.hpp>
#include <Renderer/Material.hpp>
#include <Renderer/DescriptorAllocator.hpp>
#include <Renderer/Mesh.hpp>
#include <Renderer/VulkanTexture.hpp>
#include <Renderer/Camera.hpp>
#include <Renderer/Vulkan/DescriptorSet.hpp>
#include <Renderer/VulkanBuffer.hpp>
#include <Renderer/RenderableModel.hpp>

#include <vector>
#include <array>
#include <string_view>

namespace Nth {
	class RenderObject;
	class Model;
	struct Texture;
	struct ShaderBinding;

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

		VulkanInstance m_vulkan;
		RenderWindow m_renderWindow;

		DescriptorAllocator m_descriptorAllocator;

		// TODO: May move it in dedicated class
		size_t addDescriptorSetLayout(std::vector<ShaderBinding> const& bindings);
		std::vector<Vk::DescriptorSetLayout> m_descriptorSetLayouts;

		size_t m_resourceIndex;

		std::array<Vk::DescriptorSet, Renderer::resourceCount> m_modelDescriptors;
		std::array<VulkanBuffer, Renderer::resourceCount> m_modelBuffers;

		std::array<Vk::DescriptorSet, Renderer::resourceCount> m_viewerDescriptors;
		std::array<VulkanBuffer, Renderer::resourceCount> m_viewerBuffers;

		std::array<Vk::DescriptorSet, Renderer::resourceCount> m_lightDescriptors;
		std::array<VulkanBuffer, Renderer::resourceCount> m_lightBuffers;

		// TODO: Review this
		RenderableMesh registerMesh(Mesh const& mesh) const;
		VulkanTexture registerTexture(Texture const& texture);

		std::vector<RenderableModel> m_renderables;
	};
}

#endif
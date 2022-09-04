#ifndef NTH_RENDERER_RENDERER_HPP
#define NTH_RENDERER_RENDERER_HPP

#include <Renderer/VulkanInstance.hpp>
#include <Renderer/Vulkan/DescriptorSetLayout.hpp>
#include <Renderer/RenderWindow.hpp>
#include <Renderer/Material.hpp>
#include <Renderer/DescriptorAllocator.hpp>
#include <Renderer/Mesh.hpp>
#include <Renderer/VulkanTexture.hpp>
#include <Renderer/RenderingResource.hpp>
#include <Renderer/Camera.hpp>

#include <vector>
#include <string_view>

namespace Nth {
	class RenderObject;

	class Renderer {
	public:
		Renderer();
		Renderer(Renderer const&) = delete;
		Renderer(Renderer&&) = default;
		~Renderer() = default;

		RenderWindow& getWindow(VideoMode const& mode, const std::string_view title);
		VulkanTexture createTexture(const std::string_view name);
		Material createMaterial(MaterialInfos const& infos);

		void createMesh(Mesh& mesh);

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
		bool createRenderingResources();

		VulkanInstance m_vulkan;
		RenderWindow m_renderWindow;

		DescriptorAllocator m_descriptorAllocator;

		// TODO: May move it in dedicated class
		size_t addDescriptorSetLayout(std::vector<VkDescriptorSetLayoutBinding> const& layoutBindings);
		std::vector<Vk::DescriptorSetLayout> m_descriptorSetLayouts;

		std::vector<RenderingResource> m_renderingResources;
		size_t m_resourceIndex;
	};
}

#endif
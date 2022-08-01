#ifndef NTH_RENDERER_RENDERWINDOW_HPP
#define NTH_RENDERER_RENDERWINDOW_HPP

#include "Window/Window.hpp"

#include "Renderer/Vulkan/VulkanInstance.hpp"
#include "Renderer/Vulkan/Surface.hpp"
#include "Renderer/Vulkan/Swapchain.hpp"
#include "Renderer/Vulkan/Queue.hpp"
#include "Renderer/Vulkan/RenderPass.hpp"
#include "Renderer/RenderingResource.hpp"
#include "Renderer/Vulkan/DescriptorSet.hpp"
#include "Renderer/Mesh.hpp"
#include "Renderer/Material.hpp"
#include "Renderer/VulkanBuffer.hpp"
#include "Renderer/VulkanTexture.hpp"
#include "Renderer/DepthImage.hpp"

#include "Math/Vector2.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <array>

namespace Nth {
	class RenderObject;

	// TODO: Move out
	// TODO: Precalculate VP CPU-side
	struct UniformBufferObject {
		glm::mat4 view;
		glm::mat4 proj;
	};

	// TODO: Move out
	struct ShaderStorageBufferObject {
		glm::mat4 model;
	};

	class RenderWindow : public Window {
	public:
		RenderWindow(Vk::VulkanInstance& vulkanInstance);
		RenderWindow(Vk::VulkanInstance& vulkanInstance, VideoMode const& mode, const std::string_view title);
		RenderWindow(RenderWindow const&) = delete;
		RenderWindow(RenderWindow&&) = default;
		~RenderWindow();

		bool create(VideoMode const& mode, const std::string_view title);
		bool draw(std::vector<RenderObject> const& objects);

		// TODO: Temp while moving features
		bool createMesh(Mesh& mesh);
		std::vector<RenderingResource>& getRenderingRessources();
		Vk::DescriptorSet& getDescriptor();
		Vk::RenderPass& getRenderPass();
		bool updateDescriptorSet();

		static constexpr uint32_t resourceCount = 3;

		RenderWindow& operator=(RenderWindow const&) = delete;
		RenderWindow& operator=(RenderWindow&&) = default;

	private:
		bool createSwapchain();
		bool createRenderPass();
		bool createStagingBuffer();
		bool createRenderingResources();
		bool createTexture();
		bool copyTextureData(char const* textureData, uint32_t dataSize, uint32_t width, uint32_t height);
		bool createUniformBuffer();
		bool createSSBO();
		bool createDepthRessource();
		void onWindowSizeChanged();

		VkSurfaceFormatKHR getSwapchainFormat(std::vector<VkSurfaceFormatKHR> const& surfaceFormats) const;
		uint32_t getSwapchainNumImages(VkSurfaceCapabilitiesKHR const& capabilities) const;
		VkExtent2D getSwapchainExtent(VkSurfaceCapabilitiesKHR const& capabilities, Vector2ui const& size) const;
		VkImageUsageFlags getSwapchainUsageFlags(VkSurfaceCapabilitiesKHR const& capabilities) const;
		VkSurfaceTransformFlagBitsKHR getSwapchainTransform(VkSurfaceCapabilitiesKHR const& capabilities) const;
		VkPresentModeKHR getSwapchainPresentMode(std::vector<VkPresentModeKHR> const& presentModes) const;
		bool prepareFrame(RenderingResource& ressources, Vk::SwapchainImage const& imageParameters, std::vector<RenderObject> const& objects) const;
		bool createFramebuffer(Vk::Framebuffer& framebuffer, Vk::SwapchainImage const& swapchainImage) const;
		bool copyUniformBufferData();
		bool copyBufferByStaging(VulkanBuffer& target, VulkanBuffer& staging, std::function<void(void*)> copyFunction);
		UniformBufferObject getUniformBufferData() const;

		Vk::VulkanInstance& m_vulkan;
		Vk::Surface m_surface;
		Vk::Swapchain m_swapchain;
		Vk::Queue m_presentQueue;
		Vk::Queue m_graphicsQueue;
		Vk::RenderPass m_renderPass;
		VulkanBuffer m_stagingBuffer;
		VulkanBuffer m_uniformBuffer;
		VulkanTexture m_image;
		DepthImage m_depth;
		Vk::DescriptorSet m_descriptor;
		std::vector<RenderingResource> m_renderingResources;

		Vector2ui m_swapchainSize;
		size_t m_resourceIndex;
	};
}

#endif

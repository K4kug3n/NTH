#ifndef NTH_RENDERER_RENDERWINDOW_HPP
#define NTH_RENDERER_RENDERWINDOW_HPP

#include "Window/Window.hpp"

#include "Renderer/VulkanInstance.hpp"
#include "Renderer/Surface.hpp"
#include "Renderer/Swapchain.hpp"
#include "Renderer/Queue.hpp"
#include "Renderer/Semaphore.hpp"
#include "Renderer/CommandPool.hpp"
#include "Renderer/CommandBuffer.hpp"
#include "Renderer/RenderPass.hpp"
#include "Renderer/Framebuffer.hpp"
#include "Renderer/ShaderModule.hpp"
#include "Renderer/PipelineLayout.hpp"
#include "Renderer/Pipeline.hpp"
#include "Renderer/Buffer.hpp"
#include "Renderer/DeviceMemory.hpp"
#include "Renderer/RenderingResource.hpp"
#include "Renderer/Image.hpp"
#include "Renderer/Sampler.hpp"
#include "Renderer/DescriptorPool.hpp"
#include "Renderer/DescriptorSet.hpp"
#include "Renderer/DescriptorSetLayout.hpp"

#include "Math/Vector2.hpp"

#include <vector>

namespace Nth {
	// TODO: Move out
	struct VertexData {
		float   x, y, z, w;
		float   u, v;
	};

	// TODO: Move out
	struct BufferParameters {
		Vk::Buffer buffer;
		Vk::DeviceMemory memory;
	};

	// TODO: Move out
	struct ImageParameters {
		Vk::Image image;
		Vk::ImageView view;
		Vk::DeviceMemory memory;
		Vk::Sampler sampler;
	};

	// TODO: Move out
	struct DescriptorSetParameters {
		Vk::DescriptorPool pool;
		Vk::DescriptorSet descriptor;
		Vk::DescriptorSetLayout layout;
	};

	class RenderWindow : public Window {
	public:
		RenderWindow(Vk::VulkanInstance& vulkanInstance);
		RenderWindow(Vk::VulkanInstance& vulkanInstance, VideoMode const& mode, std::string const& title);
		RenderWindow(RenderWindow const&) = delete;
		RenderWindow(RenderWindow&&) = default;
		~RenderWindow();

		bool create(VideoMode const& mode, std::string const& title);
		bool draw();

		static constexpr uint32_t resourceCount = 3;

		RenderWindow& operator=(RenderWindow const&) = delete;
		RenderWindow& operator=(RenderWindow&&) = default;

	private:
		bool createSwapchain();
		bool createRenderPass();
		bool createPipeline();
		bool createVertexBuffer();
		bool createStagingBuffer();
		bool createRenderingResources();
		bool copyVertexData();
		bool createTexture();
		bool copyTextureData(char const* textureData, uint32_t dataSize, uint32_t width, uint32_t height);
		bool createDescriptorSetLayout();
		bool createDescriptorPool();
		bool allocateDescriptorSet();
		bool updateDescriptorSet();
		bool createUniformBuffer();
		void onWindowSizeChanged();

		VkSurfaceFormatKHR getSwapchainFormat(std::vector<VkSurfaceFormatKHR> const& surfaceFormats) const;
		uint32_t getSwapchainNumImages(VkSurfaceCapabilitiesKHR const& capabilities) const;
		VkExtent2D getSwapchainExtent(VkSurfaceCapabilitiesKHR const& capabilities, Vector2ui const& size) const;
		VkImageUsageFlags getSwapchainUsageFlags(VkSurfaceCapabilitiesKHR const& capabilities) const;
		VkSurfaceTransformFlagBitsKHR getSwapchainTransform(VkSurfaceCapabilitiesKHR const& capabilities) const;
		VkPresentModeKHR getSwapchainPresentMode(std::vector<VkPresentModeKHR> const& presentModes) const;
		Vk::ShaderModule createShaderModule(std::string const& filename) const;
		Vk::PipelineLayout createPipelineLayout() const;
		bool allocateBufferMemory(Vk::Buffer const& buffer, VkMemoryPropertyFlagBits memoryProperty, Vk::DeviceMemory& memory) const;
		bool createBuffer(VkBufferUsageFlags usage, VkMemoryPropertyFlagBits memoryProperty, VkDeviceSize size, BufferParameters& bufferParams) const;
		bool prepareFrame(Vk::CommandBuffer& commandbuffer, Vk::SwapchainImage const& imageParameters, Vk::Framebuffer& framebuffer) const;
		bool createFramebuffer(Vk::Framebuffer& framebuffer, Vk::ImageView const& imageView) const;
		std::vector<float> const& getVertexData() const;
		bool createImage(uint32_t width, uint32_t height, Vk::Image& image) const;
		bool allocateImageMemory(Vk::Image const& image, VkMemoryPropertyFlagBits property, Vk::DeviceMemory& memory) const;
		bool createImageView(ImageParameters& imageParameters) const;
		bool createSampler(Vk::Sampler& sampler) const;
		bool copyUniformBufferData();
		std::array<float, 16> getUniformBufferData() const;

		Vk::VulkanInstance& m_vulkanInstance;
		Vk::Surface m_surface;
		Vk::Swapchain m_swapchain;
		Vk::Queue m_presentQueue;
		Vk::Queue m_graphicsQueue;
		Vk::CommandPool m_graphicCommandPool;
		Vk::RenderPass m_renderPass;
		Vk::Pipeline m_graphicPipeline;
		Vk::PipelineLayout m_pipelineLayout;
		BufferParameters m_vertexBuffer;
		BufferParameters m_stagingBuffer;
		BufferParameters m_uniformBuffer;
		ImageParameters m_image;
		DescriptorSetParameters m_descriptor;
		std::vector<Vk::RenderingResource> m_renderingResources;

		Vector2ui m_swapchainSize;
		size_t m_resourceIndex;
	};
}

#endif

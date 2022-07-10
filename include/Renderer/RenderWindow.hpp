#pragma once

#ifndef NTH_RENDERWINDOW_HPP
#define NTH_RENDERWINDOW_HPP

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

#include "Math/Vector2.hpp"

#include <vector>

namespace Nth {

	class Image;

	// TODO: Move out
	struct VertexData {
		float   x, y, z, w;
		float   r, g, b, a;
	};

	// TODO: Move out
	struct BufferParameters {
		Buffer buffer;
		DeviceMemory memory;
	};

	// TODO: Move out
	struct ImageParameters {
		Image image;
		ImageView view;
		DeviceMemory memory;
		//Sampler sampler;
	};

	class RenderWindow : public Window {
	public:
		RenderWindow(VulkanInstance& vulkanInstance);
		RenderWindow(VulkanInstance& vulkanInstance, VideoMode const& mode, std::string const& title);
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
		bool createSemaphores();
		bool createRenderPass();
		bool createPipeline();
		bool createVertexBuffer();
		bool createStagingBuffer();
		bool createRenderingResources();
		bool copyVertexData();
		bool createTexture();
		bool copyTextureData(char* textureData, uint32_t dataSize, uint32_t width, uint32_t height);
		void onWindowSizeChanged();

		VkSurfaceFormatKHR getSwapchainFormat(std::vector<VkSurfaceFormatKHR> const& surfaceFormats) const;
		uint32_t getSwapchainNumImages(VkSurfaceCapabilitiesKHR const& capabilities) const;
		VkExtent2D getSwapchainExtent(VkSurfaceCapabilitiesKHR const& capabilities, Vector2ui const& size) const;
		VkImageUsageFlags getSwapchainUsageFlags(VkSurfaceCapabilitiesKHR const& capabilities) const;
		VkSurfaceTransformFlagBitsKHR getSwapchainTransform(VkSurfaceCapabilitiesKHR const& capabilities) const;
		VkPresentModeKHR getSwapchainPresentMode(std::vector<VkPresentModeKHR> const& presentModes) const;
		ShaderModule createShaderModule(std::string const& filename) const;
		PipelineLayout createPipelineLayout() const;
		bool allocateBufferMemory(Buffer const& buffer, VkMemoryPropertyFlagBits memoryProperty, DeviceMemory& memory) const;
		bool createBuffer(VkBufferUsageFlags usage, VkMemoryPropertyFlagBits memoryProperty, VkDeviceSize size, BufferParameters& bufferParams) const;
		bool prepareFrame(CommandBuffer& commandbuffer, SwapchainImage const& imageParameters, Framebuffer& framebuffer) const;
		bool createFramebuffer(Framebuffer& framebuffer, ImageView const& imageView) const;
		std::vector<float> const& getVertexData() const;
		bool createImage(uint32_t width, uint32_t height, Image& image) const;
		bool allocateImageMemory(Image const& image, VkMemoryPropertyFlagBits property, DeviceMemory& memory) const;
		bool createImageView(ImageParameters& imageParameters) const;

		VulkanInstance& m_vulkanInstance;
		Surface m_surface;
		Swapchain m_swapchain;
		Queue m_presentQueue;
		Queue m_graphicsQueue;
		CommandPool m_graphicCommandPool;
		RenderPass m_renderPass;
		Pipeline m_graphicPipeline;
		BufferParameters m_vertexBuffer;
		BufferParameters m_stagingBuffer;
		ImageParameters m_image;
		std::vector<RenderingResource> m_renderingResources;

		Vector2ui m_swapchainSize;
		size_t m_resourceIndex;
	};
}

#endif

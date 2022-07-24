#ifndef NTH_RENDERER_RENDERWINDOW_HPP
#define NTH_RENDERER_RENDERWINDOW_HPP

#include "Window/Window.hpp"

#include "Renderer/Vulkan/VulkanInstance.hpp"
#include "Renderer/Vulkan/Surface.hpp"
#include "Renderer/Vulkan/Swapchain.hpp"
#include "Renderer/Vulkan/Queue.hpp"
#include "Renderer/Vulkan/Semaphore.hpp"
#include "Renderer/Vulkan/CommandPool.hpp"
#include "Renderer/Vulkan/CommandBuffer.hpp"
#include "Renderer/Vulkan/RenderPass.hpp"
#include "Renderer/Vulkan/Framebuffer.hpp"
#include "Renderer/Vulkan/ShaderModule.hpp"
#include "Renderer/Vulkan/PipelineLayout.hpp"
#include "Renderer/Vulkan/Buffer.hpp"
#include "Renderer/Vulkan/DeviceMemory.hpp"
#include "Renderer/RenderingResource.hpp"
#include "Renderer/Vulkan/Image.hpp"
#include "Renderer/Vulkan/Sampler.hpp"
#include "Renderer/Vulkan/DescriptorPool.hpp"
#include "Renderer/Vulkan/DescriptorSet.hpp"
#include "Renderer/Vulkan/DescriptorSetLayout.hpp"
#include "Renderer/Vertex.hpp"
#include "Renderer/Mesh.hpp"
#include "Renderer/Material.hpp"

#include "Math/Vector2.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <array>

namespace Nth {
	// TODO: Move out
	struct BufferParameters {
		Vk::Buffer buffer;
		Vk::DeviceMemory memory;
	};

	// TODO: Move out
	struct TextureParameters {
		Vk::Image image;
		Vk::ImageView view;
		Vk::DeviceMemory memory;
		Vk::Sampler sampler;
	};

	// TODO: Move out
	struct ImageParameters {
		Vk::Image image;
		Vk::ImageView view;
		Vk::DeviceMemory memory;
	};

	// TODO: Move out
	struct DescriptorSetParameters {
		Vk::DescriptorSet descriptor;
		Vk::DescriptorSetLayout layout;
	};

	// TODO: Move out
	// TODO: Precalculate VP CPU-side
	struct UniformBufferObject {
		glm::mat4 model;
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
		bool loadModel();
		bool createVertexBuffer();
		bool createIndicesBuffer();
		bool createStagingBuffer();
		bool createRenderingResources();
		bool copyVertexData();
		bool copyIndicesData();
		bool createTexture();
		bool copyTextureData(char const* textureData, uint32_t dataSize, uint32_t width, uint32_t height);
		bool createDescriptorSetLayout();
		bool createDescriptorPool();
		bool allocateDescriptorSet();
		bool updateDescriptorSet();
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
		Vk::ShaderModule createShaderModule(std::string const& filename) const;
		Vk::PipelineLayout createPipelineLayout() const;
		bool allocateBufferMemory(Vk::Buffer const& buffer, VkMemoryPropertyFlagBits memoryProperty, Vk::DeviceMemory& memory) const;
		bool createBuffer(VkBufferUsageFlags usage, VkMemoryPropertyFlagBits memoryProperty, VkDeviceSize size, BufferParameters& bufferParams) const;
		bool prepareFrame(Vk::CommandBuffer& commandbuffer, Vk::SwapchainImage const& imageParameters, Vk::Framebuffer& framebuffer) const;
		bool createFramebuffer(Vk::Framebuffer& framebuffer, Vk::SwapchainImage const& swapchainImage) const;
		uint32_t findMemoryType(uint32_t memoryTypeBit, VkMemoryPropertyFlags properties) const;
		bool createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, Vk::Image& image, Vk::DeviceMemory& memory) const;
		bool createImageView(Vk::ImageView& view, Vk::Image const& image, VkFormat format, VkImageAspectFlags aspectFlags) const;
		bool createSampler(Vk::Sampler& sampler) const;
		bool copyUniformBufferData();
		bool copySSBOData();
		UniformBufferObject getUniformBufferData() const;

		// Depth
		VkFormat findSupportedFormat(std::vector<VkFormat> const& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;
		VkFormat findDepthFormat() const;
		bool hasStencilComponent(VkFormat format) const;

		Vk::VulkanInstance& m_vulkanInstance;
		Vk::Surface m_surface;
		Vk::Swapchain m_swapchain;
		Vk::Queue m_presentQueue;
		Vk::Queue m_graphicsQueue;
		Vk::CommandPool m_graphicCommandPool;
		Vk::RenderPass m_renderPass;
		Material m_material;
		Mesh m_mesh;
		BufferParameters m_vertexBuffer;
		BufferParameters m_indexBuffer;
		BufferParameters m_stagingBuffer;
		BufferParameters m_uniformBuffer;
		BufferParameters m_ssbo;
		TextureParameters m_image;
		ImageParameters m_depth;
		Vk::DescriptorPool m_descriptorPool;
		// TODO: No more memory, need another one (make it automatic ?)
		Vk::DescriptorPool m_descriptorPool2;
		DescriptorSetParameters m_descriptor;
		DescriptorSetParameters m_ssboDescriptor;
		std::vector<RenderingResource> m_renderingResources;

		Vector2ui m_swapchainSize;
		size_t m_resourceIndex;
	};
}

#endif

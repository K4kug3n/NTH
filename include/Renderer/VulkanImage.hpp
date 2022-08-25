#ifndef NTH_RENDERER_VULKANIMAGE_HPP
#define NTH_RENDERER_VULKANIMAGE_HPP

#include <Renderer/Vulkan/Image.hpp>
#include <Renderer/Vulkan/ImageView.hpp>
#include <Renderer/Vulkan/DeviceMemory.hpp>
#include <Renderer/Vulkan/Buffer.hpp>

namespace Nth {
	namespace Vk {
		class CommandBuffer;
		class Device;
	}

	class VulkanDevice;

	class VulkanImage {
	public:
		VulkanImage() = default;
		VulkanImage(VulkanImage const&) = delete;
		VulkanImage(VulkanImage&&) = default;
		~VulkanImage() = default;

		void create(VulkanDevice const& device, uint32_t width, uint32_t height, size_t stagingSize, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
		void createView(VkFormat format, VkImageAspectFlags aspectFlags);

		void copyByStaging(void const* data, size_t size, uint32_t width, uint32_t height, Vk::CommandBuffer& commandBuffer);

		VulkanImage& operator=(VulkanImage const&) = delete;
		VulkanImage& operator=(VulkanImage&&) = default;

		Vk::Image handle;
		Vk::ImageView view;
		Vk::DeviceMemory memory;
	private:
		uint32_t findMemoryType(Vk::Device const& device, uint32_t memoryTypeBit, VkMemoryPropertyFlags properties) const;
		void createStaging(Vk::Device const& device, size_t size);

		VulkanDevice const* m_device;

		Vk::Buffer m_staging;
		Vk::DeviceMemory m_stagingMemory;
	};
}

#endif
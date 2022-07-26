#ifndef NTH_RENDERER_VULKANIMAGE_HPP
#define NTH_RENDERER_VULKANIMAGE_HPP

#include "Renderer/Vulkan/Image.hpp"
#include "Renderer/Vulkan/ImageView.hpp"
#include "Renderer/Vulkan/DeviceMemory.hpp"

namespace Nth {
	namespace Vk {
		class Device;
	}

	class VulkanImage {
	public:
		VulkanImage() = default;
		VulkanImage(VulkanImage const&) = delete;
		VulkanImage(VulkanImage&&) = default;
		~VulkanImage() = default;

		bool create(Vk::Device const& device, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
		bool createView(Vk::Device const& device, VkFormat format, VkImageAspectFlags aspectFlags);

		VulkanImage& operator=(VulkanImage const&) = delete;
		VulkanImage& operator=(VulkanImage&&) = default;

		Vk::Image handle;
		Vk::ImageView view;
		Vk::DeviceMemory memory;
	private:
		uint32_t findMemoryType(Vk::Device const& device, uint32_t memoryTypeBit, VkMemoryPropertyFlags properties) const;
	};
}

#endif
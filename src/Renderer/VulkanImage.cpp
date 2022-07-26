#include "Renderer/VulkanImage.hpp"

#include "Renderer/Vulkan/Device.hpp"
#include "Renderer/Vulkan/PhysicalDevice.hpp"

#include <iostream>

namespace Nth {
	bool VulkanImage::create(Vk::Device const& device, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties) {
		VkImageCreateInfo imageCreateInfo = {
			VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,  // VkStructureType        sType;
			nullptr,                              // const void            *pNext
			0,                                    // VkImageCreateFlags     flags
			VK_IMAGE_TYPE_2D,                     // VkImageType            imageType
			format,                               // VkFormat               format
			{                                     // VkExtent3D             extent
				width,                                // uint32_t               width
				height,                               // uint32_t               height
				1                                     // uint32_t               depth
			},
			1,                                    // uint32_t               mipLevels
			1,                                    // uint32_t               arrayLayers
			VK_SAMPLE_COUNT_1_BIT,                // VkSampleCountFlagBits  samples
			tiling,                               // VkImageTiling          tiling
			usage,                                // VkImageUsageFlags      usage
			VK_SHARING_MODE_EXCLUSIVE,            // VkSharingMode          sharingMode
			0,                                    // uint32_t               queueFamilyIndexCount
			nullptr,                              // const uint32_t        *pQueueFamilyIndices
			VK_IMAGE_LAYOUT_UNDEFINED             // VkImageLayout          initialLayout
		};

		if (!handle.create(device, imageCreateInfo)) {
			return false;
		}

		VkMemoryRequirements memRequirements = handle.getImageMemoryRequirements();

		VkMemoryAllocateInfo memoryAllocateInfo = {
			VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,                            // VkStructureType                        sType
			nullptr,                                                           // const void                            *pNext
			memRequirements.size,                                              // VkDeviceSize                           allocationSize
			findMemoryType(device, memRequirements.memoryTypeBits, properties) // uint32_t                               memoryTypeIndex
		};

		if (!memory.create(device, memoryAllocateInfo)) {
			return false;
		}

		handle.bindImageMemory(memory);

		return true;
	}

	bool VulkanImage::createView(Vk::Device const& device, VkFormat format, VkImageAspectFlags aspectFlags) {
		VkImageViewCreateInfo imageViewCreateInfo = {
			VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO, // VkStructureType          sType
			nullptr,                                  // const void              *pNext
			0,                                        // VkImageViewCreateFlags   flags
			handle(),                                 // VkImage                  image
			VK_IMAGE_VIEW_TYPE_2D,                    // VkImageViewType          viewType
			format,                                   // VkFormat                 format
			{                                         // VkComponentMapping       components
				VK_COMPONENT_SWIZZLE_IDENTITY,            // VkComponentSwizzle       r
				VK_COMPONENT_SWIZZLE_IDENTITY,            // VkComponentSwizzle       g
				VK_COMPONENT_SWIZZLE_IDENTITY,            // VkComponentSwizzle       b
				VK_COMPONENT_SWIZZLE_IDENTITY             // VkComponentSwizzle       a
			},
			{                                         // VkImageSubresourceRange  subresourceRange
				aspectFlags,                              // VkImageAspectFlags       aspectMask
				0,                                        // uint32_t                 baseMipLevel
				1,                                        // uint32_t                 levelCount
				0,                                        // uint32_t                 baseArrayLayer
				1                                         // uint32_t                 layerCount
			}
		};

		return view.create(device, imageViewCreateInfo);
	}

	uint32_t VulkanImage::findMemoryType(Vk::Device const& device, uint32_t memoryTypeBit, VkMemoryPropertyFlags properties) const {
		VkPhysicalDeviceMemoryProperties memProperties = device.getPhysicalDevice().getMemoryProperties();

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((memoryTypeBit & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}

		throw std::runtime_error("Canno't find suitable memory type!");
	}
}

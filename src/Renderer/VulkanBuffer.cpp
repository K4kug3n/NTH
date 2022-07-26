#include "Renderer/VulkanBuffer.hpp"

#include "Renderer/Vulkan/Device.hpp"
#include "Renderer/Vulkan/PhysicalDevice.hpp"

#include <iostream>

namespace Nth {
	bool VulkanBuffer::create(Vk::Device const& device, VkBufferUsageFlags usage, VkMemoryPropertyFlagBits memoryProperty, VkDeviceSize size) {
		VkBufferCreateInfo bufferCreateInfo = {
			VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,             // VkStructureType                sType
			nullptr,                                          // const void                    *pNext
			0,                                                // VkBufferCreateFlags            flags
			size,                                             // VkDeviceSize                   size
			usage,                                            // VkBufferUsageFlags             usage
			VK_SHARING_MODE_EXCLUSIVE,                        // VkSharingMode                  sharingMode
			0,                                                // uint32_t                       queueFamilyIndexCount
			nullptr                                           // const uint32_t                *pQueueFamilyIndices
		};

		if (!handle.create(device, bufferCreateInfo)) {
			std::cerr << "Could not create buffer!" << std::endl;
			return false;
		}

		if (!allocateBufferMemory(device, memoryProperty)) {
			std::cerr << "Could not allocate memory for a buffer!" << std::endl;
			return false;
		}

		if (!handle.bindBufferMemory(memory)) {
			std::cerr << "Could not bind memory to a buffer!" << std::endl;
			return false;
		}

		return true;
	}

	bool VulkanBuffer::allocateBufferMemory(Vk::Device const& device, VkMemoryPropertyFlagBits memoryProperty) {
		VkMemoryRequirements bufferMemoryRequirements = handle.getMemoryRequirements();;
		VkPhysicalDeviceMemoryProperties memoryProperties = device.getPhysicalDevice().getMemoryProperties();

		for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i) {
			if ((bufferMemoryRequirements.memoryTypeBits & (1 << i)) &&
				(memoryProperties.memoryTypes[i].propertyFlags & memoryProperty) == memoryProperty) {

				VkMemoryAllocateInfo memoryAllocateInfo = {
					VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,     // VkStructureType                        sType
					nullptr,                                    // const void                            *pNext
					bufferMemoryRequirements.size,              // VkDeviceSize                           allocationSize
					i                                           // uint32_t                               memoryTypeIndex
				};

				if (memory.create(device, memoryAllocateInfo)) {
					return true;
				}
			}
		}

		return false;
	}
}

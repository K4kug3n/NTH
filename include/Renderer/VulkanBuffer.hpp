#ifndef NTH_RENDERER_VULKANBUFFER_HPP
#define NTH_RENDERER_VULKANBUFFER_HPP

#include "Renderer/Vulkan/Buffer.hpp"
#include "Renderer/Vulkan/DeviceMemory.hpp"

namespace Nth {
	namespace Vk {
		class Device;
	}

	class VulkanBuffer {
	public:
		VulkanBuffer() = default;
		VulkanBuffer(VulkanBuffer const&) = delete;
		VulkanBuffer(VulkanBuffer&&) = default;
		~VulkanBuffer() = default;

		bool create(Vk::Device const& device, VkBufferUsageFlags usage, VkMemoryPropertyFlagBits memoryProperty, VkDeviceSize size);

		VulkanBuffer& operator=(VulkanBuffer const&) = delete;
		VulkanBuffer& operator=(VulkanBuffer&&) = default;

		Vk::Buffer handle;
		Vk::DeviceMemory memory;
	private:
		bool allocateBufferMemory(Vk::Device const& device, VkMemoryPropertyFlagBits memoryProperty);
	};
}

#endif
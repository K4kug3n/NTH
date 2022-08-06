#ifndef NTH_RENDERER_VULKANBUFFER_HPP
#define NTH_RENDERER_VULKANBUFFER_HPP

#include "Renderer/Vulkan/Buffer.hpp"
#include "Renderer/Vulkan/DeviceMemory.hpp"

namespace Nth {
	namespace Vk {
		class Device;
		class CommandBuffer;
		class Queue;
	}

	class VulkanBuffer {
	public:
		VulkanBuffer() = default;
		VulkanBuffer(Vk::Device const& device, VkBufferUsageFlags usage, VkMemoryPropertyFlagBits memoryProperty, VkDeviceSize size);
		VulkanBuffer(VulkanBuffer const&) = delete;
		VulkanBuffer(VulkanBuffer&&) = default;
		~VulkanBuffer() = default;

		void copyByStaging(const void* data, size_t size, Vk::CommandBuffer& commandBuffer, Vk::Queue& presentQueue);

		VulkanBuffer& operator=(VulkanBuffer const&) = delete;
		VulkanBuffer& operator=(VulkanBuffer&&) = default;

		Vk::Buffer handle;
		Vk::DeviceMemory memory;

		Vk::Buffer staging;
		Vk::DeviceMemory stagingMemory;
	private:
		bool allocateBufferMemory(Vk::Device const& device, VkMemoryPropertyFlagBits memoryProperty, Vk::Buffer& buffer, Vk::DeviceMemory& memory);

		Vk::Device const* m_device;
	};
}

#endif
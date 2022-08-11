#ifndef NTH_RENDERER_VULKANBUFFER_HPP
#define NTH_RENDERER_VULKANBUFFER_HPP

#include "Renderer/Vulkan/Buffer.hpp"
#include "Renderer/Vulkan/DeviceMemory.hpp"

namespace Nth {
	namespace Vk {
		class CommandBuffer;
	}

	class VulkanDevice;

	class VulkanBuffer {
	public:
		VulkanBuffer();
		VulkanBuffer(VulkanDevice const& device, VkBufferUsageFlags usage, VkMemoryPropertyFlagBits memoryProperty, VkDeviceSize size);
		VulkanBuffer(VulkanBuffer const&) = delete;
		VulkanBuffer(VulkanBuffer&&) = default;
		~VulkanBuffer() = default;

		void copy(const void* data, size_t size, Vk::CommandBuffer& commandBuffer);

		VulkanBuffer& operator=(VulkanBuffer const&) = delete;
		VulkanBuffer& operator=(VulkanBuffer&&) = default;

		Vk::Buffer handle;
	private:
		bool allocateBufferMemory(Vk::Device const& device, VkMemoryPropertyFlagBits memoryProperty, Vk::Buffer& buffer, Vk::DeviceMemory& memory);
		void createStaging(Vk::Device const& device, VkDeviceSize size);
		void copyByStaging(const void* data, size_t size, Vk::CommandBuffer& commandBuffer);

		Vk::Buffer m_staging;
		Vk::DeviceMemory m_stagingMemory;

		Vk::DeviceMemory m_memory;

		VkMemoryPropertyFlagBits m_memoryProperty;

		VulkanDevice const* m_device;
	};
}

#endif
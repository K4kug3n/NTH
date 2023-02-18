#ifndef NTH_RENDERER_RENDERBUFFER_HPP
#define NTH_RENDERER_RENDERBUFFER_HPP

#include <Renderer/Vulkan/Buffer.hpp>
#include <Renderer/Vulkan/DeviceMemory.hpp>

namespace Nth {
	namespace Vk {
		class CommandBuffer;
	}

	class RenderDevice;

	class RenderBuffer {
	public:
		RenderBuffer();
		RenderBuffer(const RenderDevice& device, VkBufferUsageFlags usage, VkMemoryPropertyFlagBits memoryProperty, VkDeviceSize size);
		RenderBuffer(const RenderBuffer&) = delete;
		RenderBuffer(RenderBuffer&&) = default;
		~RenderBuffer() = default;

		void copy(const void* data, size_t size);

		RenderBuffer& operator=(const RenderBuffer&) = delete;
		RenderBuffer& operator=(RenderBuffer&&) = default;

		Vk::Buffer handle;
	private:
		void allocate_buffer_memory(const Vk::Device& device, VkMemoryPropertyFlagBits memoryProperty, Vk::Buffer& buffer, Vk::DeviceMemory& memory);
		void create_staging(const Vk::Device& device, VkDeviceSize size);
		void copy_by_staging(const void* data, size_t size);

		Vk::Buffer m_staging;
		Vk::DeviceMemory m_staging_memory;

		Vk::DeviceMemory m_memory;

		VkMemoryPropertyFlagBits m_memory_property;

		RenderDevice const* m_device;
	};
}

#endif
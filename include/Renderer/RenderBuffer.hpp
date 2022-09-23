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
		RenderBuffer(RenderDevice const& device, VkBufferUsageFlags usage, VkMemoryPropertyFlagBits memoryProperty, VkDeviceSize size);
		RenderBuffer(RenderBuffer const&) = delete;
		RenderBuffer(RenderBuffer&&) = default;
		~RenderBuffer() = default;

		void copy(const void* data, size_t size);

		RenderBuffer& operator=(RenderBuffer const&) = delete;
		RenderBuffer& operator=(RenderBuffer&&) = default;

		Vk::Buffer handle;
	private:
		bool allocateBufferMemory(Vk::Device const& device, VkMemoryPropertyFlagBits memoryProperty, Vk::Buffer& buffer, Vk::DeviceMemory& memory);
		void createStaging(Vk::Device const& device, VkDeviceSize size);
		void copyByStaging(const void* data, size_t size);

		Vk::Buffer m_staging;
		Vk::DeviceMemory m_stagingMemory;

		Vk::DeviceMemory m_memory;

		VkMemoryPropertyFlagBits m_memoryProperty;

		RenderDevice const* m_device;
	};
}

#endif
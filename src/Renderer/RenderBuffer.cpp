#include <Renderer/RenderBuffer.hpp>

#include <Renderer/RenderDevice.hpp>
#include <Renderer/Vulkan/PhysicalDevice.hpp>
#include <Renderer/Vulkan/CommandBuffer.hpp>
#include <Renderer/Vulkan/Queue.hpp>

#include <cstring>
#include <stdexcept>
#include <cassert>

namespace Nth {
	RenderBuffer::RenderBuffer() :
		m_device(nullptr),
		m_memory_property(VK_MEMORY_PROPERTY_FLAG_BITS_MAX_ENUM) { }

	RenderBuffer::RenderBuffer(const RenderDevice& device, VkBufferUsageFlags usage, VkMemoryPropertyFlagBits memoryProperty, VkDeviceSize size) :
		m_device(&device),
		m_memory_property(memoryProperty) {
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

		handle.create(device.get_handle(), bufferCreateInfo);

		allocate_buffer_memory(device.get_handle(), memoryProperty, handle, m_memory);

		handle.bind_buffer_memory(m_memory);

		if (memoryProperty & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
			create_staging(device.get_handle(), size);
		}
	}

	void RenderBuffer::copy(const void* data, size_t size) {
		if (m_memory_property & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
			copy_by_staging(data, size);
		}
		else {
			m_memory.map(0, size, 0);
			void* mappedPtr = m_memory.get_mapped_pointer();

			std::memcpy(mappedPtr, data, size);

			m_memory.flush_mapped_memory(0, size);

			m_memory.unmap();
		}
	}

	void RenderBuffer::allocate_buffer_memory(const Vk::Device& device, VkMemoryPropertyFlagBits memory_property, Vk::Buffer& buffer, Vk::DeviceMemory& memory) {
		VkMemoryRequirements bufferMemoryRequirements = buffer.get_memory_requirements();;
		VkPhysicalDeviceMemoryProperties memoryProperties = device.get_physical_device().get_memory_properties();

		for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i) {
			if ((bufferMemoryRequirements.memoryTypeBits & (1 << i)) &&
				(memoryProperties.memoryTypes[i].propertyFlags & memory_property) == memory_property) {

				VkMemoryAllocateInfo memoryAllocateInfo = {
					VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,     // VkStructureType                        sType
					nullptr,                                    // const void                            *pNext
					bufferMemoryRequirements.size,              // VkDeviceSize                           allocationSize
					i                                           // uint32_t                               memoryTypeIndex
				};

				memory.create(device, memoryAllocateInfo);
			}
		}
	}

	void RenderBuffer::create_staging(const Vk::Device& device, VkDeviceSize size) {
		VkBufferCreateInfo stagingCreateInfo = {
			VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,             // VkStructureType                sType
			nullptr,                                          // const void                    *pNext
			0,                                                // VkBufferCreateFlags            flags
			size,                                             // VkDeviceSize                   size
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,                 // VkBufferUsageFlags             usage
			VK_SHARING_MODE_EXCLUSIVE,                        // VkSharingMode                  sharingMode
			0,                                                // uint32_t                       queueFamilyIndexCount
			nullptr                                           // const uint32_t                *pQueueFamilyIndices
		};

		m_staging.create(device, stagingCreateInfo);

		allocate_buffer_memory(device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, m_staging, m_staging_memory);

		m_staging.bind_buffer_memory(m_staging_memory);
	}

	void RenderBuffer::copy_by_staging(const void* data, size_t size) {
		assert(m_device != nullptr);

		m_staging_memory.map(0, handle.get_size(), 0);

		void* staging_buffer_memory_pointer = m_staging_memory.get_mapped_pointer();

		std::memcpy(staging_buffer_memory_pointer, data, size);

		m_staging_memory.flush_mapped_memory(0, m_staging.get_size());

		m_staging_memory.unmap();

		VkCommandBufferBeginInfo command_buffer_begin_info = {
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, // VkStructureType              sType
			nullptr,                                     // const void                  *pNext
			VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, // VkCommandBufferUsageFlags    flags
			nullptr                                      // const VkCommandBufferInheritanceInfo  *pInheritanceInfo
		};

		Vk::CommandBuffer command_buffer{ m_device->allocate_command_buffer() };
		command_buffer.begin(command_buffer_begin_info);

		VkBufferCopy bufferCopyInfo = {
			0,                                // VkDeviceSize       srcOffset
			0,                                // VkDeviceSize       dstOffset
			handle.get_size()                  // VkDeviceSize       size
		};
		command_buffer.copy_buffer(m_staging(), handle(), bufferCopyInfo);

		VkBufferMemoryBarrier buffer_memory_barrier = {
			VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER, // VkStructureType    sType;
			nullptr,                                 // const void        *pNext
			VK_ACCESS_TRANSFER_WRITE_BIT,            // VkAccessFlags      srcAccessMask
			VK_ACCESS_UNIFORM_READ_BIT,              // VkAccessFlags      dstAccessMask
			VK_QUEUE_FAMILY_IGNORED,                 // uint32_t           srcQueueFamilyIndex
			VK_QUEUE_FAMILY_IGNORED,                 // uint32_t           dstQueueFamilyIndex
			handle(),                                // VkBuffer           buffer
			0,                                       // VkDeviceSize       offset
			VK_WHOLE_SIZE                            // VkDeviceSize       size
		};
		command_buffer.pipeline_barrier(VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, 0, 0, nullptr, 1, &buffer_memory_barrier, 0, nullptr);

		command_buffer.end();

		// Submit command buffer and copy data from staging buffer to a vertex buffer
		VkCommandBuffer vk_command_buffer = command_buffer();
		VkSubmitInfo submit_info = {
			VK_STRUCTURE_TYPE_SUBMIT_INFO,    // VkStructureType    sType
			nullptr,                          // const void        *pNext
			0,                                // uint32_t           waitSemaphoreCount
			nullptr,                          // const VkSemaphore *pWaitSemaphores
			nullptr,                          // const VkPipelineStageFlags *pWaitDstStageMask;
			1,                                // uint32_t           commandBufferCount
			&vk_command_buffer,               // const VkCommandBuffer *pCommandBuffers
			0,                                // uint32_t           signalSemaphoreCount
			nullptr                           // const VkSemaphore *pSignalSemaphores
		};

		m_device->present_queue().submit(submit_info, VK_NULL_HANDLE);

		m_device->get_handle().wait_idle();
	}
}

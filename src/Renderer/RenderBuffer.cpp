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
		m_memoryProperty(VK_MEMORY_PROPERTY_FLAG_BITS_MAX_ENUM) { }

	RenderBuffer::RenderBuffer(RenderDevice const& device, VkBufferUsageFlags usage, VkMemoryPropertyFlagBits memoryProperty, VkDeviceSize size) :
		m_device(&device),
		m_memoryProperty(memoryProperty) {
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

		if (!handle.create(device.getHandle(), bufferCreateInfo)) {
			throw std::runtime_error("Could not create buffer!");
		}

		if (!allocateBufferMemory(device.getHandle(), memoryProperty, handle, m_memory)) {
			throw std::runtime_error("Could not allocate memory for buffer!");
		}

		if (!handle.bindBufferMemory(m_memory)) {
			throw std::runtime_error("Could not bind memory to buffer!");
		}

		if (memoryProperty & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
			createStaging(device.getHandle(), size);
		}
	}

	void RenderBuffer::copy(const void* data, size_t size) {
		if (m_memoryProperty & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
			copyByStaging(data, size);
		}
		else {
			m_memory.map(0, size, 0);
			void* mappedPtr = m_memory.getMappedPointer();

			std::memcpy(mappedPtr, data, size);

			m_memory.flushMappedMemory(0, size);

			m_memory.unmap();
		}
	}

	bool RenderBuffer::allocateBufferMemory(Vk::Device const& device, VkMemoryPropertyFlagBits memoryProperty, Vk::Buffer& buffer, Vk::DeviceMemory& memory) {
		VkMemoryRequirements bufferMemoryRequirements = buffer.getMemoryRequirements();;
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

	void RenderBuffer::createStaging(Vk::Device const& device, VkDeviceSize size) {
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

		if (!m_staging.create(device, stagingCreateInfo)) {
			throw std::runtime_error("Could not create staging!");
		}

		if (!allocateBufferMemory(device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, m_staging, m_stagingMemory)) {
			throw std::runtime_error("Could not allocate staging!");
		}

		if (!m_staging.bindBufferMemory(m_stagingMemory)) {
			throw std::runtime_error("Could not bind staging memory!");
		}
	}

	void RenderBuffer::copyByStaging(const void* data, size_t size) {
		assert(m_device != nullptr);

		if (!m_stagingMemory.map(0, handle.getSize(), 0)) {
			throw std::runtime_error("Could not map memory and upload data to a staging buffer!");
		}

		void* stagingBufferMemoryPointer = m_stagingMemory.getMappedPointer();

		std::memcpy(stagingBufferMemoryPointer, data, size);

		m_stagingMemory.flushMappedMemory(0, m_staging.getSize());

		m_stagingMemory.unmap();

		VkCommandBufferBeginInfo commandBufferBeginInfo = {
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, // VkStructureType              sType
			nullptr,                                     // const void                  *pNext
			VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, // VkCommandBufferUsageFlags    flags
			nullptr                                      // const VkCommandBufferInheritanceInfo  *pInheritanceInfo
		};

		Vk::CommandBuffer commandBuffer{ m_device->allocateCommandBuffer() };
		commandBuffer.begin(commandBufferBeginInfo);

		VkBufferCopy bufferCopyInfo = {
			0,                                // VkDeviceSize       srcOffset
			0,                                // VkDeviceSize       dstOffset
			handle.getSize()                  // VkDeviceSize       size
		};
		commandBuffer.copyBuffer(m_staging(), handle(), bufferCopyInfo);

		VkBufferMemoryBarrier bufferMemoryBarrier = {
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
		commandBuffer.pipelineBarrier(VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, 0, 0, nullptr, 1, &bufferMemoryBarrier, 0, nullptr);

		commandBuffer.end();

		// Submit command buffer and copy data from staging buffer to a vertex buffer
		VkSubmitInfo submitInfo = {
			VK_STRUCTURE_TYPE_SUBMIT_INFO,    // VkStructureType    sType
			nullptr,                          // const void        *pNext
			0,                                // uint32_t           waitSemaphoreCount
			nullptr,                          // const VkSemaphore *pWaitSemaphores
			nullptr,                          // const VkPipelineStageFlags *pWaitDstStageMask;
			1,                                // uint32_t           commandBufferCount
			&commandBuffer(),                 // const VkCommandBuffer *pCommandBuffers
			0,                                // uint32_t           signalSemaphoreCount
			nullptr                           // const VkSemaphore *pSignalSemaphores
		};

		if (!m_device->presentQueue().submit(submitInfo, VK_NULL_HANDLE)) {
			throw std::runtime_error("Can't submit copy");
		}

		m_device->getHandle().waitIdle();
	}
}

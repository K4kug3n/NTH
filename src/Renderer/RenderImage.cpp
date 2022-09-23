#include <Renderer/RenderImage.hpp>

#include <Renderer/Vulkan/Device.hpp>
#include <Renderer/Vulkan/PhysicalDevice.hpp>
#include <Renderer/RenderDevice.hpp>
#include <Renderer/Vulkan/CommandBuffer.hpp>

#include <cstring>
#include <iostream>
#include <cassert>

namespace Nth {
	void RenderImage::create(RenderDevice const& device, uint32_t width, uint32_t height, size_t stagingSize, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties) {
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

		if (!handle.create(device.getHandle(), imageCreateInfo)) {
			throw std::runtime_error("Can't create image for Vulkan Image");
		}

		VkMemoryRequirements memRequirements = handle.getImageMemoryRequirements();

		VkMemoryAllocateInfo memoryAllocateInfo = {
			VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,                                        // VkStructureType                        sType
			nullptr,                                                                       // const void                            *pNext
			memRequirements.size,                                                          // VkDeviceSize                           allocationSize
			findMemoryType(device.getHandle(), memRequirements.memoryTypeBits, properties) // uint32_t                               memoryTypeIndex
		};

		if (!memory.create(device.getHandle(), memoryAllocateInfo)) {
			throw std::runtime_error("Can't create memory for Vulkan Image");
		}

		if (!handle.bindImageMemory(memory)) {
			throw std::runtime_error("Could not bind image memory to buffer!");
		}

		if (stagingSize > 0) {
			createStaging(device.getHandle(), stagingSize);
		}

		m_device = &device;
	}

	void RenderImage::createView(VkFormat format, VkImageAspectFlags aspectFlags) {
		assert(m_device != nullptr);

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

		if (!view.create(m_device->getHandle(), imageViewCreateInfo)) {
			throw std::runtime_error("Can't create view for Vulkan Image");
		}
	}

	void RenderImage::copy(void const* data, size_t size, uint32_t width, uint32_t height) {
		assert(m_device != nullptr);

		if (!m_stagingMemory.map(0, size, 0)) {
			throw std::runtime_error("Could not map memory and upload texture data to a staging buffer!");
		}
		void* stagingBufferMemoryPointer = m_stagingMemory.getMappedPointer();

		std::memcpy(stagingBufferMemoryPointer, data, size);

		if (!m_stagingMemory.flushMappedMemory(0, size)) {
			throw std::runtime_error("Could not flush mapped memory to a staging buffer!");
		}

		m_stagingMemory.unmap();

		// Prepare command buffer to copy data from staging buffer to a vertex buffer
		VkCommandBufferBeginInfo commandBufferBeginInfo = {
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,  // VkStructureType                        sType
			nullptr,                                      // const void                            *pNext
			VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,  // VkCommandBufferUsageFlags              flags
			nullptr                                       // const VkCommandBufferInheritanceInfo  *pInheritanceInfo
		};

		Vk::CommandBuffer commandBuffer{ m_device->allocateCommandBuffer() };
		commandBuffer.begin(commandBufferBeginInfo);

		VkImageSubresourceRange image_subresource_range = {
			VK_IMAGE_ASPECT_COLOR_BIT,              // VkImageAspectFlags        aspectMask
			0,                                      // uint32_t                  baseMipLevel
			1,                                      // uint32_t                  levelCount
			0,                                      // uint32_t                  baseArrayLayer
			1                                       // uint32_t                  layerCount
		};

		VkImageMemoryBarrier imageMemoryBarrierFromUndefinedToTransferDst = {
			VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER, // VkStructureType           sType
			nullptr,                                // const void               *pNext
			0,                                      // VkAccessFlags             srcAccessMask
			VK_ACCESS_TRANSFER_WRITE_BIT,           // VkAccessFlags             dstAccessMask
			VK_IMAGE_LAYOUT_UNDEFINED,              // VkImageLayout             oldLayout
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,   // VkImageLayout             newLayout
			VK_QUEUE_FAMILY_IGNORED,                // uint32_t                  srcQueueFamilyIndex
			VK_QUEUE_FAMILY_IGNORED,                // uint32_t                  dstQueueFamilyIndex
			handle(),                               // VkImage                   image
			image_subresource_range                 // VkImageSubresourceRange   subresourceRange
		};
		commandBuffer.pipelineBarrier(VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrierFromUndefinedToTransferDst);

		VkBufferImageCopy bufferImageCopyInfo = {
			0,                                  // VkDeviceSize               bufferOffset
			0,                                  // uint32_t                   bufferRowLength
			0,                                  // uint32_t                   bufferImageHeight
			{                                   // VkImageSubresourceLayers   imageSubresource
				VK_IMAGE_ASPECT_COLOR_BIT,          // VkImageAspectFlags         aspectMask
				0,                                  // uint32_t                   mipLevel
				0,                                  // uint32_t                   baseArrayLayer
				1                                   // uint32_t                   layerCount
			},
			{                                   // VkOffset3D                 imageOffset
				0,                                  // int32_t                    x
				0,                                  // int32_t                    y
				0                                   // int32_t                    z
			},
			{                                   // VkExtent3D                 imageExtent
				width,                              // uint32_t                   width
				height,                             // uint32_t                   height
				1                                   // uint32_t                   depth
			}
		};
		commandBuffer.copyBufferToImage(m_staging(), handle(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferImageCopyInfo);

		VkImageMemoryBarrier imageMemoryBarrierFromTransferToShaderRead = {
			VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,   // VkStructureType              sType
			nullptr,                                  // const void                  *pNext
			VK_ACCESS_TRANSFER_WRITE_BIT,             // VkAccessFlags                srcAccessMask
			VK_ACCESS_SHADER_READ_BIT,                // VkAccessFlags                dstAccessMask
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,     // VkImageLayout                oldLayout
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, // VkImageLayout                newLayout
			VK_QUEUE_FAMILY_IGNORED,                  // uint32_t                     srcQueueFamilyIndex
			VK_QUEUE_FAMILY_IGNORED,                  // uint32_t                     dstQueueFamilyIndex
			handle(),                                 // VkImage                      image
			image_subresource_range                   // VkImageSubresourceRange      subresourceRange
		};
		commandBuffer.pipelineBarrier(VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrierFromTransferToShaderRead);

		commandBuffer.end();

		// Submit command buffer and copy data from staging buffer to a vertex buffer
		VkSubmitInfo submitInfo = {
			VK_STRUCTURE_TYPE_SUBMIT_INFO,            // VkStructureType              sType
			nullptr,                                  // const void                  *pNext
			0,                                        // uint32_t                     waitSemaphoreCount
			nullptr,                                  // const VkSemaphore           *pWaitSemaphores
			nullptr,                                  // const VkPipelineStageFlags  *pWaitDstStageMask;
			1,                                        // uint32_t                     commandBufferCount
			&commandBuffer(),                         // const VkCommandBuffer       *pCommandBuffers
			0,                                        // uint32_t                     signalSemaphoreCount
			nullptr                                   // const VkSemaphore           *pSignalSemaphores
		};

		if (!m_device->graphicsQueue().submit(submitInfo, VK_NULL_HANDLE)) {
			throw std::runtime_error("Can't submit to present queue");
		}

		m_device->getHandle().waitIdle();
	}

	uint32_t RenderImage::findMemoryType(Vk::Device const& device, uint32_t memoryTypeBit, VkMemoryPropertyFlags properties) const {
		VkPhysicalDeviceMemoryProperties memProperties = device.getPhysicalDevice().getMemoryProperties();

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((memoryTypeBit & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}

		throw std::runtime_error("Canno't find suitable memory type!");
	}
	void RenderImage::createStaging(Vk::Device const& device, size_t size) {
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

		VkMemoryRequirements stagingMemRequirements = m_staging.getMemoryRequirements();

		VkMemoryAllocateInfo stagingMemoryAllocateInfo = {
			VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,                                                             // VkStructureType     sType
			nullptr,                                                                                            // const void         *pNext
			stagingMemRequirements.size,                                                                        // VkDeviceSize        allocationSize
			findMemoryType(device, stagingMemRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)  // uint32_t            memoryTypeIndex
		};

		if (!m_stagingMemory.create(device, stagingMemoryAllocateInfo)) {
			throw std::runtime_error("Can't create memory for Vulkan Image");
		}

		if (!m_staging.bindBufferMemory(m_stagingMemory)) {
			throw std::runtime_error("Could not bind staging memory!");
		}
	}
}

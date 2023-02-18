#include <Renderer/RenderImage.hpp>

#include <Renderer/Vulkan/Device.hpp>
#include <Renderer/Vulkan/PhysicalDevice.hpp>
#include <Renderer/RenderDevice.hpp>
#include <Renderer/Vulkan/CommandBuffer.hpp>

#include <cstring>
#include <iostream>
#include <cassert>

namespace Nth {
	void RenderImage::create(const RenderDevice& device, uint32_t width, uint32_t height, size_t staging_size, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties) {
		VkImageCreateInfo image_create_info = {
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

		handle.create(device.get_handle(), image_create_info);

		VkMemoryRequirements mem_requirements = handle.get_image_memory_requirements();

		VkMemoryAllocateInfo memory_allocate_info = {
			VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,                                            // VkStructureType                        sType
			nullptr,                                                                           // const void                            *pNext
			mem_requirements.size,                                                             // VkDeviceSize                           allocationSize
			find_memory_type(device.get_handle(), mem_requirements.memoryTypeBits, properties) // uint32_t                               memoryTypeIndex
		};

		memory.create(device.get_handle(), memory_allocate_info);

		handle.bind_image_memory(memory);

		if (staging_size > 0) {
			create_staging(device.get_handle(), staging_size);
		}

		m_device = &device;
	}

	void RenderImage::create_view(VkFormat format, VkImageAspectFlags aspectFlags) {
		assert(m_device != nullptr);

		VkImageViewCreateInfo image_view_create_info = {
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

		view.create(m_device->get_handle(), image_view_create_info);
	}

	void RenderImage::copy(void const* data, size_t size, uint32_t width, uint32_t height) {
		assert(m_device != nullptr);

		m_staging_memory.map(0, size, 0);
		void* stagingBufferMemoryPointer = m_staging_memory.get_mapped_pointer();

		std::memcpy(stagingBufferMemoryPointer, data, size);

		m_staging_memory.flush_mapped_memory(0, size);

		m_staging_memory.unmap();

		// Prepare command buffer to copy data from staging buffer to a vertex buffer
		VkCommandBufferBeginInfo command_buffer_begin_info = {
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,  // VkStructureType                        sType
			nullptr,                                      // const void                            *pNext
			VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,  // VkCommandBufferUsageFlags              flags
			nullptr                                       // const VkCommandBufferInheritanceInfo  *pInheritanceInfo
		};

		Vk::CommandBuffer command_buffer{ m_device->allocate_command_buffer() };
		command_buffer.begin(command_buffer_begin_info);

		VkImageSubresourceRange image_subresource_range = {
			VK_IMAGE_ASPECT_COLOR_BIT,              // VkImageAspectFlags        aspectMask
			0,                                      // uint32_t                  baseMipLevel
			1,                                      // uint32_t                  levelCount
			0,                                      // uint32_t                  baseArrayLayer
			1                                       // uint32_t                  layerCount
		};

		VkImageMemoryBarrier image_memory_barrier_from_undefined_to_transfer_dst = {
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
		command_buffer.pipeline_barrier(VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &image_memory_barrier_from_undefined_to_transfer_dst);

		VkBufferImageCopy buffer_image_copy_info = {
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
		command_buffer.copy_buffer_to_image(m_staging(), handle(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &buffer_image_copy_info);

		VkImageMemoryBarrier image_memory_barrier_from_transfer_to_shader_read = {
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
		command_buffer.pipeline_barrier(VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &image_memory_barrier_from_transfer_to_shader_read);

		command_buffer.end();

		// Submit command buffer and copy data from staging buffer to a vertex buffer
		VkCommandBuffer vk_command_buffer = command_buffer();
		VkSubmitInfo submit_info = {
			VK_STRUCTURE_TYPE_SUBMIT_INFO,            // VkStructureType              sType
			nullptr,                                  // const void                  *pNext
			0,                                        // uint32_t                     waitSemaphoreCount
			nullptr,                                  // const VkSemaphore           *pWaitSemaphores
			nullptr,                                  // const VkPipelineStageFlags  *pWaitDstStageMask;
			1,                                        // uint32_t                     commandBufferCount
			&vk_command_buffer,                       // const VkCommandBuffer       *pCommandBuffers
			0,                                        // uint32_t                     signalSemaphoreCount
			nullptr                                   // const VkSemaphore           *pSignalSemaphores
		};

		m_device->graphics_queue().submit(submit_info, VK_NULL_HANDLE);

		m_device->get_handle().wait_idle();
	}

	uint32_t RenderImage::find_memory_type(const Vk::Device& device, uint32_t memory_type_bit, VkMemoryPropertyFlags properties) const {
		VkPhysicalDeviceMemoryProperties mem_properties = device.get_physical_device().get_memory_properties();

		for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++) {
			if ((memory_type_bit & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}

		throw std::runtime_error("Canno't find suitable memory type!");
	}
	void RenderImage::create_staging(const Vk::Device& device, size_t size) {
		VkBufferCreateInfo staging_create_info = {
			VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,             // VkStructureType                sType
			nullptr,                                          // const void                    *pNext
			0,                                                // VkBufferCreateFlags            flags
			size,                                             // VkDeviceSize                   size
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,                 // VkBufferUsageFlags             usage
			VK_SHARING_MODE_EXCLUSIVE,                        // VkSharingMode                  sharingMode
			0,                                                // uint32_t                       queueFamilyIndexCount
			nullptr                                           // const uint32_t                *pQueueFamilyIndices
		};

		m_staging.create(device, staging_create_info);

		VkMemoryRequirements staging_mem_requirements = m_staging.get_memory_requirements();

		VkMemoryAllocateInfo staging_memory_allocate_info = {
			VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,                                                             // VkStructureType     sType
			nullptr,                                                                                            // const void         *pNext
			staging_mem_requirements.size,                                                                        // VkDeviceSize        allocationSize
			find_memory_type(device, staging_mem_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)  // uint32_t            memoryTypeIndex
		};

		m_staging_memory.create(device, staging_memory_allocate_info);

		m_staging.bind_buffer_memory(m_staging_memory);
	}
}

#include <Renderer/RenderingResource.hpp>

#include <Renderer/Vulkan/CommandPool.hpp>
#include <Renderer/Vulkan/Device.hpp>
#include <Renderer/RenderSurface.hpp>

#include <Maths/Vector2.hpp>

namespace Nth {
	RenderingResource::RenderingResource(RenderInstance& instance, RenderSurface& surface):
		m_instance(instance),
		m_surface(surface),
		image_index(0),
		swapchain_image(VK_NULL_HANDLE) { }

	void RenderingResource::create(uint32_t familyIndex) {
		const Vk::Device& device{ m_instance.get_device().get_handle() };

		command_pool.create(device, familyIndex, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);

		command_pool.allocate_command_buffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, command_buffer);

		image_available_semaphore.create(device);

		finished_rendering_semaphore.create(device);

		fence.create(device, VK_FENCE_CREATE_SIGNALED_BIT);
	}

	void RenderingResource::prepare(std::function<void(Vk::CommandBuffer&)> action) {
		VkCommandBufferBeginInfo command_buffer_begin_info = {
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,        // VkStructureType                        sType
			nullptr,                                            // const void                            *pNext
			VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,        // VkCommandBufferUsageFlags              flags
			nullptr                                             // const VkCommandBufferInheritanceInfo  *pInheritanceInfo
		};

		command_buffer.begin(command_buffer_begin_info);

		VkImageSubresourceRange image_subresource_range = {
			VK_IMAGE_ASPECT_COLOR_BIT,                          // VkImageAspectFlags                     aspectMask
			0,                                                  // uint32_t                               baseMipLevel
			1,                                                  // uint32_t                               levelCount
			0,                                                  // uint32_t                               baseArrayLayer
			1                                                   // uint32_t                               layerCount
		};

		const RenderDevice& device{ m_instance.get_device() };

		if (device.present_queue()!= device.graphics_queue()) {
			VkImageMemoryBarrier barrier_from_present_to_draw = {
				VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,           // VkStructureType                        sType
				nullptr,                                          // const void                            *pNext
				VK_ACCESS_MEMORY_READ_BIT,                        // VkAccessFlags                          srcAccessMask
				VK_ACCESS_MEMORY_READ_BIT,                        // VkAccessFlags                          dstAccessMask
				VK_IMAGE_LAYOUT_UNDEFINED,                        // VkImageLayout                          oldLayout
				VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,                  // VkImageLayout                          newLayout
				device.present_queue().index(),                    // uint32_t                               srcQueueFamilyIndex
				device.graphics_queue().index(),                   // uint32_t                               dstQueueFamilyIndex
				swapchain_image,                                   // VkImage                                image
				image_subresource_range                             // VkImageSubresourceRange                subresourceRange
			};

			command_buffer.pipeline_barrier(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier_from_present_to_draw);
		}

		std::vector<VkClearValue> clear_values(2);
		clear_values[0].color = { 1.0f, 0.8f, 0.4f, 0.0f };
		clear_values[1].depthStencil = { 1.0f, 0 };

		Vector2ui size = m_surface.size();

		VkRenderPassBeginInfo render_pass_begin_info = {
			VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,           // VkStructureType                        sType
			nullptr,                                            // const void                            *pNext
			m_surface.get_render_pass()(),                      // VkRenderPass                           renderPass
			framebuffer(),                                      // VkFramebuffer                          framebuffer
			{                                                   // VkRect2D                               renderArea
				{                                                  // VkOffset2D                             offset
					0,                                                // int32_t                                x
					0                                                 // int32_t                                y
				},
				{                                                  // VkExtent2D                             extent;
					size.x,
					size.y
				}
			},
			static_cast<uint32_t>(clear_values.size()),         // uint32_t                               clearValueCount
			clear_values.data()                                 // const VkClearValue                    *pClearValues
		};

		command_buffer.begin_render_pass(render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport = {
			0.0f,                               // float                                  x
			0.0f,                               // float                                  y
			static_cast<float>(size.x),         // float                                  width
			static_cast<float>(size.y),         // float                                  height
			0.0f,                               // float                                  minDepth
			1.0f                                // float                                  maxDepth
		};

		VkRect2D scissor = {
			{                                   // VkOffset2D                             offset
				0,                                  // int32_t                                x
				0                                   // int32_t                                y
			},
			{                                   // VkExtent2D                             extent
				size.x,                             // uint32_t                               width
				size.y                              // uint32_t                               height
			}
		};

		command_buffer.set_viewport(viewport);
		command_buffer.set_scissor(scissor);

		action(command_buffer);

		command_buffer.end_render_pass();

		if (device.present_queue() != device.graphics_queue()) {
			VkImageMemoryBarrier barrier_from_draw_to_present = {
				VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,           // VkStructureType                        sType
				nullptr,                                          // const void                            *pNext
				VK_ACCESS_MEMORY_READ_BIT,                        // VkAccessFlags                          srcAccessMask
				VK_ACCESS_MEMORY_READ_BIT,                        // VkAccessFlags                          dstAccessMask
				VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,                  // VkImageLayout                          oldLayout
				VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,                  // VkImageLayout                          newLayout
				device.graphics_queue().index(),                   // uint32_t                               srcQueueFamilyIndex
				device.present_queue().index(),                    // uint32_t                               dstQueueFamilyIndex
				swapchain_image,                                   // VkImage                                image
				image_subresource_range                             // VkImageSubresourceRange                subresourceRange
			};
			command_buffer.pipeline_barrier(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier_from_draw_to_present);
		}

		command_buffer.end();
	}

	void RenderingResource::present(const Vector2ui& size) {
		VkPipelineStageFlags wait_dst_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		VkSemaphore vk_image_available_semaphore = image_available_semaphore();
		VkCommandBuffer vk_command_buffer = command_buffer();
		VkSemaphore vk_finished_rendering_semaphore = finished_rendering_semaphore();
		VkSubmitInfo submit_info = {
			VK_STRUCTURE_TYPE_SUBMIT_INFO,               // VkStructureType              sType
			nullptr,                                     // const void                  *pNext
			1,                                           // uint32_t                     waitSemaphoreCount
			&vk_image_available_semaphore,               // const VkSemaphore           *pWaitSemaphores
			&wait_dst_stage_mask,                        // const VkPipelineStageFlags  *pWaitDstStageMask;
			1,                                           // uint32_t                     commandBufferCount
			&vk_command_buffer,                          // const VkCommandBuffer       *pCommandBuffers
			1,                                           // uint32_t                     signalSemaphoreCount
			&vk_finished_rendering_semaphore             // const VkSemaphore           *pSignalSemaphores
		};

		m_instance.get_device().graphics_queue().submit(submit_info, fence());

		m_surface.present(image_index, finished_rendering_semaphore, size);
	}
}

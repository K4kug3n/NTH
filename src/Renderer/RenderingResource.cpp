#include <Renderer/RenderingResource.hpp>

#include <Renderer/Vulkan/CommandPool.hpp>
#include <Renderer/Vulkan/Device.hpp>
#include <Renderer/RenderWindow.hpp>

#include <iostream>

namespace Nth {
	RenderingResource::RenderingResource(RenderWindow& owner):
		m_owner(owner){ }

	bool RenderingResource::create(uint32_t familyIndex) {
		Vk::Device const& device{ m_owner.getDevice().getHandle() };

		if (!commandPool.create(device, familyIndex, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT)) {
			std::cerr << "Can't create command buffer pool" << std::endl;
			return false;
		}

		if (!commandPool.allocateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, commandBuffer)) {
			std::cerr << "Can't allocate command buffer" << std::endl;
			return false;
		}

		if (!imageAvailableSemaphore.create(device)) {
			std::cerr << "Error: Can't create image available semaphore" << std::endl;
			return false;
		}

		if (!finishedRenderingSemaphore.create(device)) {
			std::cerr << "Error: Can't create rendering finished semaphore" << std::endl;
			return false;
		}

		if (!fence.create(device, VK_FENCE_CREATE_SIGNALED_BIT)) {
			std::cerr << "Could not create a fence!" << std::endl;
			return false;
		}

		return true;
	}

	void RenderingResource::prepare(std::function<void(Vk::CommandBuffer&)> action) {
		VkCommandBufferBeginInfo commandBufferBeginInfo = {
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,        // VkStructureType                        sType
			nullptr,                                            // const void                            *pNext
			VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,        // VkCommandBufferUsageFlags              flags
			nullptr                                             // const VkCommandBufferInheritanceInfo  *pInheritanceInfo
		};

		commandBuffer.begin(commandBufferBeginInfo);

		VkImageSubresourceRange imageSubresourceRange = {
			VK_IMAGE_ASPECT_COLOR_BIT,                          // VkImageAspectFlags                     aspectMask
			0,                                                  // uint32_t                               baseMipLevel
			1,                                                  // uint32_t                               levelCount
			0,                                                  // uint32_t                               baseArrayLayer
			1                                                   // uint32_t                               layerCount
		};

		VulkanDevice const& device{ m_owner.getDevice() };

		if (device.presentQueue()!= device.graphicsQueue()) {
			VkImageMemoryBarrier barrierFromPresentToDraw = {
				VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,           // VkStructureType                        sType
				nullptr,                                          // const void                            *pNext
				VK_ACCESS_MEMORY_READ_BIT,                        // VkAccessFlags                          srcAccessMask
				VK_ACCESS_MEMORY_READ_BIT,                        // VkAccessFlags                          dstAccessMask
				VK_IMAGE_LAYOUT_UNDEFINED,                        // VkImageLayout                          oldLayout
				VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,                  // VkImageLayout                          newLayout
				device.presentQueue().index(),                    // uint32_t                               srcQueueFamilyIndex
				device.graphicsQueue().index(),                   // uint32_t                               dstQueueFamilyIndex
				swapchainImage,                                   // VkImage                                image
				imageSubresourceRange                             // VkImageSubresourceRange                subresourceRange
			};

			commandBuffer.pipelineBarrier(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrierFromPresentToDraw);
		}

		std::vector<VkClearValue> clearValues(2);
		clearValues[0].color = { 1.0f, 0.8f, 0.4f, 0.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };

		Vector2ui size = m_owner.size();

		VkRenderPassBeginInfo renderPassBeginInfo = {
			VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,           // VkStructureType                        sType
			nullptr,                                            // const void                            *pNext
			m_owner.getRenderPass()(),                          // VkRenderPass                           renderPass
			framebuffer(),                                      // VkFramebuffer                          framebuffer
			{                                                   // VkRect2D                               renderArea
				{                                                 // VkOffset2D                             offset
					0,                                                // int32_t                                x
					0                                                 // int32_t                                y
				},
				{                                               // VkExtent2D                             extent;
					size.x,
					size.y
				}
			},
			static_cast<uint32_t>(clearValues.size()),         // uint32_t                               clearValueCount
			clearValues.data()                                 // const VkClearValue                    *pClearValues
		};

		commandBuffer.beginRenderPass(renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

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

		commandBuffer.setViewport(viewport);
		commandBuffer.setScissor(scissor);

		action(commandBuffer);

		commandBuffer.endRenderPass();

		if (device.presentQueue() != device.graphicsQueue()) {
			VkImageMemoryBarrier barrierFromDrawToPresent = {
				VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,           // VkStructureType                        sType
				nullptr,                                          // const void                            *pNext
				VK_ACCESS_MEMORY_READ_BIT,                        // VkAccessFlags                          srcAccessMask
				VK_ACCESS_MEMORY_READ_BIT,                        // VkAccessFlags                          dstAccessMask
				VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,                  // VkImageLayout                          oldLayout
				VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,                  // VkImageLayout                          newLayout
				device.graphicsQueue().index(),                   // uint32_t                               srcQueueFamilyIndex
				device.presentQueue().index(),                    // uint32_t                               dstQueueFamilyIndex
				swapchainImage,                                   // VkImage                                image
				imageSubresourceRange                             // VkImageSubresourceRange                subresourceRange
			};
			commandBuffer.pipelineBarrier(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrierFromDrawToPresent);
		}

		if (!commandBuffer.end()) {
			throw std::runtime_error("Could not record command buffer !");
		}
	}

	void RenderingResource::present() {
		VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		VkSubmitInfo submitInfo = {
			VK_STRUCTURE_TYPE_SUBMIT_INFO,               // VkStructureType              sType
			nullptr,                                     // const void                  *pNext
			1,                                           // uint32_t                     waitSemaphoreCount
			&imageAvailableSemaphore(),                  // const VkSemaphore           *pWaitSemaphores
			&waitDstStageMask,                           // const VkPipelineStageFlags  *pWaitDstStageMask;
			1,                                           // uint32_t                     commandBufferCount
			&commandBuffer(),                            // const VkCommandBuffer       *pCommandBuffers
			1,                                           // uint32_t                     signalSemaphoreCount
			&finishedRenderingSemaphore()                // const VkSemaphore           *pSignalSemaphores
		};

		if (!m_owner.getDevice().graphicsQueue().submit(submitInfo, fence())) {
			throw std::runtime_error("Can't submit ressource to graphics queue");
		}

		m_owner.present(imageIndex, finishedRenderingSemaphore);
	}
}

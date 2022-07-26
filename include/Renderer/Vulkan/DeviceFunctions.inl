
#if !defined(NTH_RENDERER_VK_DEVICE_FUNCTION)
#define NTH_RENDERER_VK_DEVICE_FUNCTION( fun )
#endif

#if !defined(NTH_RENDERER_VK_DEVICE_EXT_FUNCTION_BEGIN)
#define NTH_RENDERER_VK_DEVICE_EXT_FUNCTION_BEGIN(ext)
#endif

#if !defined(NTH_RENDERER_VK_DEVICE_EXT_FUNCTION_END)
#define NTH_RENDERER_VK_DEVICE_EXT_FUNCTION_END()
#endif


NTH_RENDERER_VK_DEVICE_FUNCTION(vkGetDeviceQueue)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkDeviceWaitIdle)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkDestroyDevice)

NTH_RENDERER_VK_DEVICE_FUNCTION(vkCreateSemaphore)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkCreateCommandPool)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkAllocateCommandBuffers)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkBeginCommandBuffer)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkCmdPipelineBarrier)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkCmdClearColorImage)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkEndCommandBuffer)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkQueueSubmit)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkFreeCommandBuffers)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkDestroyCommandPool)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkDestroySemaphore)

NTH_RENDERER_VK_DEVICE_EXT_FUNCTION_BEGIN(VK_KHR_swapchain)
	NTH_RENDERER_VK_DEVICE_FUNCTION(vkCreateSwapchainKHR)
	NTH_RENDERER_VK_DEVICE_FUNCTION(vkGetSwapchainImagesKHR)
	NTH_RENDERER_VK_DEVICE_FUNCTION(vkAcquireNextImageKHR)
	NTH_RENDERER_VK_DEVICE_FUNCTION(vkQueuePresentKHR)
	NTH_RENDERER_VK_DEVICE_FUNCTION(vkDestroySwapchainKHR)
NTH_RENDERER_VK_DEVICE_EXT_FUNCTION_END()

NTH_RENDERER_VK_DEVICE_FUNCTION(vkCreateImageView)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkCreateRenderPass)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkCreateFramebuffer)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkCreateShaderModule)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkCreatePipelineLayout)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkCreateGraphicsPipelines)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkCmdBeginRenderPass)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkCmdBindPipeline)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkCmdDraw)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkCmdDrawIndexed)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkCmdEndRenderPass)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkDestroyShaderModule)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkDestroyPipelineLayout)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkDestroyPipeline)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkDestroyRenderPass)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkDestroyFramebuffer)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkDestroyImageView)

NTH_RENDERER_VK_DEVICE_FUNCTION(vkCreateFence)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkCreateBuffer)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkGetBufferMemoryRequirements)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkAllocateMemory)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkBindBufferMemory)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkMapMemory)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkFlushMappedMemoryRanges)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkInvalidateMappedMemoryRanges)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkUnmapMemory)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkCmdSetViewport)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkCmdSetScissor)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkCmdBindVertexBuffers)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkCmdBindIndexBuffer)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkCmdPushConstants)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkResetCommandPool)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkWaitForFences)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkResetFences)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkFreeMemory)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkDestroyBuffer)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkDestroyFence)

NTH_RENDERER_VK_DEVICE_FUNCTION(vkCmdCopyBuffer)

NTH_RENDERER_VK_DEVICE_FUNCTION(vkCreateImage)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkGetImageMemoryRequirements)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkBindImageMemory)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkCreateSampler)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkCmdCopyBufferToImage)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkCreateDescriptorSetLayout)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkCreateDescriptorPool)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkAllocateDescriptorSets)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkUpdateDescriptorSets)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkCmdBindDescriptorSets)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkDestroyDescriptorPool)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkDestroyDescriptorSetLayout)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkDestroySampler)
NTH_RENDERER_VK_DEVICE_FUNCTION(vkDestroyImage)

#undef NTH_RENDERER_VK_DEVICE_FUNCTION
#undef NTH_RENDERER_VK_DEVICE_EXT_FUNCTION_BEGIN
#undef NTH_RENDERER_VK_DEVICE_EXT_FUNCTION_END
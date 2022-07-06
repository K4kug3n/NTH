#include "Renderer/RenderWindow.hpp"

#include "Renderer/ImageView.hpp"

#include "Util/Reader.hpp"

#include <iostream>

namespace Nth {
	RenderWindow::RenderWindow(VulkanInstance& vulkanInstance) :
		m_vulkanInstance(vulkanInstance),
		m_surface(vulkanInstance.getInstance()),
		m_presentQueue(),
		m_graphicsQueue(),
		m_imageAvailableSemaphore(),
		m_renderingFinishedSemaphore(),
		m_graphicCommandPool(),
		m_swapchainSize() { }

	RenderWindow::RenderWindow(VulkanInstance& vulkanInstance, VideoMode const& mode, std::string const& title) :
		m_vulkanInstance(vulkanInstance),
		m_surface(vulkanInstance.getInstance()),
		m_presentQueue(),
		m_graphicsQueue(),
		m_imageAvailableSemaphore(),
		m_renderingFinishedSemaphore(),
		m_graphicCommandPool(),
		m_swapchainSize() {
		this->create(mode, title);
	}

	RenderWindow::~RenderWindow() {
		if (m_vulkanInstance.getDevice()) {
			m_vulkanInstance.getDevice()->waitIdle();
		}

		m_swapchain.destroy();
	}

	bool RenderWindow::create(VideoMode const& mode, std::string const& title){
		if (!Window::create(mode, title)) {
			return false;
		}

		if (!m_surface.create(getHandle())) {
			std::cerr << "Error: Can't create surface" << std::endl;
			return false;
		}

		std::shared_ptr<Device> device{ m_vulkanInstance.createDevice(m_surface) };
		if (!device) {
			std::cerr << "Error: Can't create surface" << std::endl;
			return false;
		}
		
		if (!m_presentQueue.create(*device, device->getPresentQueueFamilyIndex())) {
			std::cerr << "Error: Can't create present queue" << std::endl;
			return false;
		}

		if (!m_graphicsQueue.create(*device, device->getGraphicQueueFamilyIndex())) {
			std::cerr << "Error: Can't create graphics queue" << std::endl;
			return false;
		}

		if (!m_imageAvailableSemaphore.create(*device)) {
			std::cerr << "Error: Can't create image available semaphore" << std::endl;
			return false;
		}

		if (!m_renderingFinishedSemaphore.create(*device)) {
			std::cerr << "Error: Can't create rendering finished semaphore" << std::endl;
			return false;
		}

		if (!createSwapchain()) {
			std::cerr << "Error: Can't create swapchain" << std::endl;
			return false;
		}

		if (!createRenderPass()) {
			std::cerr << "Error: Can't create renderpass" << std::endl;
			return false;
		}

		if (!createFrameBuffers()) {
			std::cerr << "Error: Can't create framebuffers" << std::endl;
			return false;
		}

		if (!createPipeline()) {
			std::cerr << "Can't create pipeline" << std::endl;
			return false;
		}

		if (!createCommandBuffers()) {
			std::cerr << "Error: Can't create command buffers" << std::endl;
			return false;
		}

		return true;
	}

	bool RenderWindow::draw() {
		if (m_swapchainSize != size()) {
			onWindowSizeChanged();
		}

		VkSemaphore imageAvailableSemaphore = m_imageAvailableSemaphore();
		VkSemaphore renderingFinishedSemaphore = m_renderingFinishedSemaphore();
		VkSwapchainKHR swapchain = m_swapchain();

		uint32_t imageIndex;
		VkResult result = m_swapchain.aquireNextImage(m_imageAvailableSemaphore(), VK_NULL_HANDLE, imageIndex);
		switch (result) {
		case VK_SUCCESS:
		case VK_SUBOPTIMAL_KHR:
			break;
		case VK_ERROR_OUT_OF_DATE_KHR:
			onWindowSizeChanged();
			return true;
		default:
			std::cerr << "Problem occurred during swap chain image acquisition !" << std::endl;
			return false;
		}

		VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
		VkSubmitInfo submitInfo = {
			VK_STRUCTURE_TYPE_SUBMIT_INFO,                // VkStructureType              sType
			nullptr,                                      // const void                  *pNext
			1,                                            // uint32_t                     waitSemaphoreCount
			&imageAvailableSemaphore,                   // const VkSemaphore           *pWaitSemaphores
			&waitDstStageMask,                         // const VkPipelineStageFlags  *pWaitDstStageMask;
			1,                                            // uint32_t                     commandBufferCount
			&m_graphicsCmdBuffers[imageIndex](),         // const VkCommandBuffer       *pCommandBuffers
			1,                                            // uint32_t                     signalSemaphoreCount
			&renderingFinishedSemaphore                 // const VkSemaphore           *pSignalSemaphores
		};

		if (!m_graphicsQueue.submit(submitInfo, VK_NULL_HANDLE)) {
			return false;
		}

		VkPresentInfoKHR presentInfo = {
			VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,           // VkStructureType              sType
			nullptr,                                      // const void                  *pNext
			1,                                            // uint32_t                     waitSemaphoreCount
			&renderingFinishedSemaphore,                // const VkSemaphore           *pWaitSemaphores
			1,                                            // uint32_t                     swapchainCount
			&swapchain,                                  // const VkSwapchainKHR        *pSwapchains
			&imageIndex,                                 // const uint32_t              *pImageIndices
			nullptr                                       // VkResult                    *pResults
		};

		result = m_presentQueue.present(presentInfo);

		switch (result) {
		case VK_SUCCESS:
			break;
		case VK_ERROR_OUT_OF_DATE_KHR:
		case VK_SUBOPTIMAL_KHR:
			onWindowSizeChanged();
			return true;
		default:
			std::cerr << "Problem occurred during image presentation !" << std::endl;
			return false;
		}

		return true;

	}

	bool RenderWindow::createSwapchain() {
		VkSurfaceCapabilitiesKHR surfaceCapabilities;
		if (!m_surface.getCapabilities(m_vulkanInstance.getDevice()->getPhysicalDevice(), surfaceCapabilities)) {
			std::cerr << "Error: Can't get surface capabilities" << std::endl;
			return false;
		}

		uint32_t imageCount{ getSwapchainNumImages(surfaceCapabilities) };
		VkImageUsageFlags swapchainUsageFlag{ getSwapchainUsageFlags(surfaceCapabilities) };
		VkSurfaceTransformFlagBitsKHR swapchainTransform{ getSwapchainTransform(surfaceCapabilities) };
		VkExtent2D swapchainExtend{ getSwapchainExtent(surfaceCapabilities, size()) };

		std::vector<VkSurfaceFormatKHR> surfaceFormats;
		if (!m_surface.getFormats(m_vulkanInstance.getDevice()->getPhysicalDevice(), surfaceFormats)) {
			std::cerr << "Error: Can't get surface formats" << std::endl;
			return false;
		}

		VkSurfaceFormatKHR surfaceFormat{ getSwapchainFormat(surfaceFormats) };

		std::vector<VkPresentModeKHR> presentModes;
		if (!m_surface.getPresentModes(m_vulkanInstance.getDevice()->getPhysicalDevice(), presentModes)) {
			std::cerr << "Error: Can't get surface present modes" << std::endl;
			return false;
		}

		VkPresentModeKHR presentMode{ getSwapchainPresentMode(presentModes) };

		if (static_cast<int>(swapchainUsageFlag) == -1) {
			return false;
		}
		if (static_cast<int>(presentMode) == -1) {
			return false;
		}

		VkSwapchainCreateInfoKHR swapchainCreateInfo = {
			VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,  // VkStructureType                sType
			nullptr,                                      // const void                    *pNext
			0,                                            // VkSwapchainCreateFlagsKHR      flags
			m_surface(),                                  // VkSurfaceKHR                   surface
			imageCount,                                   // uint32_t                       minImageCount
			surfaceFormat.format,                         // VkFormat                       imageFormat
			surfaceFormat.colorSpace,                     // VkColorSpaceKHR                imageColorSpace
			swapchainExtend,                              // VkExtent2D                     imageExtent
			1,                                            // uint32_t                       imageArrayLayers
			swapchainUsageFlag,                           // VkImageUsageFlags              imageUsage
			VK_SHARING_MODE_EXCLUSIVE,                    // VkSharingMode                  imageSharingMode
			0,                                            // uint32_t                       queueFamilyIndexCount
			nullptr,                                      // const uint32_t                *pQueueFamilyIndices
			swapchainTransform,                           // VkSurfaceTransformFlagBitsKHR  preTransform
			VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,            // VkCompositeAlphaFlagBitsKHR    compositeAlpha
			presentMode,                                  // VkPresentModeKHR               presentMode
			VK_TRUE,                                      // VkBool32                       clipped
			m_swapchain()                                 // VkSwapchainKHR                 oldSwapchain
		};

		Swapchain newSwapchain;
		if (!newSwapchain.create(*m_vulkanInstance.getDevice(), swapchainCreateInfo)) {
			std::cerr << "Error: Can't create swapchain" << std::endl;
			return false;
		}

		m_swapchain = std::move(newSwapchain);
		m_swapchainSize = size();

		return true;
	}

	bool RenderWindow::createCommandBuffers() {
		if (!m_graphicCommandPool.create(*m_vulkanInstance.getDevice(), m_presentQueue.index(), 0)) {
			std::cout << "Could not create a command pool!" << std::endl;
			return false;
		}

		uint32_t imageCount = m_swapchain.getImageCount();
		m_graphicsCmdBuffers = m_graphicCommandPool.allocateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, imageCount);

		if (!recordCommandBuffers()) {
			std::cout << "Could not record command buffers!" << std::endl;
			return false;
		}
		return true;
	}

	bool RenderWindow::recordCommandBuffers() {
		VkCommandBufferBeginInfo graphics_commandd_buffer_begin_info = {
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,    // VkStructureType                        sType
			nullptr,                                        // const void                            *pNext
			VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,   // VkCommandBufferUsageFlags              flags
			nullptr                                         // const VkCommandBufferInheritanceInfo  *pInheritanceInfo
		};

		VkImageSubresourceRange image_subresource_range = {
			VK_IMAGE_ASPECT_COLOR_BIT,                      // VkImageAspectFlags             aspectMask
			0,                                              // uint32_t                       baseMipLevel
			1,                                              // uint32_t                       levelCount
			0,                                              // uint32_t                       baseArrayLayer
			1                                               // uint32_t                       layerCount
		};

		VkClearValue clear_value = {
			{ 1.0f, 0.8f, 0.4f, 0.0f },                     // VkClearColorValue              color
		};

		std::vector<SwapchainImage> const& swap_chain_images = m_swapchain.getImages();

		for (size_t i = 0; i < m_graphicsCmdBuffers.size(); ++i) {
			m_graphicsCmdBuffers[i].begin(graphics_commandd_buffer_begin_info);

			if (m_presentQueue() != m_graphicsQueue()) {
				VkImageMemoryBarrier barrier_from_present_to_draw = {
					VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,     // VkStructureType                sType
					nullptr,                                    // const void                    *pNext
					VK_ACCESS_MEMORY_READ_BIT,                  // VkAccessFlags                  srcAccessMask
					VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,       // VkAccessFlags                  dstAccessMask
					VK_IMAGE_LAYOUT_UNDEFINED,                  // VkImageLayout                  oldLayout
					VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,            // VkImageLayout                  newLayout
					m_presentQueue.index(),                     // uint32_t                       srcQueueFamilyIndex
					m_graphicsQueue.index(),                    // uint32_t                       dstQueueFamilyIndex
					swap_chain_images[i].image,                 // VkImage                        image
					image_subresource_range                     // VkImageSubresourceRange        subresourceRange
				};

				m_graphicsCmdBuffers[i].pipelineBarrier(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier_from_present_to_draw);
			}

			VkRenderPassBeginInfo render_pass_begin_info = {
				VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,     // VkStructureType                sType
				nullptr,                                      // const void                    *pNext
				m_renderPass(),                               // VkRenderPass                   renderPass
				m_framebuffers[i](),                          // VkFramebuffer                  framebuffer
				{                                             // VkRect2D                       renderArea
					{                                           // VkOffset2D                     offset
						0,                                          // int32_t                        x
						0                                           // int32_t                        y
					},
					{                                           // VkExtent2D                     extent
						m_swapchainSize.x,                          // int32_t                        width
						m_swapchainSize.y,                          // int32_t                        height
					}
				},
				1,                                            // uint32_t                       clearValueCount
				&clear_value                                  // const VkClearValue            *pClearValues
			};

			m_graphicsCmdBuffers[i].beginRenderPass(render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

			m_graphicsCmdBuffers[i].bindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicPipeline());

			m_graphicsCmdBuffers[i].draw(3, 1, 0, 0);

			m_graphicsCmdBuffers[i].endRenderPass();

			if (m_presentQueue() != m_graphicsQueue()) {
				VkImageMemoryBarrier barrier_from_draw_to_present = {
					VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,       // VkStructureType              sType
					nullptr,                                      // const void                  *pNext
					VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,         // VkAccessFlags                srcAccessMask
					VK_ACCESS_MEMORY_READ_BIT,                    // VkAccessFlags                dstAccessMask
					VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,              // VkImageLayout                oldLayout
					VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,              // VkImageLayout                newLayout
					m_graphicsQueue.index(),                      // uint32_t                     srcQueueFamilyIndex
					m_presentQueue.index(),                       // uint32_t                     dstQueueFamilyIndex
					swap_chain_images[i].image,                   // VkImage                      image
					image_subresource_range                       // VkImageSubresourceRange      subresourceRange
				};

				m_graphicsCmdBuffers[i].pipelineBarrier(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier_from_draw_to_present);
			}

			if (!m_graphicsCmdBuffers[i].end()) {
				std::cerr << "Could not record command buffer !" << std::endl;
				return false;
			}
		}
		return true;
	}

	bool RenderWindow::createRenderPass() {
		VkAttachmentDescription attachmentDescriptions[] = {
			{
				0,                                   // VkAttachmentDescriptionFlags   flags
				m_swapchain.getFormat(),             // VkFormat                       format
				VK_SAMPLE_COUNT_1_BIT,               // VkSampleCountFlagBits          samples
				VK_ATTACHMENT_LOAD_OP_CLEAR,         // VkAttachmentLoadOp             loadOp
				VK_ATTACHMENT_STORE_OP_STORE,        // VkAttachmentStoreOp            storeOp
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,     // VkAttachmentLoadOp             stencilLoadOp
				VK_ATTACHMENT_STORE_OP_DONT_CARE,    // VkAttachmentStoreOp            stencilStoreOp
				VK_IMAGE_LAYOUT_UNDEFINED,           // VkImageLayout                  initialLayout;
				VK_IMAGE_LAYOUT_PRESENT_SRC_KHR      // VkImageLayout                  finalLayout
			}
		};

		VkAttachmentReference colorAttachmentReferences[] = {
			{
				0,                                          // uint32_t                       attachment
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL    // VkImageLayout                  layout
			}
		};

		VkSubpassDescription subpassDescriptions[] = {
			{
				0,                                          // VkSubpassDescriptionFlags      flags
				VK_PIPELINE_BIND_POINT_GRAPHICS,            // VkPipelineBindPoint            pipelineBindPoint
				0,                                          // uint32_t                       inputAttachmentCount
				nullptr,                                    // const VkAttachmentReference   *pInputAttachments
				1,                                          // uint32_t                       colorAttachmentCount
				colorAttachmentReferences,                  // const VkAttachmentReference   *pColorAttachments
				nullptr,                                    // const VkAttachmentReference   *pResolveAttachments
				nullptr,                                    // const VkAttachmentReference   *pDepthStencilAttachment
				0,                                          // uint32_t                       preserveAttachmentCount
				nullptr                                     // const uint32_t*                pPreserveAttachments
			}
		};

		VkRenderPassCreateInfo renderPassCreateInfo = {
			VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,    // VkStructureType                sType
			nullptr,                                      // const void                    *pNext
			0,                                            // VkRenderPassCreateFlags        flags
			1,                                            // uint32_t                       attachmentCount
			attachmentDescriptions,                       // const VkAttachmentDescription *pAttachments
			1,                                            // uint32_t                       subpassCount
			subpassDescriptions,                          // const VkSubpassDescription    *pSubpasses
			0,                                            // uint32_t                       dependencyCount
			nullptr                                       // const VkSubpassDependency     *pDependencies
		};

		if (!m_renderPass.create(*m_vulkanInstance.getDevice(), renderPassCreateInfo)) {
			std::cerr << "Could not create render pass !" << std::endl;
			return false;
		}

		return true;

	}

	bool RenderWindow::createFrameBuffers() {
		std::vector<SwapchainImage> const& swapchainImages = m_swapchain.getImages();
		m_framebuffers.resize(swapchainImages.size());

		for (size_t i = 0; i < swapchainImages.size(); ++i) {

			VkFramebufferCreateInfo framebufferCreateInfo = {
				VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,  // VkStructureType                sType
				nullptr,                                    // const void                    *pNext
				0,                                          // VkFramebufferCreateFlags       flags
				m_renderPass(),                             // VkRenderPass                   renderPass
				1,                                          // uint32_t                       attachmentCount
				&swapchainImages[i].view(),                 // const VkImageView             *pAttachments
				m_swapchainSize.x,                          // uint32_t                       width
				m_swapchainSize.y,                          // uint32_t                       height
				1                                           // uint32_t                       layers
			};

			m_framebuffers[i].create(*m_vulkanInstance.getDevice(), framebufferCreateInfo);
		}

		return true;
	}

	bool RenderWindow::createPipeline() {
		ShaderModule vertexShaderModule = createShaderModule("vert.spv");
		ShaderModule fragmentShaderModule = createShaderModule("frag.spv");

		if (!vertexShaderModule.isValid() || !fragmentShaderModule.isValid()) {
			return false;
		}

		std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos = {
			// Vertex shader
			{
			  VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,        // VkStructureType                                sType
			  nullptr,                                                    // const void                                    *pNext
			  0,                                                          // VkPipelineShaderStageCreateFlags               flags
			  VK_SHADER_STAGE_VERTEX_BIT,                                 // VkShaderStageFlagBits                          stage
			  vertexShaderModule(),                                       // VkShaderModule                                 module
			  "main",                                                     // const char                                    *pName
			  nullptr                                                     // const VkSpecializationInfo                    *pSpecializationInfo
			},
			// Fragment shader
			{
			  VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,        // VkStructureType                                sType
			  nullptr,                                                    // const void                                    *pNext
			  0,                                                          // VkPipelineShaderStageCreateFlags               flags
			  VK_SHADER_STAGE_FRAGMENT_BIT,                               // VkShaderStageFlagBits                          stage
			  fragmentShaderModule(),                                     // VkShaderModule                                 module
			  "main",                                                     // const char                                    *pName
			  nullptr                                                     // const VkSpecializationInfo                    *pSpecializationInfo
			}
		};

		VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {
			VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,    // VkStructureType                                sType
			nullptr,                                                      // const void                                    *pNext
			0,                                                            // VkPipelineVertexInputStateCreateFlags          flags;
			0,                                                            // uint32_t                                       vertexBindingDescriptionCount
			nullptr,                                                      // const VkVertexInputBindingDescription         *pVertexBindingDescriptions
			0,                                                            // uint32_t                                       vertexAttributeDescriptionCount
			nullptr                                                       // const VkVertexInputAttributeDescription       *pVertexAttributeDescriptions
		};

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = {
			VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,  // VkStructureType                                sType
			nullptr,                                                      // const void                                    *pNext
			0,                                                            // VkPipelineInputAssemblyStateCreateFlags        flags
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,                          // VkPrimitiveTopology                            topology
			VK_FALSE                                                      // VkBool32                                       primitiveRestartEnable
		};

		VkViewport viewport = {
			0.0f,                                                         // float                                          x
			0.0f,                                                         // float                                          y
			static_cast<float>(m_swapchainSize.x),                        // float                                          width
			static_cast<float>(m_swapchainSize.y),                        // float                                          height
			0.0f,                                                         // float                                          minDepth
			1.0f                                                          // float                                          maxDepth
		};

		VkRect2D scissor = {
			{                                                             // VkOffset2D                                     offset
				0,                                                            // int32_t                                        x
				0                                                             // int32_t                                        y
			},
			{                                                             // VkExtent2D                                     extent
				m_swapchainSize.x,                                            // int32_t                                        width
				m_swapchainSize.y                                             // int32_t                                        height
			}
		};

		VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {
			VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,        // VkStructureType                                sType
			nullptr,                                                      // const void                                    *pNext
			0,                                                            // VkPipelineViewportStateCreateFlags             flags
			1,                                                            // uint32_t                                       viewportCount
			&viewport,                                                    // const VkViewport                              *pViewports
			1,                                                            // uint32_t                                       scissorCount
			&scissor                                                      // const VkRect2D                                *pScissors
		};

		VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {
			VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,   // VkStructureType                                sType
			nullptr,                                                      // const void                                    *pNext
			0,                                                            // VkPipelineRasterizationStateCreateFlags        flags
			VK_FALSE,                                                     // VkBool32                                       depthClampEnable
			VK_FALSE,                                                     // VkBool32                                       rasterizerDiscardEnable
			VK_POLYGON_MODE_FILL,                                         // VkPolygonMode                                  polygonMode
			VK_CULL_MODE_BACK_BIT,                                        // VkCullModeFlags                                cullMode
			VK_FRONT_FACE_COUNTER_CLOCKWISE,                              // VkFrontFace                                    frontFace
			VK_FALSE,                                                     // VkBool32                                       depthBiasEnable
			0.0f,                                                         // float                                          depthBiasConstantFactor
			0.0f,                                                         // float                                          depthBiasClamp
			0.0f,                                                         // float                                          depthBiasSlopeFactor
			1.0f                                                          // float                                          lineWidth
		};

		VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = {
			VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,     // VkStructureType                                sType
			nullptr,                                                      // const void                                    *pNext
			0,                                                            // VkPipelineMultisampleStateCreateFlags          flags
			VK_SAMPLE_COUNT_1_BIT,                                        // VkSampleCountFlagBits                          rasterizationSamples
			VK_FALSE,                                                     // VkBool32                                       sampleShadingEnable
			1.0f,                                                         // float                                          minSampleShading
			nullptr,                                                      // const VkSampleMask                            *pSampleMask
			VK_FALSE,                                                     // VkBool32                                       alphaToCoverageEnable
			VK_FALSE                                                      // VkBool32                                       alphaToOneEnable
		};

		VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {
			VK_FALSE,                                                     // VkBool32                                       blendEnable
			VK_BLEND_FACTOR_ONE,                                          // VkBlendFactor                                  srcColorBlendFactor
			VK_BLEND_FACTOR_ZERO,                                         // VkBlendFactor                                  dstColorBlendFactor
			VK_BLEND_OP_ADD,                                              // VkBlendOp                                      colorBlendOp
			VK_BLEND_FACTOR_ONE,                                          // VkBlendFactor                                  srcAlphaBlendFactor
			VK_BLEND_FACTOR_ZERO,                                         // VkBlendFactor                                  dstAlphaBlendFactor
			VK_BLEND_OP_ADD,                                              // VkBlendOp                                      alphaBlendOp
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |         // VkColorComponentFlags                          colorWriteMask
			VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
		};

		VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = {
			VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,     // VkStructureType                                sType
			nullptr,                                                      // const void                                    *pNext
			0,                                                            // VkPipelineColorBlendStateCreateFlags           flags
			VK_FALSE,                                                     // VkBool32                                       logicOpEnable
			VK_LOGIC_OP_COPY,                                             // VkLogicOp                                      logicOp
			1,                                                            // uint32_t                                       attachmentCount
			&colorBlendAttachmentState,                                // const VkPipelineColorBlendAttachmentState     *pAttachments
			{ 0.0f, 0.0f, 0.0f, 0.0f }                                    // float                                          blendConstants[4]
		};

		PipelineLayout pipelineLayout = createPipelineLayout();
		if (!pipelineLayout.isValid()) {
			return false;
		}

		VkGraphicsPipelineCreateInfo pipelineCreateInfo = {
			VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,              // VkStructureType                                sType
			nullptr,                                                      // const void                                    *pNext
			0,                                                            // VkPipelineCreateFlags                          flags
			static_cast<uint32_t>(shaderStageCreateInfos.size()),         // uint32_t                                       stageCount
			& shaderStageCreateInfos[0],                                  // const VkPipelineShaderStageCreateInfo         *pStages
			&vertexInputStateCreateInfo,                                  // const VkPipelineVertexInputStateCreateInfo    *pVertexInputState;
			&inputAssemblyStateCreateInfo,                                // const VkPipelineInputAssemblyStateCreateInfo  *pInputAssemblyState
			nullptr,                                                      // const VkPipelineTessellationStateCreateInfo   *pTessellationState
			&viewportStateCreateInfo,                                     // const VkPipelineViewportStateCreateInfo       *pViewportState
			&rasterizationStateCreateInfo,                                // const VkPipelineRasterizationStateCreateInfo  *pRasterizationState
			&multisampleStateCreateInfo,                                  // const VkPipelineMultisampleStateCreateInfo    *pMultisampleState
			nullptr,                                                      // const VkPipelineDepthStencilStateCreateInfo   *pDepthStencilState
			&colorBlendStateCreateInfo,                               // const VkPipelineColorBlendStateCreateInfo     *pColorBlendState
			nullptr,                                                      // const VkPipelineDynamicStateCreateInfo        *pDynamicState
			pipelineLayout(),                                             // VkPipelineLayout                               layout
			m_renderPass(),                                               // VkRenderPass                                   renderPass
			0,                                                            // uint32_t                                       subpass
			VK_NULL_HANDLE,                                               // VkPipeline                                     basePipelineHandle
			-1                                                            // int32_t                                        basePipelineIndex
		};

		if (!m_graphicPipeline.createGraphics(*m_vulkanInstance.getDevice(), VK_NULL_HANDLE, pipelineCreateInfo)) {
			std::cerr << "Error: Could not create graphics pipeline !" << std::endl;
			return false;
		}

		return true;
	}

	void RenderWindow::onWindowSizeChanged() {
		m_vulkanInstance.getDevice()->waitIdle();

		m_graphicsCmdBuffers.clear();
		m_graphicCommandPool.destroy();
		m_framebuffers.clear();
		m_graphicPipeline.destroy();
		m_renderPass.destroy();

		if (!createSwapchain()) {
			std::cerr << "Error: Can't re-create swapchain" << std::endl;
		}

		if (!createRenderPass()) {
			std::cerr << "Error: Can't re-create renderpass" << std::endl;
		}

		if (!createFrameBuffers()) {
			std::cerr << "Error: Can't re-create framebuffers" << std::endl;
		}

		if (!createPipeline()) {
			std::cerr << "Can't re-create pipeline" << std::endl;
		}

		if (!createCommandBuffers()) {
			std::cerr << "Error: Can't re-create command buffers" << std::endl;
		}
	}

	VkSurfaceFormatKHR RenderWindow::getSwapchainFormat(std::vector<VkSurfaceFormatKHR> const& surfaceFormats) const {
		if ((surfaceFormats.size() == 1) && (surfaceFormats[0].format == VK_FORMAT_UNDEFINED)) {
			return{ VK_FORMAT_R8G8B8A8_UNORM, VK_COLORSPACE_SRGB_NONLINEAR_KHR };
		}

		for (VkSurfaceFormatKHR const& surfaceFormat : surfaceFormats) {
			if (surfaceFormat.format == VK_FORMAT_R8G8B8A8_UNORM) {
				return surfaceFormat;
			}
		}

		return surfaceFormats[0];
	}

	uint32_t RenderWindow::getSwapchainNumImages(VkSurfaceCapabilitiesKHR const& capabilities) const {
		uint32_t imageCount = capabilities.minImageCount + 1;
		if ((capabilities.maxImageCount > 0) && (imageCount > capabilities.maxImageCount)) {
			imageCount = capabilities.maxImageCount;
		}

		return imageCount;
	}

	VkExtent2D RenderWindow::getSwapchainExtent(VkSurfaceCapabilitiesKHR const& capabilities, Vector2ui const& size) const {
		if (capabilities.currentExtent.width == -1) {
			VkExtent2D swapchainExtent = { size.x, size.y };
			if (swapchainExtent.width < capabilities.minImageExtent.width) {
				swapchainExtent.width = capabilities.minImageExtent.width;
			}
			if (swapchainExtent.height < capabilities.minImageExtent.height) {
				swapchainExtent.height = capabilities.minImageExtent.height;
			}
			if (swapchainExtent.width > capabilities.maxImageExtent.width) {
				swapchainExtent.width = capabilities.maxImageExtent.width;
			}
			if (swapchainExtent.height > capabilities.maxImageExtent.height) {
				swapchainExtent.height = capabilities.maxImageExtent.height;
			}
			return swapchainExtent;
		}

		return capabilities.currentExtent;
	}

	VkImageUsageFlags RenderWindow::getSwapchainUsageFlags(VkSurfaceCapabilitiesKHR const& capabilities) const {
		if (capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
			return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		}
		std::cerr << "Error: VK_IMAGE_USAGE_TRANSFER_DST image usage is not supported by the swap chain!" << std::endl
			<< "Supported swap chain's image usages include:" << std::endl
			<< (capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT ? "    VK_IMAGE_USAGE_TRANSFER_SRC\n" : "")
			<< (capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT ? "    VK_IMAGE_USAGE_TRANSFER_DST\n" : "")
			<< (capabilities.supportedUsageFlags & VK_IMAGE_USAGE_SAMPLED_BIT ? "    VK_IMAGE_USAGE_SAMPLED\n" : "")
			<< (capabilities.supportedUsageFlags & VK_IMAGE_USAGE_STORAGE_BIT ? "    VK_IMAGE_USAGE_STORAGE\n" : "")
			<< (capabilities.supportedUsageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT ? "    VK_IMAGE_USAGE_COLOR_ATTACHMENT\n" : "")
			<< (capabilities.supportedUsageFlags & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT ? "    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT\n" : "")
			<< (capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT ? "    VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT\n" : "")
			<< (capabilities.supportedUsageFlags & VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT ? "    VK_IMAGE_USAGE_INPUT_ATTACHMENT" : "")
			<< std::endl;

		return static_cast<VkImageUsageFlags>(-1);
	}

	VkSurfaceTransformFlagBitsKHR RenderWindow::getSwapchainTransform(VkSurfaceCapabilitiesKHR const& capabilities) const {
		if (capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
			return VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		}
		else {
			return capabilities.currentTransform;
		}
	}

	VkPresentModeKHR RenderWindow::getSwapchainPresentMode(std::vector<VkPresentModeKHR> const& presentModes) const {
		// FIFO present mode is always available
		// MAILBOX is the lowest latency V-Sync enabled mode (something like triple-buffering) so use it if available
		for (VkPresentModeKHR const& presentMode : presentModes) {
			if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return presentMode;
			}
		}
		for (VkPresentModeKHR const& presentMode : presentModes) {
			if (presentMode == VK_PRESENT_MODE_FIFO_KHR) {
				return presentMode;
			}
		}

		std::cerr << "Error: FIFO present mode is not supported by the swapchain" << std::endl;
		return static_cast<VkPresentModeKHR>(-1);
	}

	ShaderModule RenderWindow::createShaderModule(std::string const& filename) const {
		std::vector<char> code{ readBinaryFile(filename) };

		ShaderModule shader;
		if (!shader.create(*m_vulkanInstance.getDevice(), code.size(), reinterpret_cast<const uint32_t*>(&code[0]))) {
			std::cerr << "Can't create shader module" << std::endl;
			return ShaderModule{};
		}

		return shader;
	}

	PipelineLayout RenderWindow::createPipelineLayout() {
		PipelineLayout layout;
		if (!layout.create(*m_vulkanInstance.getDevice(), 0, 0, nullptr, 0, nullptr)) {
			std::cerr << "Error: Could not create pipeline layout !" << std::endl;
			return PipelineLayout{};
		}

		return layout;

	}

}
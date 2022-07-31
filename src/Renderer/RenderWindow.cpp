#include "Renderer/RenderWindow.hpp"

#include "Renderer/Vulkan/ImageView.hpp"
#include "Renderer/Vulkan/PhysicalDevice.hpp"
#include "Renderer/RenderObject.hpp"

#include "Util/Reader.hpp"
#include "Util/Image.hpp"

#include "Math/Matrix4.hpp"

#include <iostream>

namespace Nth {
	RenderWindow::RenderWindow(Vk::VulkanInstance& vulkanInstance) :
		m_vulkan(vulkanInstance),
		m_surface(vulkanInstance.getInstance()),
		m_presentQueue(),
		m_graphicsQueue(),
		m_renderingResources(resourceCount),
		m_swapchainSize(),
		m_resourceIndex(0) { }

	RenderWindow::RenderWindow(Vk::VulkanInstance& vulkanInstance, VideoMode const& mode, const std::string_view title) :
		m_vulkan(vulkanInstance),
		m_surface(vulkanInstance.getInstance()),
		m_presentQueue(),
		m_graphicsQueue(),
		m_renderingResources(resourceCount),
		m_swapchainSize(),
		m_resourceIndex(0) {

		if (!create(mode, title)) {
			throw std::runtime_error("Can't create render window");
		}
	}

	RenderWindow::~RenderWindow() {
		if (m_vulkan.getDevice().isValid()) {
			m_vulkan.getDevice().waitIdle();
		}
		
		m_swapchain.destroy();
	}

	bool RenderWindow::create(VideoMode const& mode, const std::string_view title){
		if (!Window::create(mode, title)) {
			return false;
		}

		if (!m_surface.create(getHandle())) {
			std::cerr << "Error: Can't create surface" << std::endl;
			return false;
		}

		if (!m_vulkan.createDevice(m_surface)) {
			std::cerr << "Error: Can't create device" << std::endl;
			return false;
		}

		Vk::Device& device = m_vulkan.getDevice();
		
		if (!m_presentQueue.create(device, device.getPresentQueueFamilyIndex())) {
			std::cerr << "Error: Can't create present queue" << std::endl;
			return false;
		}

		if (!m_graphicsQueue.create(device, device.getGraphicQueueFamilyIndex())) {
			std::cerr << "Error: Can't create graphics queue" << std::endl;
			return false;
		}

		if (!createSwapchain()) {
			std::cerr << "Error: Can't create swapchain" << std::endl;
			return false;
		}

		if (!createStagingBuffer()) {
			std::cerr << "Can't create stagging buffer" << std::endl;
			return false;
		}

		m_descriptorAllocator.init(m_vulkan.getDevice());

		return true;
	}

	bool RenderWindow::draw(std::vector<RenderObject> const& objects) {
		if (m_swapchainSize != size()) {
			onWindowSizeChanged();
		}
		RenderingResource& currentRenderingResource = m_renderingResources[m_resourceIndex];
		VkSwapchainKHR vkSwapchain = m_swapchain();
	
		m_resourceIndex = (m_resourceIndex + 1) % resourceCount;

		if (!currentRenderingResource.fence.wait(1000000000)) {
			std::cerr << "Waiting for fence takes too long !" << std::endl;
			return false;
		}

		if (!currentRenderingResource.fence.reset()) {
			std::cerr << "Can't reset fence !" << std::endl;
			return false;
		}
		
		uint32_t imageIndex;
		VkResult result = m_swapchain.aquireNextImage(currentRenderingResource.imageAvailableSemaphore(), VK_NULL_HANDLE, imageIndex);
		switch (result) {
		case VK_SUCCESS:
		case VK_SUBOPTIMAL_KHR:
			break;
		case VK_ERROR_OUT_OF_DATE_KHR:
			onWindowSizeChanged();
			return true;
		default:
			std::cerr << "Problem occurred during swap chain image acquisition!" << std::endl;
			return false;
		}

		if (!prepareFrame(currentRenderingResource, m_swapchain.getImages()[imageIndex], objects)) {
			return false;
		}

		VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		VkSubmitInfo submitInfo = {
			VK_STRUCTURE_TYPE_SUBMIT_INFO,                         // VkStructureType              sType
			nullptr,                                               // const void                  *pNext
			1,                                                     // uint32_t                     waitSemaphoreCount
			&currentRenderingResource.imageAvailableSemaphore(),   // const VkSemaphore           *pWaitSemaphores
			&waitDstStageMask,                                     // const VkPipelineStageFlags  *pWaitDstStageMask;
			1,                                                     // uint32_t                     commandBufferCount
			&currentRenderingResource.commandBuffer(),             // const VkCommandBuffer       *pCommandBuffers
			1,                                                     // uint32_t                     signalSemaphoreCount
			&currentRenderingResource.finishedRenderingSemaphore() // const VkSemaphore           *pSignalSemaphores
		};

		if (!m_graphicsQueue.submit(submitInfo, currentRenderingResource.fence())) {
			return false;
		}

		VkPresentInfoKHR presentInfo = {
			VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,                        // VkStructureType              sType
			nullptr,                                                   // const void                  *pNext
			1,                                                         // uint32_t                     waitSemaphoreCount
			&currentRenderingResource.finishedRenderingSemaphore(),    // const VkSemaphore           *pWaitSemaphores
			1,                                                         // uint32_t                     swapchainCount
			&vkSwapchain,                                              // const VkSwapchainKHR        *pSwapchains
			&imageIndex,                                               // const uint32_t              *pImageIndices
			nullptr                                                    // VkResult                    *pResults
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
			std::cerr << "Problem occurred during image presentation!" << std::endl;
			return false;
		}

		return true;
	}

	bool RenderWindow::createMesh(Mesh& mesh) {
		if (!mesh.vertexBuffer.create(m_vulkan.getDevice(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, static_cast<uint32_t>(mesh.vertices.size() * sizeof(mesh.vertices[0])))) {
			std::cerr << "Could not create a vertex buffer!" << std::endl;
			return false;
		}

		if (!copyBufferByStaging(mesh.vertexBuffer, m_stagingBuffer, [this, &mesh](void* mappedPtr) {
			memcpy(mappedPtr, mesh.vertices.data(), mesh.vertexBuffer.handle.getSize());
		})) {
			return false;
		}

		if (!mesh.indexBuffer.create(m_vulkan.getDevice(), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, sizeof(mesh.indices[0]) * mesh.indices.size())) {
			std::cerr << "Can't create index buffer" << std::endl;
			return false;
		}

		if (!copyBufferByStaging(mesh.indexBuffer, m_stagingBuffer, [this, &mesh](void* mappedPtr) {
			memcpy(mappedPtr, mesh.indices.data(), mesh.indexBuffer.handle.getSize());
		})) {
			return false;
		}

		return true;
	}

	bool RenderWindow::setDescriptorSetLayouts(Vk::DescriptorSetLayout& descriptorLayout, Vk::DescriptorSetLayout& ssboDescriptorLayout) {
		m_descriptorLayout = &descriptorLayout;
		m_ssboDescriptorLayout = &ssboDescriptorLayout;

		if (!createRenderingResources()) {
			std::cerr << "Can't create rendering ressources" << std::endl;
			return false;
		}

		if (!allocateDescriptorSet()) {
			std::cerr << "Can't allocate descriptor set" << std::endl;
			return false;
		}

		if (!createTexture()) {
			std::cerr << "Can't create texture" << std::endl;
			return false;
		}

		if (!createDepthRessource()) {
			std::cerr << "Can't create depth ressource" << std::endl;
			return false;
		}

		if (!createSSBO()) {
			std::cerr << "Can't create ssbo" << std::endl;
			return false;
		}

		if (!createUniformBuffer()) {
			std::cerr << "Can't create uniform buffer" << std::endl;
			return false;
		}

		if (!updateDescriptorSet()) {
			std::cerr << "Can't update descriptor set" << std::endl;
			return false;
		}

		if (!createRenderPass()) {
			std::cerr << "Can't create render pass" << std::endl;
			return false;
		}

		return true;
	}

	Vk::RenderPass& RenderWindow::getRenderPass() {
		return m_renderPass;
	}

	bool RenderWindow::createSwapchain() {
		VkSurfaceCapabilitiesKHR surfaceCapabilities;
		if (!m_surface.getCapabilities(m_vulkan.getDevice().getPhysicalDevice(), surfaceCapabilities)) {
			std::cerr << "Error: Can't get surface capabilities" << std::endl;
			return false;
		}

		uint32_t imageCount{ getSwapchainNumImages(surfaceCapabilities) };
		VkImageUsageFlags swapchainUsageFlag{ getSwapchainUsageFlags(surfaceCapabilities) };
		VkSurfaceTransformFlagBitsKHR swapchainTransform{ getSwapchainTransform(surfaceCapabilities) };
		VkExtent2D swapchainExtend{ getSwapchainExtent(surfaceCapabilities, size()) };

		std::vector<VkSurfaceFormatKHR> surfaceFormats;
		if (!m_surface.getFormats(m_vulkan.getDevice().getPhysicalDevice(), surfaceFormats)) {
			std::cerr << "Error: Can't get surface formats" << std::endl;
			return false;
		}

		VkSurfaceFormatKHR surfaceFormat{ getSwapchainFormat(surfaceFormats) };

		std::vector<VkPresentModeKHR> presentModes;
		if (!m_surface.getPresentModes(m_vulkan.getDevice().getPhysicalDevice(), presentModes)) {
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

		Vk::Swapchain newSwapchain;
		if (!newSwapchain.create(m_vulkan.getDevice(), swapchainCreateInfo)) {
			std::cerr << "Error: Can't create swapchain" << std::endl;
			return false;
		}

		m_swapchain = std::move(newSwapchain);
		m_swapchainSize = size();

		return true;
	}

	bool RenderWindow::createRenderPass() {
		std::vector<VkAttachmentDescription> attachmentDescriptions = {
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
			},
			{
				0,                                                // VkAttachmentDescriptionFlags   flags
				findDepthFormat(),                                // VkFormat                       format
				VK_SAMPLE_COUNT_1_BIT,                            // VkSampleCountFlagBits          samples
				VK_ATTACHMENT_LOAD_OP_CLEAR,                      // VkAttachmentLoadOp             loadOp
				VK_ATTACHMENT_STORE_OP_DONT_CARE,                 // VkAttachmentStoreOp            storeOp
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,                  // VkAttachmentLoadOp             stencilLoadOp
				VK_ATTACHMENT_STORE_OP_DONT_CARE,                 // VkAttachmentStoreOp            stencilStoreOp
				VK_IMAGE_LAYOUT_UNDEFINED,                        // VkImageLayout                  initialLayout;
				VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL  // VkImageLayout                  finalLayout
			}
		};

		VkAttachmentReference colorAttachmentReferences = {
			0,                                          // uint32_t                       attachment
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL    // VkImageLayout                  layout
		};

		VkAttachmentReference depthAttachmentRef = {
			1,                                               // uint32_t                       attachment
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL // VkImageLayout                  layout
		};

		VkSubpassDescription subpassDescriptions[] = {
			{
				0,                                          // VkSubpassDescriptionFlags      flags
				VK_PIPELINE_BIND_POINT_GRAPHICS,            // VkPipelineBindPoint            pipelineBindPoint
				0,                                          // uint32_t                       inputAttachmentCount
				nullptr,                                    // const VkAttachmentReference   *pInputAttachments
				1,                                          // uint32_t                       colorAttachmentCount
				&colorAttachmentReferences,                 // const VkAttachmentReference   *pColorAttachments
				nullptr,                                    // const VkAttachmentReference   *pResolveAttachments
				&depthAttachmentRef,                        // const VkAttachmentReference   *pDepthStencilAttachment
				0,                                          // uint32_t                       preserveAttachmentCount
				nullptr                                     // const uint32_t*                pPreserveAttachments
			}
		};

		std::vector<VkSubpassDependency> dependencies = {
			{
				VK_SUBPASS_EXTERNAL,                            // uint32_t                       srcSubpass
				0,                                              // uint32_t                       dstSubpass
				VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,           // VkPipelineStageFlags           srcStageMask
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,  // VkPipelineStageFlags           dstStageMask
				VK_ACCESS_MEMORY_READ_BIT,                      // VkAccessFlags                  srcAccessMask
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,           // VkAccessFlags                  dstAccessMask
				VK_DEPENDENCY_BY_REGION_BIT                     // VkDependencyFlags              dependencyFlags
			},
			{
				VK_SUBPASS_EXTERNAL,                                                                     // uint32_t                       srcSubpass
				0,                                                                                       // uint32_t                       dstSubpass
				VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,  // VkPipelineStageFlags           srcStageMask
				VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,  // VkPipelineStageFlags           dstStageMask
				0,                                                                                       // VkAccessFlags                  srcAccessMask
				VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,                                            // VkAccessFlags                  dstAccessMask
				VK_DEPENDENCY_BY_REGION_BIT                                                              // VkDependencyFlags              dependencyFlags
			},
			{
				0,                                              // uint32_t                       srcSubpass
				VK_SUBPASS_EXTERNAL,                            // uint32_t                       dstSubpass
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,  // VkPipelineStageFlags           srcStageMask
				VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,           // VkPipelineStageFlags           dstStageMask
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,           // VkAccessFlags                  srcAccessMask
				VK_ACCESS_MEMORY_READ_BIT,                      // VkAccessFlags                  dstAccessMask
				VK_DEPENDENCY_BY_REGION_BIT                     // VkDependencyFlags              dependencyFlags
			}
		};

		VkRenderPassCreateInfo renderPassCreateInfo = {
			VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,            // VkStructureType                sType
			nullptr,                                              // const void                    *pNext
			0,                                                    // VkRenderPassCreateFlags        flags
			static_cast<uint32_t>(attachmentDescriptions.size()), // uint32_t                       attachmentCount
			attachmentDescriptions.data(),                        // const VkAttachmentDescription *pAttachments
			1,                                                    // uint32_t                       subpassCount
			subpassDescriptions,                                  // const VkSubpassDescription    *pSubpasses
			static_cast<uint32_t>(dependencies.size()),           // uint32_t                       dependencyCount
			dependencies.data()                                   // const VkSubpassDependency     *pDependencies
		};

		if (!m_renderPass.create(m_vulkan.getDevice(), renderPassCreateInfo)) {
			std::cerr << "Could not create render pass !" << std::endl;
			return false;
		}

		return true;

	}

	bool RenderWindow::createStagingBuffer() {
		if (!m_stagingBuffer.create(m_vulkan.getDevice(),  VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, 5000000)) {
			std::cerr << "Could not staging buffer!" << std::endl;
			return false;
		}

		return true;

	}

	bool RenderWindow::createRenderingResources() {
		for (size_t i = 0; i < m_renderingResources.size(); ++i) {
			if (!m_renderingResources[i].create(m_vulkan.getDevice(), m_presentQueue.index())) {
				std::cerr << "Can't create rendering ressource" << std::endl;
				return false;
			}

			m_renderingResources[i].ssboDescriptor = m_descriptorAllocator.allocate(*m_ssboDescriptorLayout);
		}

		return true;
	}

	bool RenderWindow::createTexture() {
		Image image = Image::loadFromFile("viking_room.png", PixelChannel::Rgba);
		
		std::vector<char> const& pixels = image.pixels();
		if (pixels.empty()) {
			return false;
		}

		if (!m_image.create(
			m_vulkan.getDevice(),
			image.width(),
			image.height(),
			VK_FORMAT_R8G8B8A8_UNORM,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		)) {
			return false;
		}

		if (!m_image.createView(m_vulkan.getDevice(), VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT)) {
			return false;
		}

		if (!copyTextureData(pixels.data(), static_cast<uint32_t>(pixels.size()), image.width(), image.height())) {
			return false;
		}

		return true;
	}

	bool RenderWindow::copyTextureData(char const* textureData, uint32_t dataSize, uint32_t width, uint32_t height) {
		if (!m_stagingBuffer.memory.map(0, dataSize, 0)) {
			std::cerr << "Could not map memory and upload texture data to a staging buffer!" << std::endl;
			return false;
		}
		void* stagingBufferMemoryPointer = m_stagingBuffer.memory.getMappedPointer();

		memcpy(stagingBufferMemoryPointer, textureData, dataSize);
	
		m_stagingBuffer.memory.flushMappedMemory(0, dataSize);

		m_stagingBuffer.memory.unmap();

		// Prepare command buffer to copy data from staging buffer to a vertex buffer
		VkCommandBufferBeginInfo commandBufferBeginInfo = {
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,  // VkStructureType                        sType
			nullptr,                                      // const void                            *pNext
			VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,  // VkCommandBufferUsageFlags              flags
			nullptr                                       // const VkCommandBufferInheritanceInfo  *pInheritanceInfo
		};

		Vk::CommandBuffer& commandBuffer = m_renderingResources[0].commandBuffer;

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
			m_image.image.handle(),                 // VkImage                   image
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
		commandBuffer.copyBufferToImage(m_stagingBuffer.handle(), m_image.image.handle(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferImageCopyInfo);

		VkImageMemoryBarrier imageMemoryBarrierFromTransferToShaderRead = {
			VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,   // VkStructureType              sType
			nullptr,                                  // const void                  *pNext
			VK_ACCESS_TRANSFER_WRITE_BIT,             // VkAccessFlags                srcAccessMask
			VK_ACCESS_SHADER_READ_BIT,                // VkAccessFlags                dstAccessMask
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,     // VkImageLayout                oldLayout
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, // VkImageLayout                newLayout
			VK_QUEUE_FAMILY_IGNORED,                  // uint32_t                     srcQueueFamilyIndex
			VK_QUEUE_FAMILY_IGNORED,                  // uint32_t                     dstQueueFamilyIndex
			m_image.image.handle(),                   // VkImage                      image
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

		if (!m_graphicsQueue.submit(submitInfo, VK_NULL_HANDLE)) {
			return false;
		}

		m_vulkan.getDevice().waitIdle();

		return true;
	}

	bool RenderWindow::allocateDescriptorSet() {
		m_descriptor = m_descriptorAllocator.allocate(*m_descriptorLayout);

		return true;
	}

	bool RenderWindow::updateDescriptorSet() {
		VkDescriptorImageInfo imageInfo = {
			m_image.sampler(),                          // VkSampler                      sampler
			m_image.image.view(),                       // VkImageView                    imageView
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL    // VkImageLayout                  imageLayout
		};

		VkDescriptorBufferInfo bufferInfo = {
			m_uniformBuffer.handle(),                // VkBuffer         buffer
			0,                                       // VkDeviceSize     offset
			m_uniformBuffer.handle.getSize()         // VkDeviceSize     range
		};

		std::vector<VkWriteDescriptorSet> descriptorWrites = {
			{
				VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,     // VkStructureType                sType
				nullptr,                                    // const void                    *pNext
				m_descriptor(),                             // VkDescriptorSet                dstSet
				0,                                          // uint32_t                       dstBinding
				0,                                          // uint32_t                       dstArrayElement
				1,                                          // uint32_t                       descriptorCount
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,  // VkDescriptorType               descriptorType
				&imageInfo,                                 // const VkDescriptorImageInfo   *pImageInfo
				nullptr,                                    // const VkDescriptorBufferInfo  *pBufferInfo
				nullptr                                     // const VkBufferView            *pTexelBufferView
			},
			{
				VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,    // VkStructureType     sType
				nullptr,                                   // const void         *pNext
				m_descriptor(),                            // VkDescriptorSet     dstSet
				1,                                         // uint32_t            dstBinding
				0,                                         // uint32_t            dstArrayElement
				1,                                         // uint32_t            descriptorCount
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         // VkDescriptorType    descriptorType
				nullptr,                                   // const VkDescriptorImageInfo  *pImageInfo
				&bufferInfo,                               // const VkDescriptorBufferInfo *pBufferInfo
				nullptr                                    // const VkBufferView *pTexelBufferView
			}
		};

		// TODO: Check if update methode should be in Device class 
		m_descriptor.update(static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data());

		for (size_t i = 0; i < m_renderingResources.size(); ++i) {
			VkDescriptorBufferInfo ssboInfo = {
				m_renderingResources[i].ssbo.handle(),         // VkBuffer         buffer
				0,                                             // VkDeviceSize     offset
				m_renderingResources[i].ssbo.handle.getSize()  // VkDeviceSize     range
			};

			std::vector<VkWriteDescriptorSet> descriptorWrites2 = {
				{
					VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,     // VkStructureType                sType
					nullptr,                                    // const void                    *pNext
					m_renderingResources[i].ssboDescriptor(),   // VkDescriptorSet                dstSet
					0,                                          // uint32_t                       dstBinding
					0,                                          // uint32_t                       dstArrayElement
					1,                                          // uint32_t                       descriptorCount
					VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,          // VkDescriptorType               descriptorType
					nullptr,                                    // const VkDescriptorImageInfo   *pImageInfo
					&ssboInfo,                                  // const VkDescriptorBufferInfo  *pBufferInfo
					nullptr                                     // const VkBufferView            *pTexelBufferView
				},
			};

			m_renderingResources[i].ssboDescriptor.update(static_cast<uint32_t>(descriptorWrites2.size()), descriptorWrites2.data());
		}

		return true;
	}

	bool RenderWindow::createUniformBuffer() {
		if (!m_uniformBuffer.create(m_vulkan.getDevice(), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, sizeof(UniformBufferObject))) {
			std::cerr << "Could not create uniform buffer!" << std::endl;
			return false;
		}

		if (!copyUniformBufferData()) {
			return false;
		}

		return true;
	}

	bool RenderWindow::createSSBO() {
		for (size_t i = 0; i < m_renderingResources.size(); ++i) {
			if (!m_renderingResources[i].ssbo.create(
				m_vulkan.getDevice(),
				VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
				10000 * sizeof(ShaderStorageBufferObject))) {
				std::cerr << "Can't create SSBO" << std::endl;
				return false;
			}
		}

		return true;
	}

	bool RenderWindow::createDepthRessource() {
		VulkanImage newDepth;

		VkFormat depthFormat = findDepthFormat();

		if (!newDepth.create(
			m_vulkan.getDevice(),
			m_swapchainSize.x, 
			m_swapchainSize.y, 
			depthFormat, 
			VK_IMAGE_TILING_OPTIMAL, 
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)) {
			std::cerr << "Can't create depth image" << std::endl;
			return false;
		}

		if (!newDepth.createView(m_vulkan.getDevice(), depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT)) {
			return false;
		}

		m_depth = std::move(newDepth);

		return true;
	}

	void RenderWindow::onWindowSizeChanged() {
		m_vulkan.getDevice().waitIdle();

		if (!createSwapchain()) {
			std::cerr << "Error: Can't re-create swapchain" << std::endl;
		}

		if (!copyUniformBufferData()) {
			std::cerr << "Error: Can't re-copy uniform data" << std::endl;
		}

		if (!createDepthRessource()) {
			std::cerr << "Error: Can't re-create depth ressource" << std::endl;
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
		// MAILBOX is the lowest latency V-Sync enabled mode (somNTHing like triple-buffering) so use it if available
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

	bool RenderWindow::allocateBufferMemory(Vk::Buffer const& buffer, VkMemoryPropertyFlagBits memoryProperty, Vk::DeviceMemory& memory) const {
		VkMemoryRequirements bufferMemoryRequirements = buffer.getMemoryRequirements();;
		VkPhysicalDeviceMemoryProperties memoryProperties = m_vulkan.getDevice().getPhysicalDevice().getMemoryProperties();

		for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i) {
			if ((bufferMemoryRequirements.memoryTypeBits & (1 << i)) &&
				(memoryProperties.memoryTypes[i].propertyFlags & memoryProperty) == memoryProperty) {

				VkMemoryAllocateInfo memoryAllocateInfo = {
					VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,     // VkStructureType                        sType
					nullptr,                                    // const void                            *pNext
					bufferMemoryRequirements.size,              // VkDeviceSize                           allocationSize
					i                                           // uint32_t                               memoryTypeIndex
				};

				if (memory.create(m_vulkan.getDevice(), memoryAllocateInfo)) {
					return true;
				}
			}
		}
		return false;
	}

	bool RenderWindow::prepareFrame(RenderingResource& ressources, Vk::SwapchainImage const& imageParameters, std::vector<RenderObject> const& objects) const {
		ressources.framebuffer.destroy();
		if (!createFramebuffer(ressources.framebuffer, imageParameters)) {
			return false;
		}

		ressources.ssbo.memory.map(0, ressources.ssbo.handle.getSize(), 0);
		void* mappedPtr = ressources.ssbo.memory.getMappedPointer();
		ShaderStorageBufferObject* objectSSBO = (ShaderStorageBufferObject*)mappedPtr;

		for (size_t i = 0; i < objects.size(); ++i) {
			objectSSBO[i].model = objects[i].transformMatrix;
		}

		ressources.ssbo.memory.flushMappedMemory(0, ressources.ssbo.handle.getSize());
		
		ressources.ssbo.memory.unmap();

		VkCommandBufferBeginInfo commandBufferBeginInfo = {
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,        // VkStructureType                        sType
			nullptr,                                            // const void                            *pNext
			VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,        // VkCommandBufferUsageFlags              flags
			nullptr                                             // const VkCommandBufferInheritanceInfo  *pInheritanceInfo
		};

		ressources.commandBuffer.begin(commandBufferBeginInfo);

		VkImageSubresourceRange imageSubresourceRange = {
			VK_IMAGE_ASPECT_COLOR_BIT,                          // VkImageAspectFlags                     aspectMask
			0,                                                  // uint32_t                               baseMipLevel
			1,                                                  // uint32_t                               levelCount
			0,                                                  // uint32_t                               baseArrayLayer
			1                                                   // uint32_t                               layerCount
		};

		if (m_presentQueue() != m_graphicsQueue()) {
			VkImageMemoryBarrier barrierFromPresentToDraw = {
				VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,           // VkStructureType                        sType
				nullptr,                                          // const void                            *pNext
				VK_ACCESS_MEMORY_READ_BIT,                        // VkAccessFlags                          srcAccessMask
				VK_ACCESS_MEMORY_READ_BIT,                        // VkAccessFlags                          dstAccessMask
				VK_IMAGE_LAYOUT_UNDEFINED,                        // VkImageLayout                          oldLayout
				VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,                  // VkImageLayout                          newLayout
				m_presentQueue.index(),                           // uint32_t                               srcQueueFamilyIndex
				m_graphicsQueue.index(),                          // uint32_t                               dstQueueFamilyIndex
				imageParameters.image,                            // VkImage                                image
				imageSubresourceRange                             // VkImageSubresourceRange                subresourceRange
			};

			ressources.commandBuffer.pipelineBarrier(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrierFromPresentToDraw);
		}

		std::vector<VkClearValue> clearValues(2);
		clearValues[0].color = { 1.0f, 0.8f, 0.4f, 0.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };

		VkRenderPassBeginInfo renderPassBeginInfo = {
			VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,           // VkStructureType                        sType
			nullptr,                                            // const void                            *pNext
			m_renderPass(),                                     // VkRenderPass                           renderPass
			ressources.framebuffer(),                           // VkFramebuffer                          framebuffer
			{                                                   // VkRect2D                               renderArea
				{                                                 // VkOffset2D                             offset
					0,                                                // int32_t                                x
					0                                                 // int32_t                                y
				},
				{                                               // VkExtent2D                             extent;
					m_swapchainSize.x,
					m_swapchainSize.y
				}
			},
			static_cast<uint32_t>(clearValues.size()),         // uint32_t                               clearValueCount
			clearValues.data()                                 // const VkClearValue                    *pClearValues
		};

		ressources.commandBuffer.beginRenderPass(renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport = {
			0.0f,                                               // float                                  x
			0.0f,                                               // float                                  y
			static_cast<float>(m_swapchainSize.x),              // float                                  width
			static_cast<float>(m_swapchainSize.y),              // float                                  height
			0.0f,                                               // float                                  minDepth
			1.0f                                                // float                                  maxDepth
		};

		VkRect2D scissor = {
			{                                                   // VkOffset2D                             offset
				0,                                                  // int32_t                                x
				0                                                   // int32_t                                y
			},
			{                                                   // VkExtent2D                             extent
				m_swapchainSize.x,                                  // uint32_t                               width
				m_swapchainSize.y                                   // uint32_t                               height
			}
		};

		ressources.commandBuffer.setViewport(viewport);
		ressources.commandBuffer.setScissor(scissor);

		Mesh* lastMesh = nullptr;
		Material* lastMaterial = nullptr;
		for (size_t i = 0; i < objects.size(); ++i) {
			if (objects[i].material != lastMaterial) {
				ressources.commandBuffer.bindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, objects[i].material->pipeline());

				VkDescriptorSet vkDescriptorSet = m_descriptor();
				ressources.commandBuffer.bindDescriptorSets(objects[i].material->pipelineLayout(), 0, 1, &vkDescriptorSet, 0, nullptr);

				VkDescriptorSet vkSsboDescriptorSet = ressources.ssboDescriptor();
				ressources.commandBuffer.bindDescriptorSets(objects[i].material->pipelineLayout(), 1, 1, &vkSsboDescriptorSet, 0, nullptr);

				lastMaterial = objects[i].material;
			}

			if (objects[i].mesh != lastMesh) {
				VkDeviceSize offset = 0;
				ressources.commandBuffer.bindVertexBuffer(objects[i].mesh->vertexBuffer.handle(), offset);

				ressources.commandBuffer.bindIndexBuffer(objects[i].mesh->indexBuffer.handle(), 0, VK_INDEX_TYPE_UINT32);

				lastMesh = objects[i].mesh;
			}

			ressources.commandBuffer.drawIndexed(static_cast<uint32_t>(objects[i].mesh->indices.size()), 1, 0, 0, 0);
		}

		ressources.commandBuffer.endRenderPass();

		if (m_presentQueue() != m_graphicsQueue()) {
			VkImageMemoryBarrier barrierFromDrawToPresent = {
				VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,           // VkStructureType                        sType
				nullptr,                                          // const void                            *pNext
				VK_ACCESS_MEMORY_READ_BIT,                        // VkAccessFlags                          srcAccessMask
				VK_ACCESS_MEMORY_READ_BIT,                        // VkAccessFlags                          dstAccessMask
				VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,                  // VkImageLayout                          oldLayout
				VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,                  // VkImageLayout                          newLayout
				m_graphicsQueue.index(),                          // uint32_t                               srcQueueFamilyIndex
				m_presentQueue.index(),                           // uint32_t                               dstQueueFamilyIndex
				imageParameters.image,                            // VkImage                                image
				imageSubresourceRange                             // VkImageSubresourceRange                subresourceRange
			};
			ressources.commandBuffer.pipelineBarrier(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrierFromDrawToPresent);
		}

		if (!ressources.commandBuffer.end()) {
			std::cerr << "Could not record command buffer !" << std::endl;
			return false;
		}

		return true;
	}

	bool RenderWindow::createFramebuffer(Vk::Framebuffer& framebuffer, Vk::SwapchainImage const& swapchainImage) const {
		std::vector<VkImageView> attachements{
			swapchainImage.view(),
			m_depth.view()
		};

		VkFramebufferCreateInfo framebufferCreateInfo = {
		VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,  // VkStructureType                sType
			nullptr,                                    // const void                    *pNext
			0,                                          // VkFramebufferCreateFlags       flags
			m_renderPass(),                             // VkRenderPass                   renderPass
			static_cast<uint32_t>(attachements.size()), // uint32_t                       attachmentCount
			attachements.data(),                        // const VkImageView             *pAttachments
			m_swapchainSize.x,                          // uint32_t                       width
			m_swapchainSize.y,                          // uint32_t                       height
			1                                           // uint32_t                       layers
		};

		return framebuffer.create(m_vulkan.getDevice(), framebufferCreateInfo);
	}

	bool RenderWindow::copyUniformBufferData() {
		const UniformBufferObject uniformData = getUniformBufferData();

		return copyBufferByStaging(m_uniformBuffer, m_stagingBuffer, [this , &uniformData](void* mappedPtr) {
			memcpy(mappedPtr, &uniformData, m_uniformBuffer.handle.getSize());
		});
	}

	bool RenderWindow::copyBufferByStaging(VulkanBuffer& target, VulkanBuffer& staging, std::function<void(void*)> copyFunction) {
		if (!staging.memory.map(0, target.handle.getSize(), 0)) {
			std::cerr << "Could not map memory and upload data to a staging buffer!" << std::endl;
			return false;
		}

		void* stagingBufferMemoryPointer = m_stagingBuffer.memory.getMappedPointer();
		
		copyFunction(stagingBufferMemoryPointer);

		staging.memory.flushMappedMemory(0, target.handle.getSize());

		staging.memory.unmap();

		// Prepare command buffer to copy data from staging buffer to a uniform buffer
		Vk::CommandBuffer& commandBuffer = m_renderingResources[0].commandBuffer;

		VkCommandBufferBeginInfo commandBufferBeginInfo = {
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, // VkStructureType              sType
			nullptr,                                     // const void                  *pNext
			VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, // VkCommandBufferUsageFlags    flags
			nullptr                                      // const VkCommandBufferInheritanceInfo  *pInheritanceInfo
		};

		commandBuffer.begin(commandBufferBeginInfo);

		VkBufferCopy bufferCopyInfo = {
			0,                                // VkDeviceSize       srcOffset
			0,                                // VkDeviceSize       dstOffset
			target.handle.getSize()  // VkDeviceSize       size
		};
		commandBuffer.copyBuffer(staging.handle(), target.handle(), bufferCopyInfo);

		VkBufferMemoryBarrier buffer_memory_barrier = {
			VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER, // VkStructureType    sType;
			nullptr,                          // const void        *pNext
			VK_ACCESS_TRANSFER_WRITE_BIT,     // VkAccessFlags      srcAccessMask
			VK_ACCESS_UNIFORM_READ_BIT,       // VkAccessFlags      dstAccessMask
			VK_QUEUE_FAMILY_IGNORED,          // uint32_t           srcQueueFamilyIndex
			VK_QUEUE_FAMILY_IGNORED,          // uint32_t           dstQueueFamilyIndex
			target.handle(),                  // VkBuffer           buffer
			0,                                // VkDeviceSize       offset
			VK_WHOLE_SIZE                     // VkDeviceSize       size
		};
		commandBuffer.pipelineBarrier(VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, 0, 0, nullptr, 1, &buffer_memory_barrier, 0, nullptr);

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

		if (!m_graphicsQueue.submit(submitInfo, VK_NULL_HANDLE)) {
			return false;
		}

		m_vulkan.getDevice().waitIdle();

		return true;
	}

	UniformBufferObject RenderWindow::getUniformBufferData() const {
		UniformBufferObject ubo {};

		ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.proj = glm::perspective(glm::radians(45.0f), static_cast<float>(m_swapchainSize.x) / static_cast<float>(m_swapchainSize.y), 0.1f, 10.0f);
		ubo.proj[1][1] *= -1;

		return ubo;
	}

	VkFormat RenderWindow::findSupportedFormat(std::vector<VkFormat> const& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const {
		Vk::PhysicalDevice const& physicalDevice = m_vulkan.getDevice().getPhysicalDevice();
		
		for (VkFormat format : candidates) {
			VkFormatProperties props = physicalDevice.getFormatProperties(format);
			
			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
				return format;
			}
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
				return format;
			}
		}

		throw std::runtime_error("Canno't find supported format!");
	}

	VkFormat RenderWindow::findDepthFormat() const {
		return findSupportedFormat(
			{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
		);
	}

	bool RenderWindow::hasStencilComponent(VkFormat format) const {
		return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
	}
}
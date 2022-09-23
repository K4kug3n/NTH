#include <Renderer/RenderWindow.hpp>

#include <Renderer/Vulkan/ImageView.hpp>
#include <Renderer/Vulkan/PhysicalDevice.hpp>
#include <Renderer/RenderObject.hpp>
#include <Renderer/Material.hpp>
#include <Renderer/Mesh.hpp>
#include <Renderer/RenderTexture.hpp>

#include <Util/Reader.hpp>
#include <Util/Image.hpp>

#include <Math/Matrix4.hpp>

#include <iostream>

namespace Nth {
	RenderWindow::RenderWindow(RenderInstance& vulkanInstance) :
		m_vulkan(vulkanInstance),
		m_surface(vulkanInstance.getHandle()),
		m_ressourceIndex(0),
		m_swapchainSize() { }

	RenderWindow::RenderWindow(RenderInstance& vulkanInstance, VideoMode const& mode, const std::string_view title) :
		m_vulkan(vulkanInstance),
		m_surface(vulkanInstance.getHandle()),
		m_ressourceIndex(0),
		m_swapchainSize() {

		if (!create(mode, title)) {
			throw std::runtime_error("Can't create render window");
		}
	}

	RenderWindow::~RenderWindow() {
		if (m_vulkan.getDevice().getHandle().isValid()) {
			m_vulkan.getDevice().getHandle().waitIdle();
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

		if (!createSwapchain()) {
			std::cerr << "Error: Can't create swapchain" << std::endl;
			return false;
		}

		if (!createDepthRessource()) {
			std::cerr << "Can't create depth ressource" << std::endl;
			return false;
		}

		if (!createRenderPass()) {
			std::cerr << "Can't create render pass" << std::endl;
			return false;
		}

		if (!createRenderingResources()) {
			std::cerr << "Can't create rendering ressources" << std::endl;
			return false;
		}

		return true;
	}

	RenderingResource& RenderWindow::aquireNextImage() {
		if (m_swapchainSize != size()) {
			onWindowSizeChanged();
		}

		RenderingResource& ressource = m_renderingResources[m_ressourceIndex];
		m_ressourceIndex = (m_ressourceIndex + 1) % m_renderingResources.size();

		if (!ressource.fence.wait(1000000000)) {
			std::runtime_error("Waiting for fence takes too long !");
		}

		if (!ressource.fence.reset()) {
			std::runtime_error("Can't reset fence !");
		}

		uint32_t imageIndex;
		VkResult result = m_swapchain.aquireNextImage(ressource.imageAvailableSemaphore(), VK_NULL_HANDLE, imageIndex);
		switch (result) {
		case VK_SUCCESS:
		case VK_SUBOPTIMAL_KHR:
			break;
		case VK_ERROR_OUT_OF_DATE_KHR:
			onWindowSizeChanged();
		default:
			throw std::runtime_error("Problem occurred during swap chain image acquisition!");
		}

		Vk::SwapchainImage const& swpachainImage = m_swapchain.getImages()[imageIndex];

		ressource.framebuffer.destroy();
		if (!createFramebuffer(ressource.framebuffer, swpachainImage)) {
			throw std::runtime_error("Can't create framebiuffer");
		}
		ressource.swapchainImage = swpachainImage.image;
		ressource.imageIndex = imageIndex;

		return ressource;
	}

	void RenderWindow::present(uint32_t imageIndex, Vk::Semaphore const& semaphore) {
		VkSwapchainKHR vkSwapchain = m_swapchain();
		VkPresentInfoKHR presentInfo = {
			VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,                        // VkStructureType              sType
			nullptr,                                                   // const void                  *pNext
			1,                                                         // uint32_t                     waitSemaphoreCount
			&semaphore(),                                              // const VkSemaphore           *pWaitSemaphores
			1,                                                         // uint32_t                     swapchainCount
			&vkSwapchain,                                              // const VkSwapchainKHR        *pSwapchains
			&imageIndex,                                               // const uint32_t              *pImageIndices
			nullptr                                                    // VkResult                    *pResults
		};
		VkResult result = m_vulkan.getDevice().presentQueue().present(presentInfo);

		switch (result) {
		case VK_SUCCESS:
			break;
		case VK_ERROR_OUT_OF_DATE_KHR:
		case VK_SUBOPTIMAL_KHR:
			onWindowSizeChanged();
		default:
			throw std::runtime_error("Problem occurred during image presentation!");
		}
	}

	Vk::RenderPass& RenderWindow::getRenderPass() {
		return m_renderPass;
	}

	RenderDevice const& RenderWindow::getDevice() const {
		return m_vulkan.getDevice();
	}

	bool RenderWindow::createSwapchain() {
		VkSurfaceCapabilitiesKHR surfaceCapabilities;
		if (!m_surface.getCapabilities(m_vulkan.getDevice().getHandle().getPhysicalDevice(), surfaceCapabilities)) {
			std::cerr << "Error: Can't get surface capabilities" << std::endl;
			return false;
		}

		uint32_t imageCount{ getSwapchainNumImages(surfaceCapabilities) };
		VkImageUsageFlags swapchainUsageFlag{ getSwapchainUsageFlags(surfaceCapabilities) };
		VkSurfaceTransformFlagBitsKHR swapchainTransform{ getSwapchainTransform(surfaceCapabilities) };
		VkExtent2D swapchainExtend{ getSwapchainExtent(surfaceCapabilities, size()) };

		std::vector<VkSurfaceFormatKHR> surfaceFormats;
		if (!m_surface.getFormats(m_vulkan.getDevice().getHandle().getPhysicalDevice(), surfaceFormats)) {
			std::cerr << "Error: Can't get surface formats" << std::endl;
			return false;
		}

		VkSurfaceFormatKHR surfaceFormat{ getSwapchainFormat(surfaceFormats) };

		std::vector<VkPresentModeKHR> presentModes;
		if (!m_surface.getPresentModes(m_vulkan.getDevice().getHandle().getPhysicalDevice(), presentModes)) {
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
		if (!newSwapchain.create(m_vulkan.getDevice().getHandle(), swapchainCreateInfo)) {
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
				m_depth.format(),                                 // VkFormat                       format
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

		if (!m_renderPass.create(m_vulkan.getDevice().getHandle(), renderPassCreateInfo)) {
			std::cerr << "Could not create render pass !" << std::endl;
			return false;
		}

		return true;

	}

	bool RenderWindow::createDepthRessource() {
		DepthImage newDepth;

		if (!newDepth.create(m_vulkan.getDevice(), m_swapchainSize)) {
			return false;
		}

		m_depth = std::move(newDepth);

		return true;
	}

	void RenderWindow::onWindowSizeChanged() {
		m_vulkan.getDevice().getHandle().waitIdle();

		if (!createSwapchain()) {
			throw std::runtime_error("Can't re-create swapchain");
		}

		if (!createDepthRessource()) {
			throw std::runtime_error("Can't re-create depth ressource");
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

	bool RenderWindow::createFramebuffer(Vk::Framebuffer& framebuffer, Vk::SwapchainImage const& swapchainImage) const {
		std::vector<VkImageView> attachements{
			swapchainImage.view(),
			m_depth.view()()
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

		return framebuffer.create(m_vulkan.getDevice().getHandle(), framebufferCreateInfo);
	}

	bool RenderWindow::createRenderingResources() {
		m_renderingResources.emplace_back(RenderingResource{ *this });
		m_renderingResources.emplace_back(RenderingResource{ *this });
		m_renderingResources.emplace_back(RenderingResource{ *this });

		for (size_t i = 0; i < m_renderingResources.size(); ++i) {
			if (!m_renderingResources[i].create(m_vulkan.getDevice().graphicsQueue().index())) {
				std::cerr << "Can't create rendering ressource" << std::endl;
				return false;
			}
		}

		return true;
	}
}
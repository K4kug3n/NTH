#include <Renderer/RenderWindow.hpp>

#include <Renderer/Vulkan/ImageView.hpp>
#include <Renderer/Vulkan/PhysicalDevice.hpp>
#include <Renderer/RenderObject.hpp>
#include <Renderer/Material.hpp>
#include <Renderer/Mesh.hpp>
#include <Renderer/RenderTexture.hpp>

#include <Window/WindowHandle.hpp>

#include <Utils/Reader.hpp>
#include <Utils/Image.hpp>

#include <Maths/Matrix4.hpp>

#include <iostream>

namespace Nth {
	RenderWindow::RenderWindow(RenderInstance& vulkan_instance) :
		m_vulkan(vulkan_instance),
		m_surface(vulkan_instance.get_handle()),
		m_ressourceIndex(0),
		m_swapchain_size() { }

	RenderWindow::RenderWindow(RenderInstance& vulkan_instance, const std::string& title) :
		Window(title, 10, 10, 640, 480, 0),
		m_vulkan(vulkan_instance),
		m_surface(vulkan_instance.get_handle()),
		m_ressourceIndex(0),
		m_swapchain_size() {

		create();
	}

	RenderWindow::~RenderWindow() {
		if (m_vulkan.get_device().get_handle().is_valid()) {
			m_vulkan.get_device().get_handle().wait_idle();
		}
		
		m_swapchain.destroy();
	}

	void RenderWindow::create(){
		m_surface.create(handle());

		m_vulkan.create_device(m_surface);

		create_swapchain();

		create_depth_ressource();

		create_render_pass();

		create_rendering_resources();
	}

	RenderingResource& RenderWindow::aquireNextImage() {
		if (m_swapchain_size != size()) {
			on_window_size_changed();
		}

		RenderingResource& ressource = m_rendering_resources[m_ressourceIndex];
		m_ressourceIndex = (m_ressourceIndex + 1) % m_rendering_resources.size();

		ressource.fence.wait(1000000000);

		ressource.fence.reset();

		uint32_t imageIndex;
		VkResult result = m_swapchain.aquire_next_image(ressource.image_available_semaphore(), VK_NULL_HANDLE, imageIndex);
		switch (result) {
		case VK_SUCCESS:
		case VK_SUBOPTIMAL_KHR:
			break;
		case VK_ERROR_OUT_OF_DATE_KHR:
			on_window_size_changed();
		default:
			throw std::runtime_error("Problem occurred during swap chain image acquisition!");
		}

		const Vk::SwapchainImage& swpachain_image = m_swapchain.get_images()[imageIndex];

		ressource.framebuffer.destroy();
		create_framebuffer(ressource.framebuffer, swpachain_image);
		ressource.swapchain_image = swpachain_image.image;
		ressource.image_index = imageIndex;

		return ressource;
	}

	void RenderWindow::present(uint32_t imageIndex, const Vk::Semaphore& semaphore) {
		VkSwapchainKHR vkSwapchain = m_swapchain();
		VkSemaphore vk_semaphore = semaphore();
		VkPresentInfoKHR presentInfo = {
			VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,                        // VkStructureType              sType
			nullptr,                                                   // const void                  *pNext
			1,                                                         // uint32_t                     waitSemaphoreCount
			&vk_semaphore,                                             // const VkSemaphore           *pWaitSemaphores
			1,                                                         // uint32_t                     swapchainCount
			&vkSwapchain,                                              // const VkSwapchainKHR        *pSwapchains
			&imageIndex,                                               // const uint32_t              *pImageIndices
			nullptr                                                    // VkResult                    *pResults
		};
		VkResult result = m_vulkan.get_device().present_queue().present(presentInfo);

		switch (result) {
		case VK_SUCCESS:
			break;
		case VK_ERROR_OUT_OF_DATE_KHR:
		case VK_SUBOPTIMAL_KHR:
			on_window_size_changed();
		default:
			throw std::runtime_error("Problem occurred during image presentation!");
		}
	}

	Vk::RenderPass& RenderWindow::get_render_pass() {
		return m_render_pass;
	}

	const RenderDevice& RenderWindow::get_device() const {
		return m_vulkan.get_device();
	}

	void RenderWindow::create_swapchain() {
		VkSurfaceCapabilitiesKHR surface_capabilities{ m_surface.get_capabilities(m_vulkan.get_device().get_handle().get_physical_device()) };
		uint32_t image_count{ get_swapchain_num_images(surface_capabilities) };
		VkImageUsageFlags swapchain_usage_flag{ get_swapchain_usage_flags(surface_capabilities) };
		VkSurfaceTransformFlagBitsKHR swapchain_transform{ get_swapchain_transform(surface_capabilities) };
		VkExtent2D swapchain_extend{ get_swapchain_extent(surface_capabilities, size()) };
		std::vector<VkSurfaceFormatKHR> surface_formats{ m_surface.get_formats(m_vulkan.get_device().get_handle().get_physical_device()) };
		VkSurfaceFormatKHR surface_format{ get_swapchain_format(surface_formats) };
		std::vector<VkPresentModeKHR> present_modes{ m_surface.get_present_modes(m_vulkan.get_device().get_handle().get_physical_device()) };
		VkPresentModeKHR present_mode{ get_swapchain_present_mode(present_modes) };

		if (static_cast<int>(swapchain_usage_flag) == -1) {
			throw std::runtime_error("Wrong swapchain usage flag");
		}
		if (static_cast<int>(present_mode) == -1) {
			throw std::runtime_error("Wrong present mode");
		}

		VkSwapchainCreateInfoKHR swapchain_create_info = {
			VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,  // VkStructureType                sType
			nullptr,                                      // const void                    *pNext
			0,                                            // VkSwapchainCreateFlagsKHR      flags
			m_surface(),                                  // VkSurfaceKHR                   surface
			image_count,                                   // uint32_t                       minImageCount
			surface_format.format,                         // VkFormat                       imageFormat
			surface_format.colorSpace,                     // VkColorSpaceKHR                imageColorSpace
			swapchain_extend,                              // VkExtent2D                     imageExtent
			1,                                            // uint32_t                       imageArrayLayers
			swapchain_usage_flag,                           // VkImageUsageFlags              imageUsage
			VK_SHARING_MODE_EXCLUSIVE,                    // VkSharingMode                  imageSharingMode
			0,                                            // uint32_t                       queueFamilyIndexCount
			nullptr,                                      // const uint32_t                *pQueueFamilyIndices
			swapchain_transform,                           // VkSurfaceTransformFlagBitsKHR  preTransform
			VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,            // VkCompositeAlphaFlagBitsKHR    compositeAlpha
			present_mode,                                  // VkPresentModeKHR               presentMode
			VK_TRUE,                                      // VkBool32                       clipped
			m_swapchain()                                 // VkSwapchainKHR                 oldSwapchain
		};

		Vk::Swapchain new_swapchain;
		new_swapchain.create(m_vulkan.get_device().get_handle(), swapchain_create_info);

		m_swapchain = std::move(new_swapchain);
		m_swapchain_size = size();
	}

	void RenderWindow::create_render_pass() {
		std::vector<VkAttachmentDescription> attachment_descriptions = {
			{
				0,                                   // VkAttachmentDescriptionFlags   flags
				m_swapchain.get_format(),             // VkFormat                       format
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

		VkAttachmentReference color_attachment_references = {
			0,                                          // uint32_t                       attachment
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL    // VkImageLayout                  layout
		};

		VkAttachmentReference depth_attachment_ref = {
			1,                                               // uint32_t                       attachment
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL // VkImageLayout                  layout
		};

		VkSubpassDescription subpass_descriptions[] = {
			{
				0,                                          // VkSubpassDescriptionFlags      flags
				VK_PIPELINE_BIND_POINT_GRAPHICS,            // VkPipelineBindPoint            pipelineBindPoint
				0,                                          // uint32_t                       inputAttachmentCount
				nullptr,                                    // const VkAttachmentReference   *pInputAttachments
				1,                                          // uint32_t                       colorAttachmentCount
				&color_attachment_references,                 // const VkAttachmentReference   *pColorAttachments
				nullptr,                                    // const VkAttachmentReference   *pResolveAttachments
				&depth_attachment_ref,                        // const VkAttachmentReference   *pDepthStencilAttachment
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

		VkRenderPassCreateInfo render_pass_create_info = {
			VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,            // VkStructureType                sType
			nullptr,                                              // const void                    *pNext
			0,                                                    // VkRenderPassCreateFlags        flags
			static_cast<uint32_t>(attachment_descriptions.size()), // uint32_t                       attachmentCount
			attachment_descriptions.data(),                        // const VkAttachmentDescription *pAttachments
			1,                                                    // uint32_t                       subpassCount
			subpass_descriptions,                                  // const VkSubpassDescription    *pSubpasses
			static_cast<uint32_t>(dependencies.size()),           // uint32_t                       dependencyCount
			dependencies.data()                                   // const VkSubpassDependency     *pDependencies
		};

		m_render_pass.create(m_vulkan.get_device().get_handle(), render_pass_create_info);
	}

	void RenderWindow::create_depth_ressource() {
		DepthImage newDepth;

		newDepth.create(m_vulkan.get_device(), m_swapchain_size);

		m_depth = std::move(newDepth);
	}

	void RenderWindow::on_window_size_changed() {
		m_vulkan.get_device().get_handle().wait_idle();

		create_swapchain();
		create_depth_ressource();
	}

	VkSurfaceFormatKHR RenderWindow::get_swapchain_format(const std::vector<VkSurfaceFormatKHR>& surface_formats) const {
		if ((surface_formats.size() == 1) && (surface_formats[0].format == VK_FORMAT_UNDEFINED)) {
			return{ VK_FORMAT_R8G8B8A8_UNORM, VK_COLORSPACE_SRGB_NONLINEAR_KHR };
		}

		for (const VkSurfaceFormatKHR& surface_format : surface_formats) {
			if (surface_format.format == VK_FORMAT_R8G8B8A8_UNORM) {
				return surface_format;
			}
		}

		return surface_formats[0];
	}

	uint32_t RenderWindow::get_swapchain_num_images(const VkSurfaceCapabilitiesKHR& capabilities) const {
		uint32_t imageCount = capabilities.minImageCount + 1;
		if ((capabilities.maxImageCount > 0) && (imageCount > capabilities.maxImageCount)) {
			imageCount = capabilities.maxImageCount;
		}

		return imageCount;
	}

	VkExtent2D RenderWindow::get_swapchain_extent(const VkSurfaceCapabilitiesKHR& capabilities, const Vector2ui& size) const {
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

	VkImageUsageFlags RenderWindow::get_swapchain_usage_flags(const VkSurfaceCapabilitiesKHR& capabilities) const {
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

	VkSurfaceTransformFlagBitsKHR RenderWindow::get_swapchain_transform(const VkSurfaceCapabilitiesKHR& capabilities) const {
		if (capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
			return VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		}
		else {
			return capabilities.currentTransform;
		}
	}

	VkPresentModeKHR RenderWindow::get_swapchain_present_mode(const std::vector<VkPresentModeKHR>& present_modes) const {
		// FIFO present mode is always available
		// MAILBOX is the lowest latency V-Sync enabled mode (somNTHing like triple-buffering) so use it if available
		for (const VkPresentModeKHR& present_mode : present_modes) {
			if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return present_mode;
			}
		}
		for (const VkPresentModeKHR& present_mode : present_modes) {
			if (present_mode == VK_PRESENT_MODE_FIFO_KHR) {
				return present_mode;
			}
		}

		std::cerr << "Error: FIFO present mode is not supported by the swapchain" << std::endl;
		return static_cast<VkPresentModeKHR>(-1);
	}

	void RenderWindow::create_framebuffer(Vk::Framebuffer& framebuffer, const Vk::SwapchainImage& swapchainImage) const {
		std::vector<VkImageView> attachements{
			swapchainImage.view(),
			m_depth.view()()
		};

		VkFramebufferCreateInfo framebufferCreateInfo = {
		VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,  // VkStructureType                sType
			nullptr,                                    // const void                    *pNext
			0,                                          // VkFramebufferCreateFlags       flags
			m_render_pass(),                             // VkRenderPass                   renderPass
			static_cast<uint32_t>(attachements.size()), // uint32_t                       attachmentCount
			attachements.data(),                        // const VkImageView             *pAttachments
			m_swapchain_size.x,                          // uint32_t                       width
			m_swapchain_size.y,                          // uint32_t                       height
			1                                           // uint32_t                       layers
		};

		framebuffer.create(m_vulkan.get_device().get_handle(), framebufferCreateInfo);
	}

	void RenderWindow::create_rendering_resources() {
		m_rendering_resources.emplace_back(RenderingResource{ *this });
		m_rendering_resources.emplace_back(RenderingResource{ *this });
		m_rendering_resources.emplace_back(RenderingResource{ *this });

		for (size_t i = 0; i < m_rendering_resources.size(); ++i) {
			m_rendering_resources[i].create(m_vulkan.get_device().graphics_queue().index());
		}
	}
}
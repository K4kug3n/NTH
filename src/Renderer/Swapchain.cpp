#include "Renderer/Swapchain.hpp"

#include "Renderer/Device.hpp"
#include "Renderer/VkUtil.hpp"
#include "Renderer/Semaphore.hpp"

#include <iostream>

namespace Nth {
	Swapchain::Swapchain() :
		m_swapchain(VK_NULL_HANDLE),
		m_format(VK_FORMAT_UNDEFINED),
		m_device(nullptr) {
	}

	Swapchain::Swapchain(Swapchain&& object) noexcept :
		m_swapchain(object.m_swapchain),
		m_format(object.m_format),
		m_device(std::move(object.m_device)) {
		object.m_swapchain = VK_NULL_HANDLE;
	}

	Swapchain::~Swapchain() {
		destroy();
	}

	VkResult Swapchain::aquireNextImage(VkSemaphore const& semaphore, VkFence const& fence, uint32_t & imageIndex) {
		return m_device->vkAcquireNextImageKHR((*m_device)(), m_swapchain, UINT64_MAX, semaphore, fence, &imageIndex);
	}

	bool Swapchain::create(Device const& device, VkSwapchainCreateInfoKHR const& infos) {
		VkResult result{ device.vkCreateSwapchainKHR(device(), &infos, nullptr, &m_swapchain) };
		if (result != VK_SUCCESS) {
			std::cerr << "Error: Can't create swapchain, " << toString(result) << std::endl;
			return false;
		}

		/*std::vector<VkImage> vkImages(m_imageCount, VK_NULL_HANDLE);
		result = device.vkGetSwapchainImagesKHR(device(), m_swapchain, &m_imageCount, vkImages.data());
		if (result != VK_SUCCESS) {
			std::cout << "Error: Could not get swap chain images, " << toString(result) << std::endl;
			return 0;
		}*/

		//m_imagesParameters.resize(vkImages.size());
		//for (size_t i{0}; i < m_imagesParameters.size(); ++i) {
		//	m_imagesParameters[i].image = vkImages[i];
		//	
		//	VkImageViewCreateInfo imageViewCreateInfo = {
		//		VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,   // VkStructureType                sType
		//		nullptr,                                    // const void                    *pNext
		//		0,                                          // VkImageViewCreateFlags         flags
		//		vkImages[i],                                    // VkImage                        image
		//		VK_IMAGE_VIEW_TYPE_2D,                      // VkImageViewType                viewType
		//		infos.imageFormat,                          // VkFormat                       format
		//		{                                           // VkComponentMapping             components
		//			VK_COMPONENT_SWIZZLE_IDENTITY,              // VkComponentSwizzle             r
		//			VK_COMPONENT_SWIZZLE_IDENTITY,              // VkComponentSwizzle             g
		//			VK_COMPONENT_SWIZZLE_IDENTITY,              // VkComponentSwizzle             b
		//			VK_COMPONENT_SWIZZLE_IDENTITY               // VkComponentSwizzle             a
		//		},
		//		{                                           // VkImageSubresourceRange        subresourceRange
		//			VK_IMAGE_ASPECT_COLOR_BIT,                  // VkImageAspectFlags             aspectMask
		//			0,                                          // uint32_t                       baseMipLevel
		//			1,                                          // uint32_t                       levelCount
		//			0,                                          // uint32_t                       baseArrayLayer
		//			1                                           // uint32_t                       layerCount
		//		}
		//	};

		//	if (!m_imagesParameters[i].view.create(device, imageViewCreateInfo)) {
		//		return false;
		//	}
		//}

		m_format = infos.imageFormat;

		m_device = &device;

		return true;
	}

	void Swapchain::destroy() {
		if (m_swapchain != VK_NULL_HANDLE) {
			m_device->vkDestroySwapchainKHR((*m_device)(), m_swapchain, nullptr);
			m_swapchain = VK_NULL_HANDLE;
		}
	}

	VkFormat Swapchain::getFormat() const {
		return m_format;
	}

	uint32_t Swapchain::getImageCount() const {
		uint32_t imageCount = 0;
		VkResult result = m_device->vkGetSwapchainImagesKHR((*m_device)(), m_swapchain, &imageCount, nullptr);
		if ((result != VK_SUCCESS) || (imageCount == 0)) {
			throw std::runtime_error("Could not get the number of swap chain images : " + toString(result));
		}

		return imageCount;
	}

	std::vector<VkImage> Swapchain::getImages() const {
		uint32_t imageCount = getImageCount();
		std::vector<VkImage> swapchainImages(imageCount);
		if (m_device->vkGetSwapchainImagesKHR((*m_device)(), m_swapchain, &imageCount, &swapchainImages[0]) != VK_SUCCESS) {
			throw std::runtime_error("Could not get swap chain images !");
		}

		return swapchainImages;
	}

	VkSwapchainKHR const& Swapchain::operator()() const {
		return m_swapchain;
	}

	Swapchain& Swapchain::operator=(Swapchain&& object) noexcept {
		std::swap(m_swapchain, object.m_swapchain);
		std::swap(m_device, object.m_device);
		std::swap(m_format, object.m_format);

		return *this;
	}
}
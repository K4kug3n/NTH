#include "Renderer/Swapchain.hpp"

#include "Renderer/Device.hpp"
#include "Renderer/VkUtil.hpp"
#include "Renderer/Semaphore.hpp"

#include <iostream>

namespace Nth {
	namespace Vk {
		Swapchain::Swapchain() :
			m_swapchain(VK_NULL_HANDLE),
			m_format(VK_FORMAT_UNDEFINED),
			m_device(nullptr) {
		}

		Swapchain::Swapchain(Swapchain&& object) noexcept :
			m_swapchain(object.m_swapchain),
			m_format(object.m_format),
			m_device(std::move(object.m_device)),
			m_images(std::move(object.m_images)) {
			object.m_swapchain = VK_NULL_HANDLE;
		}

		Swapchain::~Swapchain() {
			destroy();
		}

		VkResult Swapchain::aquireNextImage(VkSemaphore const& semaphore, VkFence const& fence, uint32_t& imageIndex) {
			return m_device->vkAcquireNextImageKHR((*m_device)(), m_swapchain, UINT64_MAX, semaphore, fence, &imageIndex);
		}

		bool Swapchain::create(Device const& device, VkSwapchainCreateInfoKHR const& infos) {
			VkResult result{ device.vkCreateSwapchainKHR(device(), &infos, nullptr, &m_swapchain) };
			if (result != VK_SUCCESS) {
				std::cerr << "Error: Can't create swapchain, " << toString(result) << std::endl;
				return false;
			}

			m_device = &device;

			uint32_t imageCount = 0;
			result = m_device->vkGetSwapchainImagesKHR((*m_device)(), m_swapchain, &imageCount, nullptr);
			if ((result != VK_SUCCESS) || (imageCount == 0)) {
				std::cerr << "Could not get the number of swap chain images : " << toString(result) << std::endl;
				return false;
			}

			std::vector<VkImage> vkImages(imageCount);
			if (m_device->vkGetSwapchainImagesKHR((*m_device)(), m_swapchain, &imageCount, &vkImages[0]) != VK_SUCCESS) {
				std::cerr << "Could not get swap chain images !" << std::endl;
				return false;
			}

			m_images.resize(vkImages.size());
			for (size_t i{ 0 }; i < m_images.size(); ++i) {
				m_images[i].image = vkImages[i];

				VkImageViewCreateInfo imageViewCreateInfo = {
					VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,   // VkStructureType                sType
					nullptr,                                    // const void                    *pNext
					0,                                          // VkImageViewCreateFlags         flags
					vkImages[i],                                    // VkImage                        image
					VK_IMAGE_VIEW_TYPE_2D,                      // VkImageViewType                viewType
					infos.imageFormat,                          // VkFormat                       format
					{                                           // VkComponentMapping             components
						VK_COMPONENT_SWIZZLE_IDENTITY,              // VkComponentSwizzle             r
						VK_COMPONENT_SWIZZLE_IDENTITY,              // VkComponentSwizzle             g
						VK_COMPONENT_SWIZZLE_IDENTITY,              // VkComponentSwizzle             b
						VK_COMPONENT_SWIZZLE_IDENTITY               // VkComponentSwizzle             a
					},
					{                                           // VkImageSubresourceRange        subresourceRange
						VK_IMAGE_ASPECT_COLOR_BIT,                  // VkImageAspectFlags             aspectMask
						0,                                          // uint32_t                       baseMipLevel
						1,                                          // uint32_t                       levelCount
						0,                                          // uint32_t                       baseArrayLayer
						1                                           // uint32_t                       layerCount
					}
				};

				if (!m_images[i].view.create(device, imageViewCreateInfo)) {
					return false;
				}
			}

			m_format = infos.imageFormat;

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
			return static_cast<uint32_t>(m_images.size());
		}

		std::vector<SwapchainImage> const& Swapchain::getImages() const {
			return m_images;
		}

		VkSwapchainKHR Swapchain::operator()() const {
			return m_swapchain;
		}

		Swapchain& Swapchain::operator=(Swapchain&& object) noexcept {
			std::swap(m_swapchain, object.m_swapchain);
			std::swap(m_device, object.m_device);
			std::swap(m_format, object.m_format);
			std::swap(m_images, object.m_images);

			return *this;
		}
	}
}
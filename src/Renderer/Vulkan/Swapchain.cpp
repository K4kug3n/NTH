#include <Renderer/Vulkan/Swapchain.hpp>

#include <Renderer/Vulkan/Device.hpp>
#include <Renderer/Vulkan/VkUtils.hpp>
#include <Renderer/Vulkan/Semaphore.hpp>

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

		VkResult Swapchain::aquire_next_image(VkSemaphore semaphore, VkFence fence, uint32_t& imageIndex) {
			return m_device->vkAcquireNextImageKHR((*m_device)(), m_swapchain, UINT64_MAX, semaphore, fence, &imageIndex);
		}

		void Swapchain::create(const Device& device, const VkSwapchainCreateInfoKHR& infos) {
			VkResult result{ device.vkCreateSwapchainKHR(device(), &infos, nullptr, &m_swapchain) };
			if (result != VK_SUCCESS) {
				throw std::runtime_error("Can't create swapchain, " + to_string(result));
			}

			m_device = &device;

			uint32_t imageCount = 0;
			result = m_device->vkGetSwapchainImagesKHR((*m_device)(), m_swapchain, &imageCount, nullptr);
			if ((result != VK_SUCCESS) || (imageCount == 0)) {
				throw std::runtime_error("Could not get the number of swap chain images, " + to_string(result));
			}

			std::vector<VkImage> vk_images(imageCount);
			result = m_device->vkGetSwapchainImagesKHR((*m_device)(), m_swapchain, &imageCount, &vk_images[0]);
			if (result != VK_SUCCESS) {
				throw std::runtime_error("Could not get swap chain images, " + to_string(result));
			}

			m_images.resize(vk_images.size());
			for (size_t i{ 0 }; i < m_images.size(); ++i) {
				m_images[i].image = vk_images[i];

				VkImageViewCreateInfo image_view_create_info = {
					VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,   // VkStructureType                sType
					nullptr,                                    // const void                    *pNext
					0,                                          // VkImageViewCreateFlags         flags
					vk_images[i],                                    // VkImage                        image
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

				m_images[i].view.create(device, image_view_create_info);
			}

			m_format = infos.imageFormat;
		}

		void Swapchain::destroy() {
			if (m_swapchain != VK_NULL_HANDLE) {
				m_device->vkDestroySwapchainKHR((*m_device)(), m_swapchain, nullptr);
				m_swapchain = VK_NULL_HANDLE;
			}
		}

		VkFormat Swapchain::get_format() const {
			return m_format;
		}

		uint32_t Swapchain::get_image_count() const {
			return static_cast<uint32_t>(m_images.size());
		}

		const std::vector<SwapchainImage>& Swapchain::get_images() const {
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
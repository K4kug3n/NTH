#pragma once

#ifndef NTH_RENDERER_VK_SWAPCHAIN_HPP
#define NTH_RENDERER_VK_SWAPCHAIN_HPP

#include <Renderer/Vulkan/ImageView.hpp>

#include <vulkan/vulkan.h>

#include <vector>

namespace Nth {
	namespace Vk {
		class Device;
		class Semaphore;

		struct SwapchainImage {
			VkImage image;
			ImageView view;
		};

		class Swapchain {
		public:
			Swapchain();
			Swapchain(const Swapchain&) = delete;
			Swapchain(Swapchain&& object) noexcept;
			~Swapchain();

			VkResult aquire_next_image(VkSemaphore semaphore, VkFence fence, uint32_t& image_index);

			void create(const Device& device, const VkSwapchainCreateInfoKHR& infos);
			void destroy();

			VkFormat get_format() const;
			uint32_t get_image_count() const;
			const std::vector<SwapchainImage>& get_images() const;

			VkSwapchainKHR operator()() const;

			Swapchain& operator=(const Swapchain&) = delete;
			Swapchain& operator=(Swapchain&& object) noexcept;

		private:
			VkSwapchainKHR m_swapchain;
			VkFormat m_format;

			std::vector<SwapchainImage> m_images;

			Device const* m_device;
		};
	}
}

#endif

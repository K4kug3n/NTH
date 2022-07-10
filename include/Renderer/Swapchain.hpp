#pragma once

#ifndef NTH_RENDERER_VK_SWAPCHAIN_HPP
#define NTH_RENDERER_VK_SWAPCHAIN_HPP

#include "Renderer/ImageView.hpp"

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
			Swapchain(Swapchain const&) = delete;
			Swapchain(Swapchain&& object) noexcept;
			~Swapchain();

			VkResult aquireNextImage(VkSemaphore const& semaphore, VkFence const& fence, uint32_t& imageIndex);

			bool create(Device const& device, VkSwapchainCreateInfoKHR const& infos);
			void destroy();

			VkFormat getFormat() const;
			uint32_t getImageCount() const;
			std::vector<SwapchainImage> const& getImages() const;

			VkSwapchainKHR operator()() const;

			Swapchain& operator=(Swapchain const&) = delete;
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

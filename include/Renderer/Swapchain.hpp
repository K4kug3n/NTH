#pragma once

#ifndef NTH_SWAPCHAIN_HPP
#define NTH_SWAPCHAIN_HPP

#include <vulkan/vulkan.h>

#include <vector>

namespace Nth {
	class Device;
	class Semaphore;

	class Swapchain {
	public:
		struct ImageParameters;

		Swapchain();
		Swapchain(Swapchain const&) = delete;
		Swapchain(Swapchain&& object) noexcept;
		~Swapchain();

		VkResult aquireNextImage(VkSemaphore const& semaphore, VkFence const& fence, uint32_t & imageIndex);

		bool create(Device const& device, VkSwapchainCreateInfoKHR const& infos);
		void destroy();

		VkFormat getFormat() const;
		uint32_t getImageCount() const;
		std::vector<VkImage> getImages() const;

		VkSwapchainKHR const& operator()() const;

		Swapchain& operator=(Swapchain const&) = delete;
		Swapchain& operator=(Swapchain&& object) noexcept;
		
	private:
		VkSwapchainKHR m_swapchain;
		VkFormat m_format;

		Device const* m_device;
	};
}

#endif

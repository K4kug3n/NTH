#pragma once

#ifndef NTH_RENDERWINDOW_HPP
#define NTH_RENDERWINDOW_HPP

#include "Window/Window.hpp"

#include "Renderer/VulkanInstance.hpp"
#include "Renderer/Surface.hpp"
#include "Renderer/Swapchain.hpp"
#include "Renderer/Queue.hpp"
#include "Renderer/Semaphore.hpp"
#include "Renderer/CommandPool.hpp"
#include "Renderer/CommandBuffer.hpp"

#include "Math/Vector2.hpp"

#include <vector>

namespace Nth {

	class RenderWindow : public Window {
	public:
		RenderWindow(VulkanInstance& vulkanInstance);
		RenderWindow(VulkanInstance& vulkanInstance, VideoMode const& mode, std::string const& title);
		RenderWindow(RenderWindow const&) = delete;
		RenderWindow(RenderWindow&&) = default;
		~RenderWindow();

		bool create(VideoMode const& mode, std::string const& title);
		void draw();

		RenderWindow& operator=(RenderWindow const&) = delete;
		RenderWindow& operator=(RenderWindow&&) = default;

	private:
		bool createSwapchain();
		bool createCommandBuffers();
		bool recordCommandBuffers();
		void onWindowSizeChanged();

		VkSurfaceFormatKHR getSwapchainFormat(std::vector<VkSurfaceFormatKHR> const& surfaceFormats) const;
		uint32_t getSwapchainNumImages(VkSurfaceCapabilitiesKHR const& capabilities) const;
		VkExtent2D getSwapchainExtent(VkSurfaceCapabilitiesKHR const& capabilities, Vector2ui const& size) const;
		VkImageUsageFlags getSwapchainUsageFlags(VkSurfaceCapabilitiesKHR const& capabilities) const;
		VkSurfaceTransformFlagBitsKHR getSwapchainTransform(VkSurfaceCapabilitiesKHR const& capabilities) const;
		VkPresentModeKHR getSwapchainPresentMode(std::vector<VkPresentModeKHR> const& presentModes) const;

		VulkanInstance& m_vulkanInstance;
		Surface m_surface;
		Swapchain m_swapchain;
		Queue m_presentQueue;
		Queue m_graphicsQueue;
		Semaphore m_imageAvailableSemaphore;
		Semaphore m_renderingFinishedSemaphore;
		CommandPool m_presentQueueCommandPool;
		std::vector<CommandBuffer> m_presentQueueCmdBuffers;

		Vector2ui m_swapchainSize;
	};
}

#endif

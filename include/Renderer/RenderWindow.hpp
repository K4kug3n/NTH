#ifndef NTH_RENDERER_RENDERWINDOW_HPP
#define NTH_RENDERER_RENDERWINDOW_HPP

#include <Window/Window.hpp>

#include <Renderer/RenderInstance.hpp>
#include <Renderer/Vulkan/Surface.hpp>
#include <Renderer/Vulkan/Swapchain.hpp>
#include <Renderer/Vulkan/Queue.hpp>
#include <Renderer/Vulkan/RenderPass.hpp>
#include <Renderer/DepthImage.hpp>
#include <Renderer/SceneParameters.hpp>
#include <Renderer/RenderingResource.hpp>

#include <Math/Vector2.hpp>

#include <vector>

namespace Nth {
	namespace Vk {
		class Framebuffer;
	}
	class RenderObject;

	class RenderWindow : public Window {
	public:
		RenderWindow(RenderInstance& vulkanInstance);
		RenderWindow(RenderInstance& vulkanInstance, VideoMode const& mode, const std::string_view title);
		RenderWindow(RenderWindow const&) = delete;
		RenderWindow(RenderWindow&&) = default;
		~RenderWindow();

		bool create(VideoMode const& mode, const std::string_view title);

		RenderingResource& aquireNextImage();
		void present(uint32_t imageIndex, Vk::Semaphore const& semaphore);

		Vk::RenderPass& getRenderPass();
		RenderDevice const& getDevice() const;

		RenderWindow& operator=(RenderWindow const&) = delete;
		RenderWindow& operator=(RenderWindow&&) = default;

	private:
		bool createSwapchain();
		bool createRenderPass();
		bool createDepthRessource();
		void onWindowSizeChanged();

		VkSurfaceFormatKHR getSwapchainFormat(std::vector<VkSurfaceFormatKHR> const& surfaceFormats) const;
		uint32_t getSwapchainNumImages(VkSurfaceCapabilitiesKHR const& capabilities) const;
		VkExtent2D getSwapchainExtent(VkSurfaceCapabilitiesKHR const& capabilities, Vector2ui const& size) const;
		VkImageUsageFlags getSwapchainUsageFlags(VkSurfaceCapabilitiesKHR const& capabilities) const;
		VkSurfaceTransformFlagBitsKHR getSwapchainTransform(VkSurfaceCapabilitiesKHR const& capabilities) const;
		VkPresentModeKHR getSwapchainPresentMode(std::vector<VkPresentModeKHR> const& presentModes) const;

		bool createFramebuffer(Vk::Framebuffer& framebuffer, Vk::SwapchainImage const& swapchainImage) const;
		bool createRenderingResources();

		RenderInstance& m_vulkan;
		Vk::Surface m_surface;
		Vk::Swapchain m_swapchain;
		Vk::RenderPass m_renderPass;
		DepthImage m_depth;

		size_t m_ressourceIndex;
		std::vector<RenderingResource> m_renderingResources;

		Vector2ui m_swapchainSize;
	};
}

#endif

#ifndef NTH_RENDERER_RENDERWINDOW_HPP
#define NTH_RENDERER_RENDERWINDOW_HPP

#include <Window/Window.hpp>

#include <Renderer/VulkanInstance.hpp>
#include <Renderer/Vulkan/Surface.hpp>
#include <Renderer/Vulkan/Swapchain.hpp>
#include <Renderer/Vulkan/Queue.hpp>
#include <Renderer/Vulkan/RenderPass.hpp>
#include <Renderer/DepthImage.hpp>
#include <Renderer/SceneParameters.hpp>

#include <Math/Vector2.hpp>

#include <vector>

namespace Nth {
	namespace Vk {
		class Framebuffer;
	}
	class RenderObject;
	class RenderingResource;

	class RenderWindow : public Window {
	public:
		RenderWindow(VulkanInstance& vulkanInstance);
		RenderWindow(VulkanInstance& vulkanInstance, VideoMode const& mode, const std::string_view title);
		RenderWindow(RenderWindow const&) = delete;
		RenderWindow(RenderWindow&&) = default;
		~RenderWindow();

		bool create(VideoMode const& mode, const std::string_view title);
		bool draw(RenderingResource& ressource, std::vector<RenderObject> const& objects, LightGpuObject const& light, ViewerGpuObject const& viewer);

		Vk::RenderPass& getRenderPass();

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

		bool prepareFrame(RenderingResource& ressources, Vk::SwapchainImage const& imageParameters, std::vector<RenderObject> const& objects, LightGpuObject const& light, ViewerGpuObject const& viewer) const;

		bool createFramebuffer(Vk::Framebuffer& framebuffer, Vk::SwapchainImage const& swapchainImage) const;

		VulkanInstance& m_vulkan;
		Vk::Surface m_surface;
		Vk::Swapchain m_swapchain;
		Vk::RenderPass m_renderPass;
		DepthImage m_depth;

		Vector2ui m_swapchainSize;
	};
}

#endif

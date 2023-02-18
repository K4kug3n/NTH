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

#include <Maths/Vector2.hpp>

#include <vector>

namespace Nth {
	namespace Vk {
		class Framebuffer;
	}
	class RenderObject;

	class RenderWindow : public Window {
	public:
		RenderWindow(RenderInstance& vulkan_instance);
		RenderWindow(RenderInstance& vulkan_instance, const std::string& title);
		RenderWindow(const RenderWindow&) = delete;
		RenderWindow(RenderWindow&&) = default;
		~RenderWindow();

		void create();

		RenderingResource& aquireNextImage();
		void present(uint32_t imageIndex, const Vk::Semaphore& semaphore);

		Vk::RenderPass& get_render_pass();
		const RenderDevice& get_device() const;

		RenderWindow& operator=(const RenderWindow&) = delete;
		RenderWindow& operator=(RenderWindow&&) = default;

	private:
		void create_swapchain();
		void create_render_pass();
		void create_depth_ressource();
		void on_window_size_changed();

		VkSurfaceFormatKHR get_swapchain_format(const std::vector<VkSurfaceFormatKHR>& surfaceFormats) const;
		uint32_t get_swapchain_num_images(const VkSurfaceCapabilitiesKHR& capabilities) const;
		VkExtent2D get_swapchain_extent(const VkSurfaceCapabilitiesKHR& capabilities, const Vector2ui& size) const;
		VkImageUsageFlags get_swapchain_usage_flags(const VkSurfaceCapabilitiesKHR& capabilities) const;
		VkSurfaceTransformFlagBitsKHR get_swapchain_transform(const VkSurfaceCapabilitiesKHR& capabilities) const;
		VkPresentModeKHR get_swapchain_present_mode(const std::vector<VkPresentModeKHR>& presentModes) const;

		void create_framebuffer(Vk::Framebuffer& framebuffer, const Vk::SwapchainImage& swapchain_image) const;
		void create_rendering_resources();

		RenderInstance& m_vulkan;
		Vk::Surface m_surface;
		Vk::Swapchain m_swapchain;
		Vk::RenderPass m_render_pass;
		DepthImage m_depth;

		size_t m_ressourceIndex;
		std::vector<RenderingResource> m_rendering_resources;

		Vector2ui m_swapchain_size;
	};
}

#endif

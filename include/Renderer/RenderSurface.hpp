#ifndef NTH_RENDERER_RENDERSURFACE_HPP
#define NTH_RENDERER_RENDERSURFACE_HPP

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
	struct WindowHandle;

	class RenderSurface {
	public:
		RenderSurface(RenderInstance& vulkan_instance);
		RenderSurface(const RenderSurface&) = delete;
		RenderSurface(RenderSurface&&) = default;
		~RenderSurface();

		RenderingResource& aquire_next_image(const Vector2ui& size);

		void create(const WindowHandle& window_hanlde);
		const Vk::Surface& get_handle() const;
		const Vk::RenderPass& get_render_pass() const;
		// TODO : Review this
		void init_render_pipeline(const Vector2ui& size);
		
		void present(uint32_t image_index, const Vk::Semaphore& semaphore, const Vector2ui& size);
		const Vector2ui& size() const;

		RenderSurface& operator=(const RenderSurface&) = delete;
		RenderSurface& operator=(RenderSurface&&) = default;

	private:
		void create_swapchain(const Vector2ui& size);
		void create_render_pass();
		void create_depth_ressource();
		void on_window_size_changed(const Vector2ui& size);

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

		size_t m_ressource_index;
		std::vector<RenderingResource> m_rendering_resources;

		Vector2ui m_swapchain_size;
	};
}

#endif

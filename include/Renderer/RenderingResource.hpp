#ifndef NTH_RENDERER_RENDERINGRESOURCE_HPP
#define NTH_RENDERER_RENDERINGRESOURCE_HPP

#include <Renderer/Vulkan/Framebuffer.hpp>
#include <Renderer/Vulkan/CommandPool.hpp>
#include <Renderer/Vulkan/CommandBuffer.hpp>
#include <Renderer/Vulkan/Semaphore.hpp>
#include <Renderer/Vulkan/Fence.hpp>

#include <functional>

namespace Nth {
	class Vk::Device;
	class RenderSurface;
	class RenderInstance;
	template<typename T> class Vector2;
	using Vector2ui = Vector2<unsigned int>;

	class RenderingResource {
	public:
		RenderingResource(RenderInstance& instance, RenderSurface& surface);
		RenderingResource(const RenderingResource&) = delete;
		RenderingResource(RenderingResource&&) = default;
		~RenderingResource() = default;

		void create(uint32_t family_index);

		void prepare(std::function<void(Vk::CommandBuffer&)> action);
		void present(const Vector2ui& size);

		Vk::Framebuffer framebuffer;
		Vk::CommandPool command_pool;
		Vk::CommandBuffer command_buffer;
		Vk::Semaphore image_available_semaphore;
		Vk::Semaphore finished_rendering_semaphore;
		Vk::Fence fence;

		VkImage swapchain_image;
		uint32_t image_index;

		RenderingResource& operator=(const RenderingResource&) = delete;
		RenderingResource& operator=(RenderingResource&&) = default;

	private:
		RenderInstance& m_instance;
		RenderSurface& m_surface;
	};
}

#endif

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
	class RenderWindow;
	class RenderDevice;

	class RenderingResource {
	public:
		RenderingResource(RenderWindow& owner);
		RenderingResource(RenderingResource const&) = delete;
		RenderingResource(RenderingResource&&) = default;
		~RenderingResource() = default;

		bool create(uint32_t familyIndex);

		void prepare(std::function<void(Vk::CommandBuffer&)> action);
		void present();

		Vk::Framebuffer framebuffer;
		Vk::CommandPool commandPool;
		Vk::CommandBuffer commandBuffer;
		Vk::Semaphore imageAvailableSemaphore;
		Vk::Semaphore finishedRenderingSemaphore;
		Vk::Fence fence;

		VkImage swapchainImage;
		uint32_t imageIndex;

		RenderingResource& operator=(RenderingResource const&) = delete;
		RenderingResource& operator=(RenderingResource&&) = default;

	private:
		RenderWindow& m_owner;
	};
}

#endif

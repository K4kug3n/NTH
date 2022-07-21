#ifndef NTH_RENDERER_VK_RENDERINGRESOURCE_HPP
#define NTH_RENDERER_VK_RENDERINGRESOURCE_HPP

#include "Renderer/Framebuffer.hpp"
#include "Renderer/CommandBuffer.hpp"
#include "Renderer/Semaphore.hpp"
#include "Renderer/Fence.hpp"

namespace Nth {
	class Vk::Device;
	class Vk::CommandPool;

	class RenderingResource {
	public:
		RenderingResource() = default;
		RenderingResource(RenderingResource const&) = delete;
		RenderingResource(RenderingResource&&) = delete;
		~RenderingResource() = default;

		bool create(Vk::Device const& device, Vk::CommandPool& pool);

		Vk::Framebuffer framebuffer;
		Vk::CommandBuffer commandBuffer;
		Vk::Semaphore imageAvailableSemaphore;
		Vk::Semaphore finishedRenderingSemaphore;
		Vk::Fence fence;

		RenderingResource& operator=(RenderingResource const&) = delete;
		RenderingResource& operator=(RenderingResource&&) = delete;
	};
}

#endif

#ifndef NTH_RENDERER_VK_RENDERINGRESOURCE_HPP
#define NTH_RENDERER_VK_RENDERINGRESOURCE_HPP

#include "Renderer/Framebuffer.hpp"
#include "Renderer/CommandBuffer.hpp"
#include "Renderer/Semaphore.hpp"
#include "Renderer/Fence.hpp"

namespace Nth {
	namespace Vk {
		class RenderingResource {
		public:
			RenderingResource();
			RenderingResource(RenderingResource const&) = delete;
			RenderingResource(RenderingResource&&) = delete;
			~RenderingResource() = default;

			Framebuffer framebuffer;
			CommandBuffer commandBuffer;
			Semaphore imageAvailableSemaphore;
			Semaphore finishedRenderingSemaphore;
			Fence fence;

			RenderingResource& operator=(RenderingResource const&) = delete;
			RenderingResource& operator=(RenderingResource&&) = delete;
		};
	}
}

#endif

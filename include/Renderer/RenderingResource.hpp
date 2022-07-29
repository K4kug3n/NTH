#ifndef NTH_RENDERER_VK_RENDERINGRESOURCE_HPP
#define NTH_RENDERER_VK_RENDERINGRESOURCE_HPP

#include "Renderer/Vulkan/Framebuffer.hpp"
#include "Renderer/Vulkan/CommandPool.hpp"
#include "Renderer/Vulkan/CommandBuffer.hpp"
#include "Renderer/Vulkan/Semaphore.hpp"
#include "Renderer/Vulkan/Fence.hpp"

namespace Nth {
	class Vk::Device;

	class RenderingResource {
	public:
		RenderingResource() = default;
		RenderingResource(RenderingResource const&) = delete;
		RenderingResource(RenderingResource&&) = delete;
		~RenderingResource() = default;

		bool create(Vk::Device const& device, uint32_t index);

		Vk::Framebuffer framebuffer;
		Vk::CommandPool commandPool;
		Vk::CommandBuffer commandBuffer;
		Vk::Semaphore imageAvailableSemaphore;
		Vk::Semaphore finishedRenderingSemaphore;
		Vk::Fence fence;

		RenderingResource& operator=(RenderingResource const&) = delete;
		RenderingResource& operator=(RenderingResource&&) = delete;
	};
}

#endif

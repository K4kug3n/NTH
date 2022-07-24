#ifndef NTH_RENDERER_VK_RENDERPASS_HPP
#define NTH_RENDERER_VK_RENDERPASS_HPP

#include <vulkan/vulkan.h>

namespace Nth {
	namespace Vk {
		class Device;

		class RenderPass {
		public:
			RenderPass();
			RenderPass(RenderPass const&) = delete;
			RenderPass(RenderPass&&) = delete;
			~RenderPass();

			bool create(Device const& device, VkRenderPassCreateInfo const& infos);
			void destroy();

			VkRenderPass const& operator()() const;

			RenderPass& operator=(RenderPass const&) = delete;
			RenderPass& operator=(RenderPass&&) = delete;

		private:
			VkRenderPass m_renderPass;
			Device const* m_device;
		};
	}
}

#endif

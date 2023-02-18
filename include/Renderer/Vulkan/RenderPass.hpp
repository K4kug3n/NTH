#ifndef NTH_RENDERER_VK_RENDERPASS_HPP
#define NTH_RENDERER_VK_RENDERPASS_HPP

#include <vulkan/vulkan.h>

namespace Nth {
	namespace Vk {
		class Device;

		class RenderPass {
		public:
			RenderPass();
			RenderPass(const RenderPass&) = delete;
			RenderPass(RenderPass&&) = delete;
			~RenderPass();

			void create(const Device& device, const VkRenderPassCreateInfo& infos);
			void destroy();

			VkRenderPass operator()() const;

			RenderPass& operator=(const RenderPass&) = delete;
			RenderPass& operator=(RenderPass&&) = delete;

		private:
			VkRenderPass m_render_pass;
			Device const* m_device;
		};
	}
}

#endif

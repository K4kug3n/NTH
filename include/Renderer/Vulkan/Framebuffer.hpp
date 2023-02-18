#ifndef NTH_RENDERER_VK_FRAMEBUFFER_HPP
#define NTH_RENDERER_VK_FRAMEBUFFER_HPP

#include <vulkan/vulkan.h>

namespace Nth {
	namespace Vk {
		class Device;

		class Framebuffer {
		public:
			Framebuffer();
			Framebuffer(const Framebuffer&) = delete;
			Framebuffer(Framebuffer&& object) noexcept;
			~Framebuffer();

			void create(const Device& device, const VkFramebufferCreateInfo& infos);
			void destroy();

			bool is_valid() const;

			VkFramebuffer operator()() const;

			Framebuffer& operator=(const Framebuffer&) = delete;
			Framebuffer& operator=(Framebuffer&& object) noexcept;

		private:
			VkFramebuffer m_framebuffer;
			Device const* m_device;
		};
	}
}

#endif

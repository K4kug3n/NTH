#ifndef NTH_RENDERER_VK_FRAMEBUFFER_HPP
#define NTH_RENDERER_VK_FRAMEBUFFER_HPP

#include <vulkan/vulkan.h>

namespace Nth {
	namespace Vk {
		class Device;

		class Framebuffer {
		public:
			Framebuffer();
			Framebuffer(Framebuffer const&) = delete;
			Framebuffer(Framebuffer&& object) noexcept;
			~Framebuffer();

			bool create(Device const& device, VkFramebufferCreateInfo const& infos);
			void destroy();

			bool isValid() const;

			VkFramebuffer const& operator()() const;

			Framebuffer& operator=(Framebuffer const&) = delete;
			Framebuffer& operator=(Framebuffer&& object) noexcept;

		private:
			VkFramebuffer m_framebuffer;
			Device const* m_device;
		};
	}
}

#endif

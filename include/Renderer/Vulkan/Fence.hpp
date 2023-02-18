#ifndef NTH_RENDERER_VK_FENCE_HPP
#define NTH_RENDERER_VK_FENCE_HPP

#include <vulkan/vulkan.h>

namespace Nth {
	namespace Vk {
		class Device;

		class Fence {
		public:
			Fence();
			Fence(const Fence&) = delete;
			Fence(Fence&& object) noexcept;
			~Fence();

			void create(const Device& device, VkFenceCreateFlags flags);
			void reset() const;
			void wait(uint64_t timeout) const;
			
			VkFence operator()() const;

			Fence& operator=(const Fence&) = delete;
			Fence& operator=(Fence&& object) noexcept;

		private:
			VkFence m_fence;
			Device const* m_device;
		};
	}
}

#endif

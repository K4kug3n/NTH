#ifndef NTH_RENDERER_VK_SEMAPHORE_HPP
#define NTH_RENDERER_VK_SEMAPHORE_HPP

#include <vulkan/vulkan.h>

namespace Nth {
	namespace Vk {
		class Device;

		class Semaphore {
		public:
			Semaphore();
			Semaphore(Semaphore const&) = delete;
			Semaphore(Semaphore&& object) noexcept;
			~Semaphore();

			bool create(Device const& device);

			VkSemaphore const& operator()() const;

			Semaphore& operator=(Semaphore const&) = delete;
			Semaphore& operator=(Semaphore&&) = delete;

		private:
			VkSemaphore m_sempahore;
			Device const* m_device;
		};
	}
}

#endif

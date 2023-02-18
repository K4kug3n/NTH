#ifndef NTH_RENDERER_VK_SEMAPHORE_HPP
#define NTH_RENDERER_VK_SEMAPHORE_HPP

#include <vulkan/vulkan.h>

namespace Nth {
	namespace Vk {
		class Device;

		class Semaphore {
		public:
			Semaphore();
			Semaphore(const Semaphore&) = delete;
			Semaphore(Semaphore&& object) noexcept;
			~Semaphore();

			void create(const Device& device);

			VkSemaphore operator()() const;

			Semaphore& operator=(const Semaphore&) = delete;
			Semaphore& operator=(Semaphore&& object) noexcept;

		private:
			VkSemaphore m_sempahore;
			Device const* m_device;
		};
	}
}

#endif

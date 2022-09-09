#ifndef NTH_RENDERER_VK_QUEUE_HPP
#define NTH_RENDERER_VK_QUEUE_HPP

#include <vulkan/vulkan.h>

namespace Nth {
	namespace Vk {
		class Device;

		class Queue {
		public:
			Queue() = default;
			Queue(Queue const&) = delete;
			Queue(Queue&&) = delete;
			~Queue() = default;

			bool create(Device const& device, uint32_t index);
			bool submit(VkSubmitInfo const& infos, VkFence fence) const;

			VkResult present(VkPresentInfoKHR const& infos) const;

			uint32_t index() const;

			VkQueue const& operator()() const;

			Queue& operator=(Queue const&) = delete;
			Queue& operator=(Queue&&) = delete;

		private:
			VkQueue m_queue;
			Device const* m_device;
			uint32_t m_index;
		};

		bool operator==(Queue const& queue1, Queue const& queue2);
		bool operator!=(Queue const& queue1, Queue const& queue2);
	}
}

#endif

#ifndef NTH_RENDERER_VK_QUEUE_HPP
#define NTH_RENDERER_VK_QUEUE_HPP

#include <vulkan/vulkan.h>

namespace Nth {
	namespace Vk {
		class Device;

		class Queue {
		public:
			Queue() = default;
			Queue(const Queue&) = delete;
			Queue(Queue&&) = delete;
			~Queue() = default;

			void create(const Device& device, uint32_t index);
			void submit(const VkSubmitInfo& infos, VkFence fence) const;

			VkResult present(const VkPresentInfoKHR& infos) const;

			uint32_t index() const;

			VkQueue operator()() const;

			Queue& operator=(const Queue&) = delete;
			Queue& operator=(Queue&&) = delete;

		private:
			VkQueue m_queue;
			Device const* m_device;
			uint32_t m_index;
		};

		bool operator==(const Queue& queue_1, const Queue& queue_2);
		bool operator!=(const Queue& queue_1, const Queue& queue_2);
	}
}

#endif

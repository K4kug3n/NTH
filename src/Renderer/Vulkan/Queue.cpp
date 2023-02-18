#include <Renderer/Vulkan/Queue.hpp>

#include <Renderer/Vulkan/VkUtils.hpp>
#include <Renderer/Vulkan/Device.hpp>

#include <iostream>

namespace Nth {
	namespace Vk {
		void Queue::create(const Device& device, uint32_t index) {
			device.vkGetDeviceQueue(device(), index, 0, &m_queue);

			m_index = index;
			m_device = &device;
		}

		void Queue::submit(const VkSubmitInfo& infos, VkFence fence) const {
			VkResult result{ m_device->vkQueueSubmit(m_queue, 1, &infos, fence) };
			if (result != VK_SUCCESS) {
				throw std::runtime_error("Can't submit to queue, " + to_string(result));
			}
		}

		VkResult Queue::present(const VkPresentInfoKHR& infos) const {
			return m_device->vkQueuePresentKHR(m_queue, &infos);
		}

		uint32_t Queue::index() const {
			return m_index;
		}

		VkQueue Queue::operator()() const {
			return m_queue;
		}

		bool operator==(const Queue& queue1, const Queue& queue2) {
			return queue1() == queue2();
		}

		bool operator!=(const Queue& queue1, const Queue& queue2) {
			return !(queue1 == queue2);
		}
	}
}

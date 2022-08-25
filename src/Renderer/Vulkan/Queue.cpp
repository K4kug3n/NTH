#include <Renderer/Vulkan/Queue.hpp>

#include <Renderer/Vulkan/Device.hpp>

#include <iostream>

namespace Nth {
	namespace Vk {
		bool Queue::create(Device const& device, uint32_t index) {
			device.vkGetDeviceQueue(device(), index, 0, &m_queue);

			m_index = index;
			m_device = &device;

			return true;
		}

		bool Queue::submit(VkSubmitInfo const& infos, VkFence fence) const {
			VkResult result{ m_device->vkQueueSubmit(m_queue, 1, &infos, fence) };
			if (result != VK_SUCCESS) {
				std::cerr << "Error: Can't submit to queue" << std::endl;
				return false;
			}

			return true;
		}

		VkResult Queue::present(VkPresentInfoKHR const& infos) const {
			return m_device->vkQueuePresentKHR(m_queue, &infos);
		}

		uint32_t Queue::index() const {
			return m_index;
		}

		VkQueue const& Queue::operator()() const {
			return m_queue;
		}

		bool operator==(Queue& queue1, Queue& queue2) {
			return queue1() == queue2();
		}

		bool operator!=(Queue& queue1, Queue& queue2) {
			return !(queue1 == queue2);
		}
	}
}

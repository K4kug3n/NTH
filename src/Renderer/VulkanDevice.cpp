#include "Renderer/VulkanDevice.hpp"
#include "Renderer/VulkanInstance.hpp"

#include "Renderer/Vulkan/PhysicalDevice.hpp"
#include "Renderer/Vulkan/Queue.hpp"

#include <stdexcept>

namespace Nth {
	VulkanDevice::VulkanDevice(VulkanInstance const& instance) :
		m_instance(instance),
		m_device(instance.getHandle()) { }

	void VulkanDevice::create(Vk::PhysicalDevice physicalDevice, VkDeviceCreateInfo const& infos, uint32_t presentQueueFamilyIndex, uint32_t graphicQueueFamilyIndex) {
		if (!m_device.create(std::move(physicalDevice), infos, presentQueueFamilyIndex, graphicQueueFamilyIndex)) {
			throw std::runtime_error("Can't create device");
		}

		if (!m_presentQueue.create(m_device, presentQueueFamilyIndex)) {
			throw std::runtime_error("Can't create present queue");
		}

		if (!m_graphicsQueue.create(m_device, graphicQueueFamilyIndex)) {
			throw std::runtime_error("Can't create graphics queue");
		}
	}

	Vk::Queue& VulkanDevice::presentQueue() {
		return m_presentQueue;
	}

	Vk::Queue const& VulkanDevice::presentQueue() const {
		return m_presentQueue;
	}

	Vk::Queue& VulkanDevice::graphicsQueue() {
		return m_graphicsQueue;
	}

	Vk::Queue const& VulkanDevice::graphicsQueue() const {
		return m_graphicsQueue;
	}

	Vk::Device& VulkanDevice::getHandle() {
		return m_device;
	}

	Vk::Device const& VulkanDevice::getHandle() const {
		return m_device;
	}
}

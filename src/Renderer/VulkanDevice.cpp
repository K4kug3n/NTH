#include <Renderer/VulkanDevice.hpp>
#include <Renderer/VulkanInstance.hpp>

#include <Renderer/Vulkan/PhysicalDevice.hpp>
#include <Renderer/Vulkan/Queue.hpp>
#include <Renderer/Vulkan/CommandBuffer.hpp>

#include <stdexcept>

namespace Nth {
	VulkanDevice::VulkanDevice(VulkanInstance const& instance) :
		m_instance(instance),
		m_device(instance.getHandle()) { }

	VulkanDevice::~VulkanDevice() {
		m_pool.destroy();
	}

	void VulkanDevice::create(Vk::PhysicalDevice physicalDevice, VkDeviceCreateInfo const& infos, uint32_t presentQueueFamilyIndex, uint32_t graphicQueueFamilyIndex) {
		if (!m_device.create(std::move(physicalDevice), infos)) {
			throw std::runtime_error("Can't create device");
		}

		if (!m_presentQueue.create(m_device, presentQueueFamilyIndex)) {
			throw std::runtime_error("Can't create present queue");
		}

		if (!m_graphicsQueue.create(m_device, graphicQueueFamilyIndex)) {
			throw std::runtime_error("Can't create graphics queue");
		}

		if (!m_pool.create(m_device, graphicQueueFamilyIndex, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT)) {
			throw std::runtime_error("Can't create pool");
		}
	}

	Vk::CommandBuffer VulkanDevice::allocateCommandBuffer() const  {
		Vk::CommandBuffer commandBuffer;

		if (!m_pool.allocateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, commandBuffer)) {
			throw std::runtime_error("Can't create command buffer");
		}

		return commandBuffer;
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

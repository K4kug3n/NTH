#include <Renderer/RenderDevice.hpp>
#include <Renderer/RenderInstance.hpp>

#include <Renderer/Vulkan/PhysicalDevice.hpp>
#include <Renderer/Vulkan/Queue.hpp>
#include <Renderer/Vulkan/CommandBuffer.hpp>

#include <stdexcept>

namespace Nth {
	RenderDevice::RenderDevice(RenderInstance const& instance) :
		m_instance(instance),
		m_device(instance.getHandle()) { }

	RenderDevice::~RenderDevice() {
		m_pool.destroy();
	}

	void RenderDevice::create(Vk::PhysicalDevice physicalDevice, VkDeviceCreateInfo const& infos, uint32_t presentQueueFamilyIndex, uint32_t graphicQueueFamilyIndex) {
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

	Vk::CommandBuffer RenderDevice::allocateCommandBuffer() const  {
		Vk::CommandBuffer commandBuffer;

		if (!m_pool.allocateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, commandBuffer)) {
			throw std::runtime_error("Can't create command buffer");
		}

		return commandBuffer;
	}

	Vk::Queue& RenderDevice::presentQueue() {
		return m_presentQueue;
	}

	Vk::Queue const& RenderDevice::presentQueue() const {
		return m_presentQueue;
	}

	Vk::Queue& RenderDevice::graphicsQueue() {
		return m_graphicsQueue;
	}

	Vk::Queue const& RenderDevice::graphicsQueue() const {
		return m_graphicsQueue;
	}

	Vk::Device& RenderDevice::getHandle() {
		return m_device;
	}

	Vk::Device const& RenderDevice::getHandle() const {
		return m_device;
	}
}

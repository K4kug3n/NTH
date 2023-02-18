#include <Renderer/RenderDevice.hpp>
#include <Renderer/RenderInstance.hpp>

#include <Renderer/Vulkan/PhysicalDevice.hpp>
#include <Renderer/Vulkan/Queue.hpp>
#include <Renderer/Vulkan/CommandBuffer.hpp>

#include <stdexcept>

namespace Nth {
	RenderDevice::RenderDevice(const RenderInstance& instance) :
		m_instance(instance),
		m_device(instance.get_handle()) { }

	RenderDevice::~RenderDevice() {
		m_pool.destroy();
	}

	void RenderDevice::create(Vk::PhysicalDevice physicalDevice, const VkDeviceCreateInfo& infos, uint32_t presentQueueFamilyIndex, uint32_t graphicQueueFamilyIndex) {
		m_device.create(std::move(physicalDevice), infos);

		m_present_queue.create(m_device, presentQueueFamilyIndex);

		m_graphics_queue.create(m_device, graphicQueueFamilyIndex);

		m_pool.create(m_device, graphicQueueFamilyIndex, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);
	}

	Vk::CommandBuffer RenderDevice::allocate_command_buffer() const  {
		Vk::CommandBuffer commandBuffer;

		m_pool.allocate_command_buffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, commandBuffer);

		return commandBuffer;
	}

	Vk::Queue& RenderDevice::present_queue() {
		return m_present_queue;
	}

	const Vk::Queue& RenderDevice::present_queue() const {
		return m_present_queue;
	}

	Vk::Queue& RenderDevice::graphics_queue() {
		return m_graphics_queue;
	}

	const Vk::Queue& RenderDevice::graphics_queue() const {
		return m_graphics_queue;
	}

	Vk::Device& RenderDevice::get_handle() {
		return m_device;
	}

	const Vk::Device& RenderDevice::get_handle() const {
		return m_device;
	}
}

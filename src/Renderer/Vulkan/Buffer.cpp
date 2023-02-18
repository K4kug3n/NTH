#include <Renderer/Vulkan/Buffer.hpp>

#include <Renderer/Vulkan/Device.hpp>
#include <Renderer/Vulkan/DeviceMemory.hpp>
#include <Renderer/Vulkan/VkUtils.hpp>

#include <stdexcept>

namespace Nth {
	namespace Vk {
		Buffer::Buffer() :
			m_buffer(VK_NULL_HANDLE),
			m_device(nullptr) {
		}

		Buffer::Buffer(Buffer&& object) noexcept :
			m_buffer(object.m_buffer),
			m_device(object.m_device),
			m_size(object.m_size) {
			object.m_buffer = VK_NULL_HANDLE;
		}

		Buffer::~Buffer() {
			if (m_buffer != VK_NULL_HANDLE) {
				m_device->vkDestroyBuffer((*m_device)(), m_buffer, nullptr);
			}
		}

		void Buffer::create(const Device& device, const VkBufferCreateInfo& infos) {
			VkResult result{ device.vkCreateBuffer(device(), &infos, nullptr, &m_buffer) };;
			if (result != VK_SUCCESS) {
				throw std::runtime_error("Error: Can't create buffer, " + to_string(result));
			}

			m_device = &device;
			m_size = static_cast<uint32_t>(infos.size);
		}

		void Buffer::bind_buffer_memory(const DeviceMemory& deviceMemory) const {
			VkResult result{ m_device->vkBindBufferMemory((*m_device)(), m_buffer, deviceMemory(), 0) };
			if (result != VK_SUCCESS) {
				throw std::runtime_error("Error: Could not bind memory for buffer, " + to_string(result));
			}
		}

		uint32_t Buffer::get_size() const {
			return m_size;
		}

		VkMemoryRequirements Buffer::get_memory_requirements() const {
			VkMemoryRequirements requirements;

			m_device->vkGetBufferMemoryRequirements((*m_device)(), m_buffer, &requirements);

			return requirements;
		}

		VkBuffer Buffer::operator()() const {
			return m_buffer;
		}

		Buffer& Buffer::operator=(Buffer&& object) noexcept {
			m_buffer = object.m_buffer;
			m_device = object.m_device;
			m_size = object.m_size;

			object.m_buffer = VK_NULL_HANDLE;

			return *this;
		}
	}
}

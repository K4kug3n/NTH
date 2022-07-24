#include "Renderer/Vulkan/Buffer.hpp"

#include "Renderer/Vulkan/Device.hpp"
#include "Renderer/Vulkan/DeviceMemory.hpp"
#include "Renderer/Vulkan/VkUtil.hpp"

#include <iostream>

namespace Nth {
	namespace Vk {
		Buffer::Buffer() :
			m_buffer(VK_NULL_HANDLE),
			m_device(nullptr) {
		}

		Buffer::~Buffer() {
			if (m_buffer != VK_NULL_HANDLE) {
				m_device->vkDestroyBuffer((*m_device)(), m_buffer, nullptr);
			}
		}

		bool Buffer::create(Device const& device, VkBufferCreateInfo const& infos) {
			VkResult result{ device.vkCreateBuffer(device(), &infos, nullptr, &m_buffer) };;
			if (result != VK_SUCCESS) {
				std::cerr << "Error: Can't create buffer, " << toString(result) << std::endl;
				return false;
			}

			m_device = &device;
			m_size = static_cast<uint32_t>(infos.size);

			return true;
		}

		bool Buffer::bindBufferMemory(DeviceMemory const& deviceMemory) const {
			VkResult result{ m_device->vkBindBufferMemory((*m_device)(), m_buffer, deviceMemory(), 0) };
			if (result != VK_SUCCESS) {
				std::cerr << "Error: Could not bind memory for buffer, " << toString(result) << std::endl;
				return false;
			}

			return true;
		}

		uint32_t Buffer::getSize() const {
			return m_size;
		}

		VkMemoryRequirements Buffer::getMemoryRequirements() const {
			VkMemoryRequirements requirements;

			m_device->vkGetBufferMemoryRequirements((*m_device)(), m_buffer, &requirements);

			return requirements;
		}

		VkBuffer Buffer::operator()() const {
			return m_buffer;
		}
	}
}

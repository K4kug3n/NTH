#include <Renderer/Vulkan/DeviceMemory.hpp>

#include <Renderer/Vulkan/Device.hpp>
#include <Renderer/Vulkan/VkUtils.hpp>

#include <iostream>

namespace Nth {
	namespace Vk {
		DeviceMemory::DeviceMemory() :
			m_device_memory(VK_NULL_HANDLE),
			m_device(nullptr),
			m_memory_pointer(nullptr) {
		}

		DeviceMemory::DeviceMemory(DeviceMemory&& object) noexcept :
			m_device(object.m_device),
			m_device_memory(object.m_device_memory),
			m_memory_pointer(object.m_memory_pointer){
			object.m_device_memory = VK_NULL_HANDLE;
		}

		DeviceMemory::~DeviceMemory() {
			destroy();
		}

		void DeviceMemory::create(const Device& device, const VkMemoryAllocateInfo& infos) {
			assert(m_device == nullptr);
			VkResult result{ device.vkAllocateMemory(device(), &infos, nullptr, &m_device_memory) };
			if (result != VK_SUCCESS) {
				throw std::runtime_error("Can't create DeviceMemory, " + to_string(result));
			}

			m_device = &device;
		}

		void DeviceMemory::map(VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags) {
			void* memory_pointer;
			VkResult result{ m_device->vkMapMemory((*m_device)(), m_device_memory, offset, size, flags, &memory_pointer) };
			if (result != VK_SUCCESS) {
				throw std::runtime_error("Can't map memory," + to_string(result));
			}

			m_memory_pointer = memory_pointer;
		}

		void DeviceMemory::flush_mapped_memory(VkDeviceSize offset, VkDeviceSize size) const {
			VkMappedMemoryRange flush_range = {
				VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,  // VkStructureType        sType
				nullptr,                                // const void            *pNext
				m_device_memory,                        // VkDeviceMemory         memory
				offset,                                 // VkDeviceSize           offset
				size                                    // VkDeviceSize           size
			};

			VkResult result{ m_device->vkFlushMappedMemoryRanges((*m_device)(), 1, &flush_range) };
			if (result != VK_SUCCESS) {
				throw std::runtime_error("Can't flush mapped memory, " + to_string(result));
			}
		}

		void DeviceMemory::unmap() {
			m_device->vkUnmapMemory((*m_device)(), m_device_memory);

			m_memory_pointer = nullptr;
		}

		void DeviceMemory::destroy() {
			if (m_device_memory != VK_NULL_HANDLE) {
				m_device->vkFreeMemory((*m_device)(), m_device_memory, nullptr);
				m_device_memory = VK_NULL_HANDLE;
			}
		}

		void* DeviceMemory::get_mapped_pointer() const {
			return m_memory_pointer;
		}

		VkDeviceMemory DeviceMemory::operator()() const {
			return m_device_memory;
		}

		DeviceMemory& DeviceMemory::operator=(DeviceMemory&& object) noexcept {
			destroy();

			m_device_memory = object.m_device_memory;
			m_device = object.m_device;
			m_memory_pointer = object.m_memory_pointer;

			object.m_device_memory = VK_NULL_HANDLE;

			return *this;
		}
	}
}

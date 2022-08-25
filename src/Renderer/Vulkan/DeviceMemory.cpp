#include <Renderer/Vulkan/DeviceMemory.hpp>

#include <Renderer/Vulkan/Device.hpp>
#include <Renderer/Vulkan/VkUtil.hpp>

#include <iostream>

namespace Nth {
	namespace Vk {
		DeviceMemory::DeviceMemory() :
			m_deviceMemory(VK_NULL_HANDLE),
			m_device(nullptr),
			m_memoryPointer(nullptr) {
		}

		DeviceMemory::DeviceMemory(DeviceMemory&& object) noexcept :
			m_device(object.m_device),
			m_deviceMemory(object.m_deviceMemory),
			m_memoryPointer(object.m_memoryPointer){
			object.m_deviceMemory = VK_NULL_HANDLE;
		}

		DeviceMemory::~DeviceMemory() {
			destroy();
		}

		bool DeviceMemory::create(Device const& device, VkMemoryAllocateInfo const& infos) {
			VkResult result{ device.vkAllocateMemory(device(), &infos, nullptr, &m_deviceMemory) };
			if (result != VK_SUCCESS) {
				return false;
			}

			m_device = &device;

			return true;
		}

		bool DeviceMemory::map(VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags) {
			void* memoryPointer;
			VkResult result{ m_device->vkMapMemory((*m_device)(), m_deviceMemory, offset, size, flags, &memoryPointer) };
			if (result != VK_SUCCESS) {
				std::cerr << "Error: Can't map memory, " << toString(result) << std::endl;
				return false;
			}

			m_memoryPointer = memoryPointer;

			return true;
		}

		bool DeviceMemory::flushMappedMemory(VkDeviceSize offset, VkDeviceSize size) const {
			VkMappedMemoryRange flushRange = {
				VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,  // VkStructureType        sType
				nullptr,                                // const void            *pNext
				m_deviceMemory,                         // VkDeviceMemory         memory
				offset,                                 // VkDeviceSize           offset
				size                                    // VkDeviceSize           size
			};

			VkResult result{ m_device->vkFlushMappedMemoryRanges((*m_device)(), 1, &flushRange) };
			if (result != VK_SUCCESS) {
				std::cerr << "Error: Can't flush mapped memory, " << toString(result) << std::endl;
				return false;
			}

			return true;
		}

		void DeviceMemory::unmap() {
			m_device->vkUnmapMemory((*m_device)(), m_deviceMemory);

			m_memoryPointer = nullptr;
		}

		void DeviceMemory::destroy() {
			if (m_deviceMemory != VK_NULL_HANDLE) {
				m_device->vkFreeMemory((*m_device)(), m_deviceMemory, nullptr);
				m_deviceMemory = VK_NULL_HANDLE;
			}
		}

		void* DeviceMemory::getMappedPointer() const {
			return m_memoryPointer;
		}

		VkDeviceMemory DeviceMemory::operator()() const {
			return m_deviceMemory;
		}

		DeviceMemory& DeviceMemory::operator=(DeviceMemory&& object) noexcept {
			destroy();

			m_deviceMemory = object.m_deviceMemory;
			m_device = object.m_device;
			m_memoryPointer = object.m_memoryPointer;

			object.m_deviceMemory = VK_NULL_HANDLE;

			return *this;
		}
	}
}

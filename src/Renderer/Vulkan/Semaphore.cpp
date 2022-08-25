#include <Renderer/Vulkan/Semaphore.hpp>

#include <Renderer/Vulkan/Device.hpp>

#include <iostream>

namespace Nth {
	namespace Vk {
		Semaphore::Semaphore() :
			m_sempahore(VK_NULL_HANDLE),
			m_device(nullptr) {
		}

		Semaphore::Semaphore(Semaphore&& object) noexcept :
			m_sempahore(object.m_sempahore),
			m_device(object.m_device) {
			object.m_sempahore = VK_NULL_HANDLE;
		}

		Semaphore::~Semaphore() {
			if (m_sempahore != VK_NULL_HANDLE) {
				m_device->vkDestroySemaphore((*m_device)(), m_sempahore, nullptr);
			}
		}

		bool Semaphore::create(Device const& device) {
			VkSemaphoreCreateInfo semaphoreCreateInfo = {
				VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,      // VkStructureType          sType
				nullptr,                                      // const void*              pNext
				0                                             // VkSemaphoreCreateFlags   flags
			};

			VkResult result{ device.vkCreateSemaphore(device(), &semaphoreCreateInfo, nullptr, &m_sempahore) };
			if (result != VK_SUCCESS) {
				std::cerr << "Error: Can't create semaphore" << std::endl;
				return false;
			}

			m_device = &device;

			return true;
		}

		VkSemaphore const& Semaphore::operator()() const {
			return m_sempahore;
		}
	}
}

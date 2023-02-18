#include <Renderer/Vulkan/Semaphore.hpp>

#include <Renderer/Vulkan/Device.hpp>
#include <Renderer/Vulkan/VkUtils.hpp>

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

		void Semaphore::create(const Device& device) {
			VkSemaphoreCreateInfo semaphore_create_info = {
				VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,      // VkStructureType          sType
				nullptr,                                      // const void*              pNext
				0                                             // VkSemaphoreCreateFlags   flags
			};

			VkResult result{ device.vkCreateSemaphore(device(), &semaphore_create_info, nullptr, &m_sempahore) };
			if (result != VK_SUCCESS) {
				throw std::runtime_error("Can't create semaphore, " + to_string(result));
			}

			m_device = &device;
		}

		VkSemaphore Semaphore::operator()() const {
			return m_sempahore;
		}

		Semaphore& Semaphore::operator=(Semaphore&& object) noexcept {
			m_sempahore = object.m_sempahore;
			m_device = object.m_device;
			object.m_sempahore = VK_NULL_HANDLE;

			return *this;
		}
	}
}

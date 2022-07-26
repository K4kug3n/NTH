#include "Renderer/Vulkan/DescriptorPool.hpp"

#include "Renderer/Vulkan/Device.hpp"
#include "Renderer/Vulkan/VkUtil.hpp"

#include <iostream>
#include <cassert>

namespace Nth {
	namespace Vk {
		DescriptorPool::DescriptorPool() :
			m_descriptorPool(VK_NULL_HANDLE),
			m_device(nullptr) {
		}

		DescriptorPool::DescriptorPool(DescriptorPool&& object) noexcept :
			m_descriptorPool(object.m_descriptorPool),
			m_device(object.m_device) {
			object.m_descriptorPool = VK_NULL_HANDLE;
		}

		DescriptorPool::~DescriptorPool() {
			destroy();
		}

		bool DescriptorPool::create(Device const& device, VkDescriptorPoolCreateInfo const& info) {
			VkResult result{ device.vkCreateDescriptorPool(device(), &info, nullptr, &m_descriptorPool) };
			if (result != VK_SUCCESS) {
				std::cerr << "Error: Can't create Descriptor Pool, " << toString(result) << std::endl;
				return false;
			}

			m_device = &device;

			return true;
		}

		bool DescriptorPool::reset() const {
			assert(m_device != nullptr);

			VkResult result{ m_device->vkResetDescriptorPool((*m_device)(), m_descriptorPool, 0) };
			if (result != VK_SUCCESS) {
				std::cerr << "Error: Can't reset Descriptor Pool, " << toString(result) << std::endl;
				return false;
			}

			return true;
		}

		void DescriptorPool::destroy() {
			if (m_descriptorPool != VK_NULL_HANDLE) {
				m_device->vkDestroyDescriptorPool((*m_device)(), m_descriptorPool, nullptr);
				m_descriptorPool = VK_NULL_HANDLE;
			}
		}

		VkDescriptorPool DescriptorPool::operator()() const {
			return m_descriptorPool;
		}

		DescriptorPool& DescriptorPool::operator=(DescriptorPool&& object) noexcept {
			destroy();

			m_descriptorPool = object.m_descriptorPool;
			m_device = object.m_device;

			object.m_descriptorPool = VK_NULL_HANDLE;

			return *this;
		}
	}
}

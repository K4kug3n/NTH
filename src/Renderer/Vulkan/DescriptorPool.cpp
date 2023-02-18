#include <Renderer/Vulkan/DescriptorPool.hpp>

#include <Renderer/Vulkan/Device.hpp>
#include <Renderer/Vulkan/VkUtils.hpp>

#include <iostream>
#include <cassert>

namespace Nth {
	namespace Vk {
		DescriptorPool::DescriptorPool() :
			m_descriptor_pool(VK_NULL_HANDLE),
			m_device(nullptr) {
		}

		DescriptorPool::DescriptorPool(DescriptorPool&& object) noexcept :
			m_descriptor_pool(object.m_descriptor_pool),
			m_device(object.m_device) {
			object.m_descriptor_pool = VK_NULL_HANDLE;
		}

		DescriptorPool::~DescriptorPool() {
			destroy();
		}

		void DescriptorPool::create(const Device& device, const VkDescriptorPoolCreateInfo& info) {
			VkResult result{ device.vkCreateDescriptorPool(device(), &info, nullptr, &m_descriptor_pool) };
			if (result != VK_SUCCESS) {
				throw std::runtime_error("Can't create Descriptor Pool, " + to_string(result));
			}

			m_device = &device;
		}

		void DescriptorPool::reset() const {
			assert(m_device != nullptr);

			VkResult result{ m_device->vkResetDescriptorPool((*m_device)(), m_descriptor_pool, 0) };
			if (result != VK_SUCCESS) {
				throw std::runtime_error("Can't reset Descriptor Pool, " + to_string(result));
			}
		}

		void DescriptorPool::destroy() {
			if (m_descriptor_pool != VK_NULL_HANDLE) {
				m_device->vkDestroyDescriptorPool((*m_device)(), m_descriptor_pool, nullptr);
				m_descriptor_pool = VK_NULL_HANDLE;
			}
		}

		VkDescriptorPool DescriptorPool::operator()() const {
			return m_descriptor_pool;
		}

		DescriptorPool& DescriptorPool::operator=(DescriptorPool&& object) noexcept {
			destroy();

			m_descriptor_pool = object.m_descriptor_pool;
			m_device = object.m_device;

			object.m_descriptor_pool = VK_NULL_HANDLE;

			return *this;
		}
	}
}

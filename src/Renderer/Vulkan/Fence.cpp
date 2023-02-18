#include <Renderer/Vulkan/Fence.hpp>

#include <Renderer/Vulkan/Device.hpp>
#include <Renderer/Vulkan/VkUtils.hpp>

#include <iostream>
#include <cassert>

namespace Nth {
	namespace Vk {
		Fence::Fence() :
			m_fence(VK_NULL_HANDLE),
			m_device(nullptr) {
		}

		Fence::Fence(Fence&& object) noexcept :
			m_fence(object.m_fence),
			m_device(object.m_device) {
			object.m_fence = VK_NULL_HANDLE;
		}

		Fence::~Fence() {
			if (m_fence != VK_NULL_HANDLE) {
				m_device->vkDestroyFence((*m_device)(), m_fence, nullptr);
			}
		}

		void Fence::create(const Device& device, VkFenceCreateFlags flags) {
			VkFenceCreateInfo fence_create_info = {
				VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, // VkStructureType                sType
				nullptr,                             // const void                    *pNext
				flags                                // VkFenceCreateFlags             flags
			};

			VkResult result{ device.vkCreateFence(device(), &fence_create_info, nullptr, &m_fence) };
			if (result != VK_SUCCESS) {
				throw std::runtime_error("Can't create fence, " + to_string(result));
			}

			m_device = &device;
		}

		void Fence::wait(uint64_t timeout) const {
			assert(m_device != nullptr);
			VkResult result{ m_device->vkWaitForFences((*m_device)(), 1, &m_fence, VK_FALSE, timeout) };
			if (result != VK_SUCCESS) {
				throw std::runtime_error("Waiting fence took too long, " + to_string(result));
			}
		}

		void Fence::reset() const {
			VkResult result{ m_device->vkResetFences((*m_device)(), 1, &m_fence) };
			if (result != VK_SUCCESS) {
				throw std::runtime_error("Can't reset fence, " + to_string(result));
			}
		}

		VkFence Fence::operator()() const {
			return m_fence;
		}

		Fence& Fence::operator=(Fence&& object) noexcept {
			m_fence = object.m_fence;
			m_device = object.m_device;

			object.m_fence = VK_NULL_HANDLE;

			return *this;
		}
	}
}

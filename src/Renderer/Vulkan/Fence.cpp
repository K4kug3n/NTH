#include <Renderer/Vulkan/Fence.hpp>

#include <Renderer/Vulkan/Device.hpp>
#include <Renderer/Vulkan/VkUtil.hpp>

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

		bool Fence::create(Device const& device, VkFenceCreateFlags flags) {
			VkFenceCreateInfo fenceCreateInfo = {
				VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, // VkStructureType                sType
				nullptr,                             // const void                    *pNext
				flags         // VkFenceCreateFlags             flags
			};

			VkResult result{ device.vkCreateFence(device(), &fenceCreateInfo, nullptr, &m_fence) };
			if (result != VK_SUCCESS) {
				std::cerr << "Error: Can't create fence, " << toString(result) << std::endl;
				return false;
			}

			m_device = &device;

			return true;
		}

		bool Fence::wait(uint64_t timeout) const {
			assert(m_device != nullptr);
			VkResult result{ m_device->vkWaitForFences((*m_device)(), 1, &m_fence, VK_FALSE, timeout) };
			if (result != VK_SUCCESS) {
				std::cerr << "Error: Waiting fence take too long, " << toString(result) << std::endl;
				return false;
			}

			return true;
		}

		bool Fence::reset() const {
			VkResult result{ m_device->vkResetFences((*m_device)(), 1, &m_fence) };
			if (result != VK_SUCCESS) {
				std::cerr << "Error: Can't reset fence, " << toString(result) << std::endl;
				return false;
			}

			return true;
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

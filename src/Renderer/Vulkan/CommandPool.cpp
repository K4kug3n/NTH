#include <Renderer/Vulkan/CommandPool.hpp>

#include <Renderer/Vulkan/Device.hpp>
#include <Renderer/Vulkan/VkUtil.hpp>
#include <Renderer/Vulkan/CommandBuffer.hpp>

#include <iostream>

namespace Nth {
	namespace Vk {
		CommandPool::CommandPool() :
			m_commandPool(VK_NULL_HANDLE),
			m_device(nullptr) {
		}

		CommandPool::~CommandPool() {
			destroy();
		}

		bool CommandPool::create(Device const& device, uint32_t index, VkCommandPoolCreateFlags flags) {
			VkCommandPoolCreateInfo cmdPoolCreateInfo = {
				VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,     // VkStructureType              sType
				nullptr,                                        // const void*                  pNext
				flags,                                          // VkCommandPoolCreateFlags     flags
				index                                           // uint32_t                     queueFamilyIndex
			};

			VkResult result{ device.vkCreateCommandPool(device(), &cmdPoolCreateInfo, nullptr, &m_commandPool) };
			if (result != VK_SUCCESS) {
				std::cerr << "Error: Can't create command pool, " << toString(result) << std::endl;
				return false;
			}

			m_device = &device;

			return true;
		}

		void CommandPool::destroy() {
			if (m_commandPool != VK_NULL_HANDLE) {
				m_device->vkDestroyCommandPool((*m_device)(), m_commandPool, nullptr);
				m_commandPool = VK_NULL_HANDLE;
			}
		}

		bool CommandPool::allocateCommandBuffer(VkCommandBufferLevel level, CommandBuffer& commandBuffer) const {
			VkCommandBufferAllocateInfo cmdBufferAllocateInfo = {
				VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, // VkStructureType              sType
				nullptr,                                        // const void*                  pNext
				m_commandPool,                                  // VkCommandPool                commandPool
				level,                                          // VkCommandBufferLevel         level
				1                                               // uint32_t                     bufferCount
			};

			VkCommandBuffer vkBuffer = VK_NULL_HANDLE;
			VkResult result{ m_device->vkAllocateCommandBuffers((*m_device)(), &cmdBufferAllocateInfo, &vkBuffer) };
			if (result != VK_SUCCESS) {
				std::cerr << "Error: Could not allocate command buffers, " << toString(result) << std::endl;
				return false;
			}

			commandBuffer = CommandBuffer(this, vkBuffer);

			return true;
		}

		bool CommandPool::reset() const {
			VkResult result{ m_device->vkResetCommandPool((*m_device)(), m_commandPool, 0) };

			if (result != VK_SUCCESS) {
				std::cerr << "Error: Can't reset command pool, " << toString(result) << std::endl;
				return false;
			}

			return true;
		}

		Device const* CommandPool::getDevice() const {
			return m_device;
		}

		VkCommandPool const& CommandPool::operator()() const {
			return m_commandPool;
		}
	}
}

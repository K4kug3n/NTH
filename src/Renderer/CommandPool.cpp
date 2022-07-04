#include "Renderer/CommandPool.hpp"

#include "Renderer/Device.hpp"
#include "Renderer/VkUtil.hpp"
#include "Renderer/CommandBuffer.hpp"

#include <iostream>

namespace Nth {
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
		}
	}

	std::vector<CommandBuffer> CommandPool::allocateCommandBuffer(VkCommandBufferLevel level, uint32_t count) const {
		VkCommandBufferAllocateInfo cmdBufferAllocateInfo = {
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, // VkStructureType              sType
			nullptr,                                        // const void*                  pNext
			m_commandPool,                                  // VkCommandPool                commandPool
			level,                                          // VkCommandBufferLevel         level
			count                                           // uint32_t                     bufferCount
		};

		std::vector<VkCommandBuffer> vkBuffers(count, VK_NULL_HANDLE);
		VkResult result{ m_device->vkAllocateCommandBuffers((*m_device)(), &cmdBufferAllocateInfo, &vkBuffers[0]) };
		if (result != VK_SUCCESS) {
			std::cerr << "Error: Could not allocate command buffers, " << toString(result) << std::endl;
			return std::vector<CommandBuffer>{};
		}

		std::vector<CommandBuffer> buffers;
		for (auto vkBuffer : vkBuffers) {
			buffers.push_back(CommandBuffer{ this, vkBuffer });
		}

		return buffers;
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

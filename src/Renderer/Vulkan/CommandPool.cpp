#include <Renderer/Vulkan/CommandPool.hpp>

#include <Renderer/Vulkan/Device.hpp>
#include <Renderer/Vulkan/VkUtils.hpp>
#include <Renderer/Vulkan/CommandBuffer.hpp>

#include <iostream>

namespace Nth {
	namespace Vk {
		CommandPool::CommandPool() :
			m_command_pool(VK_NULL_HANDLE),
			m_device(nullptr) {
		}

		CommandPool::CommandPool(CommandPool&& object) noexcept :
			m_command_pool(object.m_command_pool),
			m_device(object.m_device){
			object.m_command_pool = VK_NULL_HANDLE;
		}

		CommandPool::~CommandPool() {
			destroy();
		}

		void CommandPool::create(const Device& device, uint32_t family_index, VkCommandPoolCreateFlags flags) {
			VkCommandPoolCreateInfo cmd_pool_create_info = {
				VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,     // VkStructureType              sType
				nullptr,                                        // const void*                  pNext
				flags,                                          // VkCommandPoolCreateFlags     flags
				family_index                                           // uint32_t                     queueFamilyIndex
			};

			VkResult result{ device.vkCreateCommandPool(device(), &cmd_pool_create_info, nullptr, &m_command_pool) };
			if (result != VK_SUCCESS) {
				throw std::runtime_error("Error: Can't create command pool, " + to_string(result));
			}

			m_device = &device;
		}

		void CommandPool::destroy() {
			if (m_command_pool != VK_NULL_HANDLE) {
				m_device->vkDestroyCommandPool((*m_device)(), m_command_pool, nullptr);
				m_command_pool = VK_NULL_HANDLE;
			}
		}

		void CommandPool::allocate_command_buffer(VkCommandBufferLevel level, CommandBuffer& command_buffer) const {
			VkCommandBufferAllocateInfo cmd_buffer_allocate_info = {
				VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, // VkStructureType              sType
				nullptr,                                        // const void*                  pNext
				m_command_pool,                                  // VkCommandPool                commandPool
				level,                                          // VkCommandBufferLevel         level
				1                                               // uint32_t                     bufferCount
			};

			VkCommandBuffer vkBuffer = VK_NULL_HANDLE;
			VkResult result{ m_device->vkAllocateCommandBuffers((*m_device)(), &cmd_buffer_allocate_info, &vkBuffer) };
			if (result != VK_SUCCESS) {
				throw std::runtime_error("Could not allocate command buffers, " + to_string(result));
			}

			command_buffer = CommandBuffer(this, vkBuffer);
		}

		void CommandPool::reset() const {
			VkResult result{ m_device->vkResetCommandPool((*m_device)(), m_command_pool, 0) };

			if (result != VK_SUCCESS) {
				throw std::runtime_error("Can't reset command pool, " + to_string(result));
			}
		}

		Device const* CommandPool::get_device() const {
			return m_device;
		}

		VkCommandPool CommandPool::operator()() const {
			return m_command_pool;
		}

		CommandPool& CommandPool::operator=(CommandPool&& object) noexcept {
			m_command_pool = object.m_command_pool;
			m_device = object.m_device;
			object.m_command_pool = VK_NULL_HANDLE;

			return *this;
		}
	}
}

#ifndef NTH_RENDERER_VK_COMMANDPOOL_HPP
#define NTH_RENDERER_VK_COMMANDPOOL_HPP

#include <vulkan/vulkan.h>

#include <vector>

namespace Nth {
	namespace Vk {
		class Device;
		class CommandBuffer;

		class CommandPool {
		public:
			CommandPool();
			CommandPool(const CommandPool&) = delete;
			CommandPool(CommandPool&& object) noexcept;
			~CommandPool();

			void allocate_command_buffer(VkCommandBufferLevel level, CommandBuffer& command_buffer) const;

			void create(const Device& device, uint32_t family_index, VkCommandPoolCreateFlags flags);
			void destroy();

			void reset() const;

			Device const* get_device() const;
			VkCommandPool operator()() const;

			CommandPool& operator=(const CommandPool&) = delete;
			CommandPool& operator=(CommandPool&& object) noexcept;

		private:
			VkCommandPool m_command_pool;
			Device const* m_device;
		};
	}
}

#endif

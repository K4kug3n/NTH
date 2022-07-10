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
			CommandPool(CommandPool const&) = delete;
			CommandPool(CommandPool&&) = delete;
			~CommandPool();

			bool create(Device const& device, uint32_t index, VkCommandPoolCreateFlags flags);
			void destroy();

			bool allocateCommandBuffer(VkCommandBufferLevel level, CommandBuffer& commandBuffer) const;
			bool reset() const;

			Device const* getDevice() const;
			VkCommandPool const& operator()() const;

			CommandPool& operator=(CommandPool const&) = delete;
			CommandPool& operator=(CommandPool&&) = delete;

		private:
			VkCommandPool m_commandPool;
			Device const* m_device;
		};
	}
}

#endif

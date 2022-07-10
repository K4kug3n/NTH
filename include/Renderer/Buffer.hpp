#ifndef NTH_RENDERER_VK_BUFFER_HPP
#define NTH_RENDERER_VK_BUFFER_HPP

#include <vulkan/vulkan.h>

namespace Nth {
	namespace Vk {
		class Device;
		class DeviceMemory;

		class Buffer {
		public:
			Buffer();
			Buffer(Buffer const&) = delete;
			Buffer(Buffer&&) = delete;
			~Buffer();

			bool create(Device const& device, VkBufferCreateInfo const& infos);
			bool bindBufferMemory(DeviceMemory const& deviceMemory) const;

			uint32_t getSize() const;

			VkMemoryRequirements getMemoryRequirements() const;

			VkBuffer operator()() const;

			Buffer& operator=(Buffer const&) = delete;
			Buffer& operator=(Buffer&&) = delete;

		private:
			VkBuffer m_buffer;
			Device const* m_device;
			uint32_t m_size;
		};
	}
}

#endif

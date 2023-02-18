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
			Buffer(const Buffer&) = delete;
			Buffer(Buffer&& object) noexcept;
			~Buffer();

			void bind_buffer_memory(const DeviceMemory& device_memory) const;
			void create(const Device& device, const VkBufferCreateInfo& infos);
			
			uint32_t get_size() const;

			VkMemoryRequirements get_memory_requirements() const;

			VkBuffer operator()() const;

			Buffer& operator=(const Buffer&) = delete;
			Buffer& operator=(Buffer&& object) noexcept;

		private:
			VkBuffer m_buffer;
			Device const* m_device;
			uint32_t m_size;
		};
	}
}

#endif

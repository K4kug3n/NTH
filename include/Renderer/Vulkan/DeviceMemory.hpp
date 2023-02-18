#ifndef NTH_RENDERER_VK_DEVICEMEMORY_HPP
#define NTH_RENDERER_VK_DEVICEMEMORY_HPP

#include <vulkan/vulkan.h>

namespace Nth {
	namespace Vk {
		class Device;

		class DeviceMemory {
		public:
			DeviceMemory();
			DeviceMemory(const DeviceMemory&) = delete;
			DeviceMemory(DeviceMemory&& object) noexcept;
			~DeviceMemory();

			void create(const Device& device, const VkMemoryAllocateInfo& infos);
			void destroy();
			void flush_mapped_memory(VkDeviceSize offset, VkDeviceSize size) const;
			void* get_mapped_pointer() const;
			void map(VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags);
			void unmap();

			VkDeviceMemory operator()() const;

			DeviceMemory& operator=(const DeviceMemory&) = delete;
			DeviceMemory& operator=(DeviceMemory&& object) noexcept;

		private:
			VkDeviceMemory m_device_memory;
			Device const* m_device;
			void* m_memory_pointer;
		};
	}
}

#endif

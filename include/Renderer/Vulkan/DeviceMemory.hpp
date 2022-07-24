#ifndef NTH_RENDERER_VK_DEVICEMEMORY_HPP
#define NTH_RENDERER_VK_DEVICEMEMORY_HPP

#include <vulkan/vulkan.h>

namespace Nth {
	namespace Vk {
		class Device;

		class DeviceMemory {
		public:
			DeviceMemory();
			DeviceMemory(DeviceMemory const&) = delete;
			DeviceMemory(DeviceMemory&&) = delete;
			~DeviceMemory();

			bool create(Device const& device, VkMemoryAllocateInfo const& infos);
			bool map(VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags);
			bool flushMappedMemory(VkDeviceSize offset, VkDeviceSize size) const;
			void unmap();
			void destroy();

			void* getMappedPointer() const;

			VkDeviceMemory operator()() const;

			DeviceMemory& operator=(DeviceMemory const&) = delete;
			DeviceMemory& operator=(DeviceMemory&& object) noexcept;

		private:
			VkDeviceMemory m_deviceMemory;
			Device const* m_device;
			void* m_memoryPointer;
		};
	}
}

#endif

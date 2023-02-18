#ifndef NTH_RENDERER_VK_IMAGE_HPP
#define NTH_RENDERER_VK_IMAGE_HPP

#include <vulkan/vulkan.h>

namespace Nth {
	namespace Vk {
		class Device;
		class DeviceMemory;

		class Image {
		public:
			Image();
			Image(const Image&) = delete;
			Image(Image&& object) noexcept;
			~Image();

			void bind_image_memory(const DeviceMemory& memory);
			void create(const Device& device, const VkImageCreateInfo& infos);
			void destroy();
			
			VkMemoryRequirements get_image_memory_requirements() const;

			VkImage operator()() const;

			Image& operator=(const Image&) = delete;
			Image& operator=(Image&& object) noexcept;

		private:
			VkImage m_image;
			Device const* m_device;
		};
	}
}

#endif

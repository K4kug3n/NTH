#ifndef NTH_RENDERER_VK_IMAGEVIEW_HPP
#define NTH_RENDERER_VK_IMAGEVIEW_HPP

#include <vulkan/vulkan.h>

namespace Nth {
	namespace Vk {
		class Device;

		class ImageView {
		public:
			ImageView();
			ImageView(const ImageView&) = delete;
			ImageView(ImageView&& object) noexcept;
			~ImageView();

			void create(const Device& device, const VkImageViewCreateInfo& infos);
			void destroy();

			VkImageView operator()() const;

			ImageView& operator=(const ImageView&) = delete;
			ImageView& operator=(ImageView&& object) noexcept;

		private:
			VkImageView m_image_view;
			Device const* m_device;
		};
	}
}

#endif

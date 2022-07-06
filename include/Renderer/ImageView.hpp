#pragma once

#ifndef NTH_IMAGEVIEW_HPP
#define NTH_IMAGEVIEW_HPP

#include <vulkan/vulkan.h>

namespace Nth {
	class Device;

	class ImageView {
	public:
		ImageView();
		ImageView(ImageView const&) = delete;
		ImageView(ImageView&& object) noexcept;
		~ImageView();

		bool create(Device const& device, VkImageViewCreateInfo const& infos);
		void destroy();

		VkImageView const& operator()() const;

		ImageView& operator=(ImageView const&) = delete;
		ImageView& operator=(ImageView&& object) noexcept;

	private:
		VkImageView m_imageView;
		Device const* m_device;
	};
}

#endif

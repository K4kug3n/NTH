#pragma once

#ifndef NTH_IMAGE_HPP
#define NTH_IMAGE_HPP

#include <vulkan/vulkan.h>

namespace Nth {
	class Device;
	class DeviceMemory

	class Image {
	public:
		Image();
		Image(Image const&) = delete;
		Image(Image&& object) noexcept;
		~Image();

		bool create(Device const& device, VkImageCreateInfo const& infos);
		void destroy();
		bool bindImageMemory(DeviceMemory const& memory);

		VkMemoryRequirements getImageMemoryRequirements() const;

		VkImage const& operator()() const;

		Image& operator=(Image const&) = delete;
		Image& operator=(Image&&) = delete;

	private:
		VkImage m_image;
		Device const* m_device;
	};
}

#endif

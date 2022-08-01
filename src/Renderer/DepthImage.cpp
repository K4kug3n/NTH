#include "Renderer/DepthImage.hpp"

#include "Renderer/Vulkan/Device.hpp"
#include "Renderer/Vulkan/PhysicalDevice.hpp"
#include "Math/Vector2.hpp"

#include <iostream>

namespace Nth {
	bool DepthImage::create(Vk::Device const& device, Vector2<unsigned int> const& size) {
		m_format = findDepthFormat(device);

		if (!m_image.create(
			device,
			size.x,
			size.y,
			m_format,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)) {
			std::cerr << "Can't create depth image" << std::endl;
			return false;
		}

		if (!m_image.createView(device, m_format, VK_IMAGE_ASPECT_DEPTH_BIT)) {
			std::cerr << "Can't create depth image view" << std::endl;
			return false;
		}

		return true;
	}

	Vk::ImageView const& DepthImage::view() const {
		return m_image.view;
	}

	Vk::Image const& DepthImage::image() const {
		return m_image.handle;
	}

	VkFormat DepthImage::format() const {
		return m_format;
	}

	VkFormat DepthImage::findDepthFormat(Vk::Device const& device) const {
		return findSupportedFormat(
			device,
			{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
		);
	}

	VkFormat DepthImage::findSupportedFormat(Vk::Device const& device, std::vector<VkFormat> const& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const {
		Vk::PhysicalDevice const& physicalDevice = device.getPhysicalDevice();

		for (VkFormat format : candidates) {
			VkFormatProperties props = physicalDevice.getFormatProperties(format);

			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
				return format;
			}
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
				return format;
			}
		}

		throw std::runtime_error("Canno't find supported format!");
	}

	bool DepthImage::hasStencilComponent(VkFormat format) const {
		return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
	}
}

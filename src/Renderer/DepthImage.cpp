#include <Renderer/DepthImage.hpp>

#include <Renderer/Vulkan/Device.hpp>
#include <Renderer/Vulkan/PhysicalDevice.hpp>
#include <Renderer/RenderDevice.hpp>
#include <Maths/Vector2.hpp>

#include <iostream>

namespace Nth {
	void DepthImage::create(const RenderDevice& device, const Vector2<unsigned int>& size) {
		m_format = find_depth_format(device.get_handle());

		m_image.create(
			device,
			size.x,
			size.y,
			0,
			m_format,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		m_image.create_view(m_format, VK_IMAGE_ASPECT_DEPTH_BIT);
	}

	const Vk::ImageView& DepthImage::view() const {
		return m_image.view;
	}

	const Vk::Image& DepthImage::image() const {
		return m_image.handle;
	}

	VkFormat DepthImage::format() const {
		return m_format;
	}

	VkFormat DepthImage::find_depth_format(const Vk::Device& device) const {
		return find_supported_format(
			device,
			{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
		);
	}

	VkFormat DepthImage::find_supported_format(const Vk::Device& device, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const {
		const Vk::PhysicalDevice& physicalDevice = device.get_physical_device();

		for (VkFormat format : candidates) {
			VkFormatProperties props = physicalDevice.get_format_properties(format);

			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
				return format;
			}
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
				return format;
			}
		}

		throw std::runtime_error("Canno't find supported format!");
	}

	bool DepthImage::has_stencil_component(VkFormat format) const {
		return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
	}
}

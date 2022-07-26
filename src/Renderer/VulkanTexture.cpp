#include "Renderer/VulkanTexture.hpp"

#include "Renderer/Vulkan/Device.hpp"

#include <iostream>

namespace Nth {
	bool VulkanTexture::create(Vk::Device const& device, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties) {
		if (!image.create(device, width, height, format, tiling, usage, properties)) {
			return false;
		}

		return createSampler(device);
	}

	bool VulkanTexture::createView(Vk::Device const& device, VkFormat format, VkImageAspectFlags aspectFlags) {
		return image.createView(device, format, aspectFlags);
	}

	bool VulkanTexture::createSampler(Vk::Device const& device) {
		VkSamplerCreateInfo samplerCreateInfo = {
			VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,  // VkStructureType        sType
			nullptr,                                // const void*            pNext
			0,                                      // VkSamplerCreateFlags   flags
			VK_FILTER_LINEAR,                       // VkFilter               magFilter
			VK_FILTER_LINEAR,                       // VkFilter               minFilter
			VK_SAMPLER_MIPMAP_MODE_NEAREST,         // VkSamplerMipmapMode    mipmapMode
			VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,  // VkSamplerAddressMode   addressModeU
			VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,  // VkSamplerAddressMode   addressModeV
			VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,  // VkSamplerAddressMode   addressModeW
			0.0f,                                   // float                  mipLodBias
			VK_FALSE,                               // VkBool32               anisotropyEnable
			1.0f,                                   // float                  maxAnisotropy
			VK_FALSE,                               // VkBool32               compareEnable
			VK_COMPARE_OP_ALWAYS,                   // VkCompareOp            compareOp
			0.0f,                                   // float                  minLod
			0.0f,                                   // float                  maxLod
			VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK,// VkBorderColor          borderColor
			VK_FALSE                                // VkBool32               unnormalizedCoordinates
		};

		return sampler.create(device, samplerCreateInfo);
	}
}

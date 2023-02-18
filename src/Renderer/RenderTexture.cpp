#include <Renderer/RenderTexture.hpp>

#include <Renderer/Vulkan/Device.hpp>
#include <Renderer/RenderDevice.hpp>

#include <iostream>

namespace Nth {
	void RenderTexture::create(const RenderDevice& device, uint32_t width, uint32_t height, size_t size, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties) {
		image.create(device, width, height, size, format, tiling, usage, properties);

		createSampler(device.get_handle());
	}

	void RenderTexture::createView(VkFormat format, VkImageAspectFlags aspectFlags) {
		image.create_view(format, aspectFlags);
	}

	void RenderTexture::createSampler(const Vk::Device& device) {
		VkSamplerCreateInfo sampler_create_info = {
			VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,         // VkStructureType        sType
			nullptr,                                       // const void*            pNext
			0,                                             // VkSamplerCreateFlags   flags
			VK_FILTER_LINEAR,                              // VkFilter               magFilter
			VK_FILTER_LINEAR,                              // VkFilter               minFilter
			VK_SAMPLER_MIPMAP_MODE_NEAREST,                // VkSamplerMipmapMode    mipmapMode
			VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,         // VkSamplerAddressMode   addressModeU
			VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,         // VkSamplerAddressMode   addressModeV
			VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,         // VkSamplerAddressMode   addressModeW
			0.0f,                                          // float                  mipLodBias
			VK_FALSE,                                      // VkBool32               anisotropyEnable
			1.0f,                                          // float                  maxAnisotropy
			VK_FALSE,                                      // VkBool32               compareEnable
			VK_COMPARE_OP_ALWAYS,                          // VkCompareOp            compareOp
			0.0f,                                          // float                  minLod
			0.0f,                                          // float                  maxLod
			VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK,       // VkBorderColor          borderColor
			VK_FALSE                                       // VkBool32               unnormalizedCoordinates
		};

		sampler.create(device, sampler_create_info);
	}
}

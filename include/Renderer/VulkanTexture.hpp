#ifndef NTH_RENDERER_VULKANTEXTURE_HPP
#define NTH_RENDERER_VULKANTEXTURE_HPP

#include "Renderer/VulkanImage.hpp"
#include "Renderer/Vulkan/Sampler.hpp"

namespace Nth {
	namespace Vk {
		class Device;
	}

	class VulkanTexture {
	public:
		VulkanTexture() = default;
		VulkanTexture(VulkanTexture const&) = delete;
		VulkanTexture(VulkanTexture&&) = default;
		~VulkanTexture() = default;

		bool create(Vk::Device const& device, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
		bool createView(Vk::Device const& device, VkFormat format, VkImageAspectFlags aspectFlags);

		VulkanImage image;
		Vk::Sampler sampler;

		VulkanTexture& operator=(VulkanTexture const&) = delete;
		VulkanTexture& operator=(VulkanTexture&&) = default;
	private:
		bool createSampler(Vk::Device const& device);
	};
}

#endif
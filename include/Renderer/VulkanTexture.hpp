#ifndef NTH_RENDERER_VULKANTEXTURE_HPP
#define NTH_RENDERER_VULKANTEXTURE_HPP

#include "Renderer/VulkanImage.hpp"
#include "Renderer/Vulkan/Sampler.hpp"

namespace Nth {
	class VulkanDevice;

	class VulkanTexture {
	public:
		VulkanTexture() = default;
		VulkanTexture(VulkanTexture const&) = delete;
		VulkanTexture(VulkanTexture&&) = default;
		~VulkanTexture() = default;

		void create(VulkanDevice const& device, uint32_t width, uint32_t height, size_t size, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
		void createView(VkFormat format, VkImageAspectFlags aspectFlags);

		VulkanImage image;
		Vk::Sampler sampler;

		VulkanTexture& operator=(VulkanTexture const&) = delete;
		VulkanTexture& operator=(VulkanTexture&&) = default;
	private:
		void createSampler(Vk::Device const& device);
	};
}

#endif
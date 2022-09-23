#ifndef NTH_RENDERER_RENDERTEXTURE_HPP
#define NTH_RENDERER_RENDERTEXTURE_HPP

#include <Renderer/RenderImage.hpp>
#include <Renderer/Vulkan/Sampler.hpp>
#include <Renderer/ShaderBinding.hpp>

namespace Nth {
	class RenderDevice;

	class RenderTexture {
	public:
		RenderTexture() = default;
		RenderTexture(RenderTexture const&) = delete;
		RenderTexture(RenderTexture&&) = default;
		~RenderTexture() = default;

		void create(RenderDevice const& device, uint32_t width, uint32_t height, size_t size, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
		void createView(VkFormat format, VkImageAspectFlags aspectFlags);

		RenderImage image;
		Vk::Sampler sampler;

		ShaderBinding binding;

		RenderTexture& operator=(RenderTexture const&) = delete;
		RenderTexture& operator=(RenderTexture&&) = default;
	private:
		void createSampler(Vk::Device const& device);
	};
}

#endif
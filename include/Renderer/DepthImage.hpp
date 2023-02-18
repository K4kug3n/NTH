#ifndef NTH_RENDERER_DEPTHIMAGE_HPP
#define NTH_RENDERER_DEPTHIMAGE_HPP

#include <Renderer/RenderImage.hpp>

#include <vector>

namespace Nth {
	template<typename T> class Vector2;
	class RenderDevice;

	class DepthImage {
	public:
		DepthImage() = default;
		DepthImage(const DepthImage&) = delete;
		DepthImage(DepthImage&&) = default;
		~DepthImage() = default;

		void create(const RenderDevice& device, const Vector2<unsigned int>& size);
		
		const Vk::ImageView& view() const;
		const Vk::Image& image() const;
		VkFormat format() const;

		DepthImage& operator=(const DepthImage&) = delete;
		DepthImage& operator=(DepthImage&&) = default;
	private:
		VkFormat find_depth_format(const Vk::Device& device) const;
		VkFormat find_supported_format(const Vk::Device& device, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;
		bool has_stencil_component(VkFormat format) const;

		RenderImage m_image;
		VkFormat m_format;
	};
}

#endif
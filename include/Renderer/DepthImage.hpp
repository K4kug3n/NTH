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
		DepthImage(DepthImage const&) = delete;
		DepthImage(DepthImage&&) = default;
		~DepthImage() = default;

		bool create(RenderDevice const& device, Vector2<unsigned int> const& size);
		
		Vk::ImageView const& view() const;
		Vk::Image const& image() const;
		VkFormat format() const;

		DepthImage& operator=(DepthImage const&) = delete;
		DepthImage& operator=(DepthImage&&) = default;
	private:
		VkFormat findDepthFormat(Vk::Device const& device) const;
		VkFormat findSupportedFormat(Vk::Device const& device, std::vector<VkFormat> const& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;
		bool hasStencilComponent(VkFormat format) const;

		RenderImage m_image;
		VkFormat m_format;
	};
}

#endif
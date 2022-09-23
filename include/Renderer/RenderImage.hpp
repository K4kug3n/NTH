#ifndef NTH_RENDERER_RENDERIMAGE_HPP
#define NTH_RENDERER_RENDERIMAGE_HPP

#include <Renderer/Vulkan/Image.hpp>
#include <Renderer/Vulkan/ImageView.hpp>
#include <Renderer/Vulkan/DeviceMemory.hpp>
#include <Renderer/Vulkan/Buffer.hpp>

namespace Nth {
	namespace Vk {
		class CommandBuffer;
		class Device;
	}

	class RenderDevice;

	class RenderImage {
	public:
		RenderImage() = default;
		RenderImage(RenderImage const&) = delete;
		RenderImage(RenderImage&&) = default;
		~RenderImage() = default;

		void create(RenderDevice const& device, uint32_t width, uint32_t height, size_t stagingSize, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
		void createView(VkFormat format, VkImageAspectFlags aspectFlags);

		void copy(void const* data, size_t size, uint32_t width, uint32_t height);

		RenderImage& operator=(RenderImage const&) = delete;
		RenderImage& operator=(RenderImage&&) = default;

		Vk::Image handle;
		Vk::ImageView view;
		Vk::DeviceMemory memory;
	private:
		uint32_t findMemoryType(Vk::Device const& device, uint32_t memoryTypeBit, VkMemoryPropertyFlags properties) const;
		void createStaging(Vk::Device const& device, size_t size);

		RenderDevice const* m_device;

		Vk::Buffer m_staging;
		Vk::DeviceMemory m_stagingMemory;
	};
}

#endif
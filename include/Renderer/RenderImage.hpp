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
		RenderImage(const RenderImage&) = delete;
		RenderImage(RenderImage&&) = default;
		~RenderImage() = default;

		void create(const RenderDevice& device, uint32_t width, uint32_t height, size_t staging_size, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
		void create_view(VkFormat format, VkImageAspectFlags aspect_flags);

		void copy(void const* data, size_t size, uint32_t width, uint32_t height);

		RenderImage& operator=(const RenderImage&) = delete;
		RenderImage& operator=(RenderImage&&) = default;

		Vk::Image handle;
		Vk::ImageView view;
		Vk::DeviceMemory memory;
	private:
		uint32_t find_memory_type(const Vk::Device& device, uint32_t memory_type_bit, VkMemoryPropertyFlags properties) const;
		void create_staging(const Vk::Device& device, size_t size);

		RenderDevice const* m_device;

		Vk::Buffer m_staging;
		Vk::DeviceMemory m_staging_memory;
	};
}

#endif
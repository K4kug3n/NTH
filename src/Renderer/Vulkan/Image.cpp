#include <Renderer/Vulkan/Image.hpp>

#include <Renderer/Vulkan/Device.hpp>
#include <Renderer/Vulkan/DeviceMemory.hpp>
#include <Renderer/Vulkan/VkUtils.hpp>

#include <iostream>
#include <cassert>

namespace Nth {
	namespace Vk {
		Image::Image() :
			m_image(VK_NULL_HANDLE),
			m_device(nullptr) {
		}

		Image::Image(Image&& object) noexcept :
			m_image(object.m_image),
			m_device(object.m_device) {
			object.m_image = VK_NULL_HANDLE;
		}

		Image::~Image() {
			destroy();
		}

		void Image::bind_image_memory(const DeviceMemory& memory) {
			assert(m_device != nullptr);

			VkResult result{ m_device->vkBindImageMemory((*m_device)(), m_image, memory(), 0) };
			if (result != VK_SUCCESS) {
				throw std::runtime_error("Could not bind memory to an image," + to_string(result));
			}
		}

		void Image::create(const Device& device, const VkImageCreateInfo& infos) {
			VkResult result{ device.vkCreateImage(device(), &infos, nullptr, &m_image) };
			if (result != VK_SUCCESS) {
				throw std::runtime_error("Can't create image, " + to_string(result));
			}

			m_device = &device;
		}

		void Image::destroy() {
			if (m_image != VK_NULL_HANDLE) {
				m_device->vkDestroyImage((*m_device)(), m_image, nullptr);
				m_image = VK_NULL_HANDLE;
			}
		}

		VkMemoryRequirements Image::get_image_memory_requirements() const {
			assert(m_device != nullptr);

			VkMemoryRequirements imageMemoryRequirements;
			m_device->vkGetImageMemoryRequirements((*m_device)(), m_image, &imageMemoryRequirements);

			return imageMemoryRequirements;
		}

		VkImage Image::operator()() const {
			return m_image;
		}

		Image& Image::operator=(Image&& object) noexcept {
			destroy();

			m_image = object.m_image;
			m_device = object.m_device;

			object.m_image = VK_NULL_HANDLE;

			return *this;
		}
	}
}

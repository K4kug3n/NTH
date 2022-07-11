#include "Renderer/Image.hpp"

#include "Renderer/Device.hpp"
#include "Renderer/DeviceMemory.hpp"
#include "Renderer/VkUtil.hpp"

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

		bool Image::create(Device const& device, VkImageCreateInfo const& infos) {
			VkResult result{ device.vkCreateImage(device(), &infos, nullptr, &m_image) };
			if (result != VK_SUCCESS) {
				std::cerr << "Error: Can't create image, " << toString(result) << std::endl;
				return false;
			}

			m_device = &device;

			return true;
		}

		void Image::destroy() {
			if (m_image != VK_NULL_HANDLE) {
				m_device->vkDestroyImage((*m_device)(), m_image, nullptr);
				m_image = VK_NULL_HANDLE;
			}
		}

		bool Image::bindImageMemory(DeviceMemory const& memory) {
			assert(m_device != nullptr);

			VkResult result{ m_device->vkBindImageMemory((*m_device)(), m_image, memory(), 0) };
			if (result != VK_SUCCESS) {
				std::cerr << "Error: Could not bind memory to an image," << toString(result) << std::endl;
				return false;
			}

			return true;
		}

		VkMemoryRequirements Image::getImageMemoryRequirements() const {
			assert(m_device != nullptr);

			VkMemoryRequirements imageMemoryRequirements;
			m_device->vkGetImageMemoryRequirements((*m_device)(), m_image, &imageMemoryRequirements);

			return imageMemoryRequirements;
		}

		VkImage const& Image::operator()() const {
			return m_image;
		}
	}
}

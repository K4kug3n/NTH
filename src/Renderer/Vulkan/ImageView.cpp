#include <Renderer/Vulkan/ImageView.hpp>

#include <Renderer/Vulkan/Device.hpp>
#include <Renderer/Vulkan/VkUtils.hpp>

#include <iostream>

namespace Nth {
	namespace Vk {
		ImageView::ImageView() :
			m_image_view(VK_NULL_HANDLE),
			m_device(nullptr) {
		}

		ImageView::ImageView(ImageView&& object) noexcept :
			m_image_view(object.m_image_view),
			m_device(object.m_device) {
			object.m_image_view = VK_NULL_HANDLE;
		}

		ImageView::~ImageView() {
			destroy();
		}

		void ImageView::create(const Device& device, const VkImageViewCreateInfo& infos) {
			VkResult result{ device.vkCreateImageView(device(), &infos, nullptr, &m_image_view) };
			if (result != VK_SUCCESS) {
				throw std::runtime_error("Can't create image view, " + to_string(result));
			}

			m_device = &device;
		}

		void ImageView::destroy() {
			if (m_image_view != VK_NULL_HANDLE) {
				m_device->vkDestroyImageView((*m_device)(), m_image_view, nullptr);
				m_image_view = VK_NULL_HANDLE;
			}
		}

		VkImageView ImageView::operator()() const {
			return m_image_view;
		}

		ImageView& ImageView::operator=(ImageView&& object) noexcept {
			destroy();

			m_image_view = object.m_image_view;
			m_device = object.m_device;

			object.m_image_view = VK_NULL_HANDLE;

			return *this;
		}
	}
}

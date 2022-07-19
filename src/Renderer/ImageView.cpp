#include "Renderer/ImageView.hpp"

#include "Renderer/Device.hpp"
#include "Renderer/VkUtil.hpp"

#include <iostream>

namespace Nth {
	namespace Vk {
		ImageView::ImageView() :
			m_imageView(VK_NULL_HANDLE),
			m_device(nullptr) {
		}

		ImageView::ImageView(ImageView&& object) noexcept :
			m_imageView(object.m_imageView),
			m_device(object.m_device) {
			object.m_imageView = VK_NULL_HANDLE;
		}

		ImageView::~ImageView() {
			destroy();
		}

		bool ImageView::create(Device const& device, VkImageViewCreateInfo const& infos) {
			VkResult result{ device.vkCreateImageView(device(), &infos, nullptr, &m_imageView) };
			if (result != VK_SUCCESS) {
				std::cerr << "Error: Can't create image view, " << toString(result) << std::endl;
				return false;
			}

			m_device = &device;

			return true;
		}

		void ImageView::destroy() {
			if (m_imageView != VK_NULL_HANDLE) {
				m_device->vkDestroyImageView((*m_device)(), m_imageView, nullptr);
				m_imageView = VK_NULL_HANDLE;
			}
		}

		VkImageView const& ImageView::operator()() const {
			return m_imageView;
		}

		ImageView& ImageView::operator=(ImageView&& object) noexcept {
			destroy();

			m_imageView = object.m_imageView;
			m_device = object.m_device;

			object.m_imageView = VK_NULL_HANDLE;

			return *this;
		}
	}
}

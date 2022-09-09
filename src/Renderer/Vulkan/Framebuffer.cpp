#include <Renderer/Vulkan/Framebuffer.hpp>

#include <Renderer/Vulkan/Device.hpp>
#include <Renderer/Vulkan/VkUtil.hpp>

#include <iostream>

namespace Nth {
	namespace Vk {
		Framebuffer::Framebuffer() :
			m_framebuffer(VK_NULL_HANDLE),
			m_device(nullptr) {
		}

		Framebuffer::Framebuffer(Framebuffer&& object) noexcept :
			m_framebuffer(object.m_framebuffer),
			m_device(object.m_device) {
			object.m_framebuffer = VK_NULL_HANDLE;
		}

		Framebuffer::~Framebuffer() {
			destroy();
		}

		bool Framebuffer::create(Device const& device, VkFramebufferCreateInfo const& infos) {
			VkResult result{ device.vkCreateFramebuffer(device(), &infos, nullptr, &m_framebuffer) };
			if (result != VK_SUCCESS) {
				std::cerr << "Error: Can't create framebuffer" << toString(result) << std::endl;
				return false;
			}

			m_device = &device;

			return true;
		}

		void Framebuffer::destroy() {
			if (isValid()) {
				m_device->vkDestroyFramebuffer((*m_device)(), m_framebuffer, nullptr);
				m_framebuffer = VK_NULL_HANDLE;
			}
		}

		bool Framebuffer::isValid() const {
			return m_framebuffer != VK_NULL_HANDLE;
		}

		VkFramebuffer const& Framebuffer::operator()() const {
			return m_framebuffer;
		}

		Framebuffer& Framebuffer::operator=(Framebuffer&& object) noexcept {
			m_framebuffer = object.m_framebuffer;
			m_device= object.m_device;

			object.m_framebuffer = VK_NULL_HANDLE;

			return *this;
		}
	}
}

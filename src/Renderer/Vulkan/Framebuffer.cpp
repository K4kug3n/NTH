#include <Renderer/Vulkan/Framebuffer.hpp>

#include <Renderer/Vulkan/Device.hpp>
#include <Renderer/Vulkan/VkUtils.hpp>

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

		void Framebuffer::create(const Device& device, const VkFramebufferCreateInfo& infos) {
			VkResult result{ device.vkCreateFramebuffer(device(), &infos, nullptr, &m_framebuffer) };
			if (result != VK_SUCCESS) {
				throw std::runtime_error("Can't create framebuffer, " + to_string(result));
			}

			m_device = &device;
		}

		void Framebuffer::destroy() {
			if (is_valid()) {
				m_device->vkDestroyFramebuffer((*m_device)(), m_framebuffer, nullptr);
				m_framebuffer = VK_NULL_HANDLE;
			}
		}

		bool Framebuffer::is_valid() const {
			return m_framebuffer != VK_NULL_HANDLE;
		}

		VkFramebuffer Framebuffer::operator()() const {
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

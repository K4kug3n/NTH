#include <Renderer/Vulkan/RenderPass.hpp>

#include <Renderer/Vulkan/Device.hpp>
#include <Renderer/Vulkan/VkUtils.hpp>

#include <iostream>

namespace Nth {
	namespace Vk {
		RenderPass::RenderPass() :
			m_render_pass(VK_NULL_HANDLE),
			m_device(nullptr) {}

		RenderPass::~RenderPass() {
			destroy();
		}

		void RenderPass::create(const Device& device, const VkRenderPassCreateInfo& infos) {
			VkResult result{ device.vkCreateRenderPass(device(), &infos, nullptr, &m_render_pass) };
			if (result != VK_SUCCESS) {
				throw std::runtime_error("Can't create renderpass, " + to_string(result));
			}

			m_device = &device;
		}

		void RenderPass::destroy() {
			if (m_render_pass != VK_NULL_HANDLE) {
				m_device->vkDestroyRenderPass((*m_device)(), m_render_pass, nullptr);
				m_render_pass = VK_NULL_HANDLE;
			}
		}

		VkRenderPass RenderPass::operator()() const {
			return m_render_pass;
		}
	}	
}

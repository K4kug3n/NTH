#include <Renderer/Vulkan/RenderPass.hpp>

#include <Renderer/Vulkan/Device.hpp>
#include <Renderer/Vulkan/VkUtil.hpp>

#include <iostream>

namespace Nth {
	namespace Vk {
		RenderPass::RenderPass() :
			m_renderPass(VK_NULL_HANDLE),
			m_device(nullptr) {}

		RenderPass::~RenderPass() {
			destroy();
		}

		bool RenderPass::create(Device const& device, VkRenderPassCreateInfo const& infos) {
			VkResult result{ device.vkCreateRenderPass(device(), &infos, nullptr, &m_renderPass) };
			if (result != VK_SUCCESS) {
				std::cerr << "Error: Can't create renderpass, " << toString(result) << std::endl;
				return false;
			}

			m_device = &device;

			return true;
		}

		void RenderPass::destroy() {
			if (m_renderPass != VK_NULL_HANDLE) {
				m_device->vkDestroyRenderPass((*m_device)(), m_renderPass, nullptr);
				m_renderPass = VK_NULL_HANDLE;
			}
		}

		VkRenderPass const& RenderPass::operator()() const {
			return m_renderPass;
		}
	}	
}

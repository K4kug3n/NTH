#include "Renderer/Vulkan/Pipeline.hpp"

#include "Renderer/Vulkan/Device.hpp"
#include "Renderer/Vulkan/VkUtil.hpp"

#include <iostream>

namespace Nth {
	namespace Vk {
		Pipeline::Pipeline() :
			m_pipeline(VK_NULL_HANDLE),
			m_device(nullptr) {
		}

		Pipeline::Pipeline(Pipeline&& object) noexcept :
			m_pipeline(object.m_pipeline),
			m_device(object.m_device) {
			object.m_pipeline = VK_NULL_HANDLE;
		}

		Pipeline::~Pipeline() {
			destroy();
		}

		bool Pipeline::createGraphics(Device const& device, VkPipelineCache cache, VkGraphicsPipelineCreateInfo const& infos) {
			VkResult result{ device.vkCreateGraphicsPipelines(device(), cache, 1, &infos, nullptr, &m_pipeline) };
			if (result != VK_SUCCESS) {
				std::cerr << "Error: Can't create graphics pipeline, " << toString(result) << std::endl;
			}

			m_device = &device;

			return true;
		}

		void Pipeline::destroy() {
			if (m_pipeline != VK_NULL_HANDLE) {
				m_device->vkDestroyPipeline((*m_device)(), m_pipeline, nullptr);
				m_pipeline = VK_NULL_HANDLE;
			}
		}

		VkPipeline const& Pipeline::operator()() const {
			return m_pipeline;
		}

		Pipeline& Pipeline::operator=(Pipeline&& object) noexcept {
			destroy();

			m_pipeline = object.m_pipeline;
			m_device = object.m_device;

			object.m_pipeline = VK_NULL_HANDLE;

			return *this;
		}
	}
}

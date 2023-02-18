#include <Renderer/Vulkan/Pipeline.hpp>

#include <Renderer/Vulkan/Device.hpp>
#include <Renderer/Vulkan/VkUtils.hpp>

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

		void Pipeline::create_graphics(const Device& device, VkPipelineCache cache, const VkGraphicsPipelineCreateInfo& infos) {
			VkResult result{ device.vkCreateGraphicsPipelines(device(), cache, 1, &infos, nullptr, &m_pipeline) };
			if (result != VK_SUCCESS) {
				throw std::runtime_error("Can't create graphics pipeline, " + to_string(result));
			}

			m_device = &device;
		}

		void Pipeline::destroy() {
			if (m_pipeline != VK_NULL_HANDLE) {
				m_device->vkDestroyPipeline((*m_device)(), m_pipeline, nullptr);
				m_pipeline = VK_NULL_HANDLE;
			}
		}

		VkPipeline Pipeline::operator()() const {
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

#include <Renderer/Vulkan/PipelineLayout.hpp>

#include <Renderer/Vulkan/Device.hpp>
#include <Renderer/Vulkan/VkUtils.hpp>

#include <iostream>

namespace Nth {
	namespace Vk {
		PipelineLayout::PipelineLayout() :
			m_pipeline_layout(VK_NULL_HANDLE),
			m_device(nullptr) {
		}

		PipelineLayout::PipelineLayout(PipelineLayout&& object) noexcept :
			m_pipeline_layout(object.m_pipeline_layout),
			m_device(object.m_device) {
			object.m_pipeline_layout = VK_NULL_HANDLE;
		}

		PipelineLayout::~PipelineLayout() {
			destroy();
		}

		void PipelineLayout::create(const Device& device, VkPipelineLayoutCreateFlags flags, uint32_t descriptor_count, VkDescriptorSetLayout const* descriptot_set_layouts, uint32_t constant_count, VkPushConstantRange const* push_constant_ranges) {
			VkPipelineLayoutCreateInfo layout_create_info = {
				VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,  // VkStructureType                sType
				nullptr,                                        // const void                    *pNext
				flags,                                          // VkPipelineLayoutCreateFlags    flags
				descriptor_count,                               // uint32_t                       setLayoutCount
				descriptot_set_layouts,                         // const VkDescriptorSetLayout   *pSetLayouts
				constant_count,                                 // uint32_t                       pushConstantRangeCount
				push_constant_ranges                            // const VkPushConstantRange     *pPushConstantRanges
			};

			VkResult result{ device.vkCreatePipelineLayout(device(), &layout_create_info, nullptr, &m_pipeline_layout) };
			if (result != VK_SUCCESS) {
				throw std::runtime_error(" Can't create pipeline layout, " + to_string(result));
			}

			m_device = &device;
		}

		void PipelineLayout::destroy() {
			if (m_pipeline_layout != VK_NULL_HANDLE) {
				m_device->vkDestroyPipelineLayout((*m_device)(), m_pipeline_layout, nullptr);
				m_pipeline_layout = VK_NULL_HANDLE;
			}
		}

		bool PipelineLayout::is_valid() {
			return m_pipeline_layout != VK_NULL_HANDLE;
		}

		VkPipelineLayout PipelineLayout::operator()() const {
			return m_pipeline_layout;
		}

		PipelineLayout& PipelineLayout::operator=(PipelineLayout&& object) noexcept {
			destroy();

			m_pipeline_layout = object.m_pipeline_layout;
			m_device = object.m_device;

			object.m_pipeline_layout = VK_NULL_HANDLE;

			return *this;
		}
	}
}

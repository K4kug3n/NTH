#include <Renderer/Vulkan/PipelineLayout.hpp>

#include <Renderer/Vulkan/Device.hpp>
#include <Renderer/Vulkan/VkUtil.hpp>

#include <iostream>

namespace Nth {
	namespace Vk {
		PipelineLayout::PipelineLayout() :
			m_pipelineLayout(VK_NULL_HANDLE),
			m_device(nullptr) {
		}

		PipelineLayout::PipelineLayout(PipelineLayout&& object) noexcept :
			m_pipelineLayout(object.m_pipelineLayout),
			m_device(object.m_device) {
			object.m_pipelineLayout = VK_NULL_HANDLE;
		}

		PipelineLayout::~PipelineLayout() {
			destroy();
		}

		bool PipelineLayout::create(Device const& device, VkPipelineLayoutCreateFlags flags, uint32_t descriptorCount, VkDescriptorSetLayout const* descriptotSetLayouts, uint32_t constantCount, VkPushConstantRange const* pushConstantRanges) {
			VkPipelineLayoutCreateInfo layoutCreateInfo = {
				VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,  // VkStructureType                sType
				nullptr,                                        // const void                    *pNext
				flags,                                          // VkPipelineLayoutCreateFlags    flags
				descriptorCount,                                // uint32_t                       setLayoutCount
				descriptotSetLayouts,                           // const VkDescriptorSetLayout   *pSetLayouts
				constantCount,                                  // uint32_t                       pushConstantRangeCount
				pushConstantRanges                              // const VkPushConstantRange     *pPushConstantRanges
			};

			VkResult result{ device.vkCreatePipelineLayout(device(), &layoutCreateInfo, nullptr, &m_pipelineLayout) };
			if (result != VK_SUCCESS) {
				std::cerr << "Error: Can't create pipeline layout, " << toString(result) << std::endl;
				return false;
			}

			m_device = &device;

			return true;
		}

		void PipelineLayout::destroy() {
			if (m_pipelineLayout != VK_NULL_HANDLE) {
				m_device->vkDestroyPipelineLayout((*m_device)(), m_pipelineLayout, nullptr);
				m_pipelineLayout = VK_NULL_HANDLE;
			}
		}

		bool PipelineLayout::isValid() {
			return m_pipelineLayout != VK_NULL_HANDLE;
		}

		VkPipelineLayout PipelineLayout::operator()() const {
			return m_pipelineLayout;
		}

		PipelineLayout& PipelineLayout::operator=(PipelineLayout&& object) noexcept {
			destroy();

			m_pipelineLayout = object.m_pipelineLayout;
			m_device = object.m_device;

			object.m_pipelineLayout = VK_NULL_HANDLE;

			return *this;
		}
	}
}

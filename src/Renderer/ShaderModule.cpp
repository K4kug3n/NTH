#include "Renderer/ShaderModule.hpp"

#include "Renderer/Device.hpp"
#include "Renderer/VkUtil.hpp"

#include <iostream>

namespace Nth {
	namespace Vk {
		ShaderModule::ShaderModule(ShaderModule&& object) noexcept :
			m_shaderModule(object.m_shaderModule),
			m_device(object.m_device) {
			object.m_shaderModule = VK_NULL_HANDLE;
		}

		ShaderModule::~ShaderModule() {
			if (m_device != VK_NULL_HANDLE) {
				m_device->vkDestroyShaderModule((*m_device)(), m_shaderModule, nullptr);
			}
		}

		bool ShaderModule::create(Device const& device, size_t codeSize, uint32_t const* code) {
			VkShaderModuleCreateInfo info = {
				VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,    // VkStructureType                sType
				nullptr,                                        // const void                    *pNext
				0,                                              // VkShaderModuleCreateFlags      flags
				codeSize,                                       // size_t                         codeSize
				code                                            // const uint32_t                *pCode
			};

			VkResult result{ device.vkCreateShaderModule(device(), &info, nullptr, &m_shaderModule) };
			if (result != VK_SUCCESS) {
				std::cerr << "Error: Can't create shader module, " << toString(result) << std::endl;
				return false;
			}

			m_device = &device;

			return true;
		}

		bool ShaderModule::isValid() const {
			return m_shaderModule != VK_NULL_HANDLE;
		}

		VkShaderModule const& ShaderModule::operator()() const {
			return m_shaderModule;
		}
	}
}

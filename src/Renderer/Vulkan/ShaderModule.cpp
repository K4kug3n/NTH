#include <Renderer/Vulkan/ShaderModule.hpp>

#include <Renderer/Vulkan/Device.hpp>
#include <Renderer/Vulkan/VkUtils.hpp>

#include <iostream>

namespace Nth {
	namespace Vk {
		ShaderModule::ShaderModule(ShaderModule&& object) noexcept :
			m_shader_module(object.m_shader_module),
			m_device(object.m_device) {
			object.m_shader_module = VK_NULL_HANDLE;
		}

		ShaderModule::~ShaderModule() {
			if (m_device != VK_NULL_HANDLE) {
				m_device->vkDestroyShaderModule((*m_device)(), m_shader_module, nullptr);
			}
		}

		void ShaderModule::create(const  Device& device, size_t codeSize, uint32_t const* code) {
			VkShaderModuleCreateInfo info = {
				VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,    // VkStructureType                sType
				nullptr,                                        // const void                    *pNext
				0,                                              // VkShaderModuleCreateFlags      flags
				codeSize,                                       // size_t                         codeSize
				code                                            // const uint32_t                *pCode
			};

			VkResult result{ device.vkCreateShaderModule(device(), &info, nullptr, &m_shader_module) };
			if (result != VK_SUCCESS) {
				throw std::runtime_error("Can't create shader module, " + to_string(result));
			}

			m_device = &device;
		}

		bool ShaderModule::is_valid() const {
			return m_shader_module != VK_NULL_HANDLE;
		}

		VkShaderModule ShaderModule::operator()() const {
			return m_shader_module;
		}
	}
}

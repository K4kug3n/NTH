#ifndef NTH_RENDERER_VK_SHADERMODULE_HPP
#define NTH_RENDERER_VK_SHADERMODULE_HPP

#include <vulkan/vulkan.h>

namespace Nth {
	namespace Vk {
		class Device;

		class ShaderModule {
		public:
			ShaderModule() = default;
			ShaderModule(const ShaderModule&) = delete;
			ShaderModule(ShaderModule&& object) noexcept;
			~ShaderModule();

			void create(const Device& device, size_t code_size, uint32_t const* code);

			bool is_valid() const;

			VkShaderModule operator()() const;

			ShaderModule& operator=(const ShaderModule&) = delete;
			ShaderModule& operator=(ShaderModule&&) = delete;

		private:
			VkShaderModule m_shader_module;
			Device const* m_device;
		};
	}
}

#endif

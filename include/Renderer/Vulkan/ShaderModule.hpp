#ifndef NTH_RENDERER_VK_SHADERMODULE_HPP
#define NTH_RENDERER_VK_SHADERMODULE_HPP

#include <vulkan/vulkan.h>

namespace Nth {
	namespace Vk {
		class Device;

		class ShaderModule {
		public:
			ShaderModule() = default;
			ShaderModule(ShaderModule const&) = delete;
			ShaderModule(ShaderModule&& object) noexcept;
			~ShaderModule();

			bool create(Device const& device, size_t codeSize, uint32_t const* code);

			bool isValid() const;

			VkShaderModule const& operator()() const;

			ShaderModule& operator=(ShaderModule const&) = delete;
			ShaderModule& operator=(ShaderModule&&) = delete;

		private:
			VkShaderModule m_shaderModule;
			Device const* m_device;
		};
	}
}

#endif

#ifndef NTH_RENDERER_MATERIAL_HPP
#define NTH_RENDERER_MATERIAL_HPP

#include "Renderer/Vulkan/Pipeline.hpp"
#include "Renderer/Vulkan/PipelineLayout.hpp"
#include "Renderer/Vulkan/ShaderModule.hpp"
#include "Renderer/Vulkan/RenderPass.hpp"

#include <string_view>
#include <vector>
#include <string>

namespace Nth {
	class Vk::Device;

	struct MaterialInfos {
		std::string vertexShaderName;
		std::string fragmentShaderName;
	};

	class Material {
	public:
		Material() = default;
		Material(Material const&) = delete;
		Material(Material&&) = default;
		~Material() = default;

		bool createPipeline(Vk::Device const& device, Vk::RenderPass const& renderPass, const std::string_view vertexShaderName, const std::string_view fragmentShaderName, std::vector<VkDescriptorSetLayout> const& descriptorSetLayouts);

		Material& operator=(Material const&) = delete;
		Material& operator=(Material&&) = default;

		Vk::Pipeline pipeline;
		Vk::PipelineLayout pipelineLayout;

	private:
		bool createPipelineLayout(Vk::Device const& device, std::vector<VkDescriptorSetLayout> const& descriptorSetLayouts);

		Vk::ShaderModule createShaderModule(Vk::Device const& device, const std::string_view filename) const;
	};
}

#endif
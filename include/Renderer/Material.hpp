#ifndef NTH_RENDERER_MATERIAL_HPP
#define NTH_RENDERER_MATERIAL_HPP

#include <Renderer/Vulkan/Pipeline.hpp>
#include <Renderer/Vulkan/PipelineLayout.hpp>
#include <Renderer/Vulkan/ShaderModule.hpp>
#include <Renderer/Vulkan/RenderPass.hpp>

#include <filesystem>
#include <vector>
#include <string>

namespace Nth {
	class Vk::Device;

	struct MaterialInfos {
		std::filesystem::path vertexShaderName;
		std::filesystem::path fragmentShaderName;
	};

	class Material {
	public:
		Material() = default;
		Material(const Material&) = delete;
		Material(Material&&) = default;
		~Material() = default;

		void create_pipeline(const Vk::Device& device, const Vk::RenderPass& render_pass, const std::filesystem::path& vertex_shader_name, 
			const std::filesystem::path& fragment_shader_name, const std::vector<VkDescriptorSetLayout>& descriptor_set_layouts);

		Material& operator=(const Material&) = delete;
		Material& operator=(Material&&) = default;

		Vk::Pipeline pipeline;
		Vk::PipelineLayout pipeline_layout;

	private:
		void create_pipeline_layout(const Vk::Device& device, const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts);

		Vk::ShaderModule create_shader_module(const Vk::Device& device, const std::filesystem::path& path) const;
	};
}

#endif
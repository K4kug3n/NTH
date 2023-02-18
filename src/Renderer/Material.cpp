#include <Renderer/Material.hpp>

#include <Renderer/Vulkan/Device.hpp>
#include <Renderer/Vertex.hpp>

#include <Utils/Reader.hpp>

#include <iostream>

namespace Nth {
	void Material::create_pipeline(const Vk::Device& device, const Vk::RenderPass& render_pass, const std::filesystem::path& vertex_shader_name, const std::filesystem::path& fragment_shader_name, const std::vector<VkDescriptorSetLayout>& descriptor_set_layouts) {
		Vk::ShaderModule vertex_shader_module = create_shader_module(device, vertex_shader_name);
		Vk::ShaderModule fragment_shader_module = create_shader_module(device, fragment_shader_name);

		if (!vertex_shader_module.is_valid() || !fragment_shader_module.is_valid()) {
			throw std::runtime_error("Can't create vertex or shader module");
		}

		std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos = {
			// Vertex shader
			{
				VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,        // VkStructureType                                sType
				nullptr,                                                    // const void                                    *pNext
				0,                                                          // VkPipelineShaderStageCreateFlags               flags
				VK_SHADER_STAGE_VERTEX_BIT,                                 // VkShaderStageFlagBits                          stage
				vertex_shader_module(),                                       // VkShaderModule                                 module
				"main",                                                     // const char                                    *pName
				nullptr                                                     // const VkSpecializationInfo                    *pSpecializationInfo
			},
			// Fragment shader
			{
				VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,        // VkStructureType                                sType
				nullptr,                                                    // const void                                    *pNext
				0,                                                          // VkPipelineShaderStageCreateFlags               flags
				VK_SHADER_STAGE_FRAGMENT_BIT,                               // VkShaderStageFlagBits                          stage
				fragment_shader_module(),                                     // VkShaderModule                                 module
				"main",                                                     // const char                                    *pName
				nullptr                                                     // const VkSpecializationInfo                    *pSpecializationInfo
			}
		};

		VertexInputDescription vertex_input_description = Vertex::get_vertex_description();
		VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {
			VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,       // VkStructureType                                sType
			nullptr,                                                         // const void                                    *pNext
			0,                                                               // VkPipelineVertexInputStateCreateFlags          flags
			static_cast<uint32_t>(vertex_input_description.bindings.size()),   // uint32_t                                       vertexBindingDescriptionCount
			vertex_input_description.bindings.data(),                          // const VkVertexInputBindingDescription         *pVertexBindingDescriptions
			static_cast<uint32_t>(vertex_input_description.attributes.size()), // uint32_t                                       vertexAttributeDescriptionCount
			vertex_input_description.attributes.data()                         // const VkVertexInputAttributeDescription       *pVertexAttributeDescriptions
		};

		VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info = {
			VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,  // VkStructureType                                sType
			nullptr,                                                      // const void                                    *pNext
			0,                                                            // VkPipelineInputAssemblyStateCreateFlags        flags
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,                          // VkPrimitiveTopology                            topology
			VK_FALSE                                                      // VkBool32                                       primitiveRestartEnable
		};

		VkPipelineViewportStateCreateInfo viewport_state_create_info = {
			VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,        // VkStructureType                                sType
			nullptr,                                                      // const void                                    *pNext
			0,                                                            // VkPipelineViewportStateCreateFlags             flags
			1,                                                            // uint32_t                                       viewportCount
			nullptr,                                                      // const VkViewport                              *pViewports
			1,                                                            // uint32_t                                       scissorCount
			nullptr                                                       // const VkRect2D                                *pScissors
		};

		VkPipelineRasterizationStateCreateInfo rasterization_state_create_info = {
			VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,   // VkStructureType                                sType
			nullptr,                                                      // const void                                    *pNext
			0,                                                            // VkPipelineRasterizationStateCreateFlags        flags
			VK_FALSE,                                                     // VkBool32                                       depthClampEnable
			VK_FALSE,                                                     // VkBool32                                       rasterizerDiscardEnable
			VK_POLYGON_MODE_FILL,                                         // VkPolygonMode                                  polygonMode
			VK_CULL_MODE_NONE,                                        // VkCullModeFlags                                cullMode
			VK_FRONT_FACE_COUNTER_CLOCKWISE,                              // VkFrontFace                                    frontFace
			VK_FALSE,                                                     // VkBool32                                       depthBiasEnable
			0.0f,                                                         // float                                          depthBiasConstantFactor
			0.0f,                                                         // float                                          depthBiasClamp
			0.0f,                                                         // float                                          depthBiasSlopeFactor
			1.0f                                                          // float                                          lineWidth
		};

		VkPipelineMultisampleStateCreateInfo multisample_state_create_info = {
			VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,     // VkStructureType                                sType
			nullptr,                                                      // const void                                    *pNext
			0,                                                            // VkPipelineMultisampleStateCreateFlags          flags
			VK_SAMPLE_COUNT_1_BIT,                                        // VkSampleCountFlagBits                          rasterizationSamples
			VK_FALSE,                                                     // VkBool32                                       sampleShadingEnable
			1.0f,                                                         // float                                          minSampleShading
			nullptr,                                                      // const VkSampleMask                            *pSampleMask
			VK_FALSE,                                                     // VkBool32                                       alphaToCoverageEnable
			VK_FALSE                                                      // VkBool32                                       alphaToOneEnable
		};

		VkPipelineColorBlendAttachmentState color_blend_attachment_state = {
			VK_FALSE,                                                     // VkBool32                                       blendEnable
			VK_BLEND_FACTOR_ONE,                                          // VkBlendFactor                                  srcColorBlendFactor
			VK_BLEND_FACTOR_ZERO,                                         // VkBlendFactor                                  dstColorBlendFactor
			VK_BLEND_OP_ADD,                                              // VkBlendOp                                      colorBlendOp
			VK_BLEND_FACTOR_ONE,                                          // VkBlendFactor                                  srcAlphaBlendFactor
			VK_BLEND_FACTOR_ZERO,                                         // VkBlendFactor                                  dstAlphaBlendFactor
			VK_BLEND_OP_ADD,                                              // VkBlendOp                                      alphaBlendOp
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |         // VkColorComponentFlags                          colorWriteMask
			VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
		};

		VkPipelineColorBlendStateCreateInfo color_blend_state_create_info = {
			VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,     // VkStructureType                                sType
			nullptr,                                                      // const void                                    *pNext
			0,                                                            // VkPipelineColorBlendStateCreateFlags           flags
			VK_FALSE,                                                     // VkBool32                                       logicOpEnable
			VK_LOGIC_OP_COPY,                                             // VkLogicOp                                      logicOp
			1,                                                            // uint32_t                                       attachmentCount
			&color_blend_attachment_state,                                   // const VkPipelineColorBlendAttachmentState     *pAttachments
			{ 0.0f, 0.0f, 0.0f, 0.0f }                                    // float                                          blendConstants[4]
		};

		std::vector<VkDynamicState> dynamic_states = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR,
		};

		VkPipelineDynamicStateCreateInfo dynamic_state_create_info = {
			VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,         // VkStructureType                                sType
			nullptr,                                                      // const void                                    *pNext
			0,                                                            // VkPipelineDynamicStateCreateFlags              flags
			static_cast<uint32_t>(dynamic_states.size()),                 // uint32_t                                       dynamicStateCount
			dynamic_states.data()                                         // const VkDynamicState                          *pDynamicStates
		};

		create_pipeline_layout(device, descriptor_set_layouts);

		VkPipelineDepthStencilStateCreateInfo depth_stencil{};
		depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depth_stencil.depthTestEnable = VK_TRUE;
		depth_stencil.depthWriteEnable = VK_TRUE;
		depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;
		depth_stencil.depthBoundsTestEnable = VK_FALSE;
		depth_stencil.minDepthBounds = 0.0f; // Optional
		depth_stencil.maxDepthBounds = 1.0f; // Optional
		depth_stencil.stencilTestEnable = VK_FALSE;
		depth_stencil.front = {}; // Optional
		depth_stencil.back = {}; // Optional

		VkGraphicsPipelineCreateInfo pipeline_create_info = {
			VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,       // VkStructureType                                sType
			nullptr,                                               // const void                                    *pNext
			0,                                                     // VkPipelineCreateFlags                          flags
			static_cast<uint32_t>(shaderStageCreateInfos.size()),  // uint32_t                                       stageCount
			shaderStageCreateInfos.data(),                         // const VkPipelineShaderStageCreateInfo         *pStages
			&vertexInputStateCreateInfo,                           // const VkPipelineVertexInputStateCreateInfo    *pVertexInputState;
			&input_assembly_state_create_info,                         // const VkPipelineInputAssemblyStateCreateInfo  *pInputAssemblyState
			nullptr,                                               // const VkPipelineTessellationStateCreateInfo   *pTessellationState
			&viewport_state_create_info,                              // const VkPipelineViewportStateCreateInfo       *pViewportState
			&rasterization_state_create_info,                         // const VkPipelineRasterizationStateCreateInfo  *pRasterizationState
			&multisample_state_create_info,                           // const VkPipelineMultisampleStateCreateInfo    *pMultisampleState
			&depth_stencil,                                         // const VkPipelineDepthStencilStateCreateInfo   *pDepthStencilState
			&color_blend_state_create_info,                            // const VkPipelineColorBlendStateCreateInfo     *pColorBlendState
			&dynamic_state_create_info,                               // const VkPipelineDynamicStateCreateInfo        *pDynamicState
			pipeline_layout(),                                      // VkPipelineLayout                               layout
			render_pass(),                                          // VkRenderPass                                   renderPass
			0,                                                     // uint32_t                                       subpass
			VK_NULL_HANDLE,                                        // VkPipeline                                     basePipelineHandle
			-1                                                     // int32_t                                        basePipelineIndex
		};

		pipeline.create_graphics(device, VK_NULL_HANDLE, pipeline_create_info);
	}

	void Material::create_pipeline_layout(const Vk::Device& device, const std::vector<VkDescriptorSetLayout>& descriptor_set_layouts) {
		pipeline_layout.create(
			device,
			0,
			static_cast<uint32_t>(descriptor_set_layouts.size()),
			descriptor_set_layouts.data(),
			0,
			nullptr
		);
	}

	Vk::ShaderModule Material::create_shader_module(const Vk::Device& device, const std::filesystem::path& path) const {
		std::vector<char> code{ read_binary_file(path) };

		Vk::ShaderModule shader;
		shader.create(device, code.size(), reinterpret_cast<const uint32_t*>(&code[0]));

		return shader;
	}
}

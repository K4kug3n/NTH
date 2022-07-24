#include "Renderer/Material.hpp"

#include "Renderer/Vulkan/Device.hpp"
#include "Renderer/Vertex.hpp"

#include "Util/Reader.hpp"

#include <iostream>

namespace Nth {
	bool Material::createPipeline(Vk::Device const& device, Vk::RenderPass const& renderPass, const std::string_view vertexShaderName, const std::string_view fragmentShaderName, std::vector<VkDescriptorSetLayout> const& descriptorSetLayouts) {
		Vk::ShaderModule vertexShaderModule = createShaderModule(device, vertexShaderName);
		Vk::ShaderModule fragmentShaderModule = createShaderModule(device, fragmentShaderName);

		if (!vertexShaderModule.isValid() || !fragmentShaderModule.isValid()) {
			return false;
		}

		std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos = {
			// Vertex shader
			{
				VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,        // VkStructureType                                sType
				nullptr,                                                    // const void                                    *pNext
				0,                                                          // VkPipelineShaderStageCreateFlags               flags
				VK_SHADER_STAGE_VERTEX_BIT,                                 // VkShaderStageFlagBits                          stage
				vertexShaderModule(),                                       // VkShaderModule                                 module
				"main",                                                     // const char                                    *pName
				nullptr                                                     // const VkSpecializationInfo                    *pSpecializationInfo
			},
			// Fragment shader
			{
				VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,        // VkStructureType                                sType
				nullptr,                                                    // const void                                    *pNext
				0,                                                          // VkPipelineShaderStageCreateFlags               flags
				VK_SHADER_STAGE_FRAGMENT_BIT,                               // VkShaderStageFlagBits                          stage
				fragmentShaderModule(),                                     // VkShaderModule                                 module
				"main",                                                     // const char                                    *pName
				nullptr                                                     // const VkSpecializationInfo                    *pSpecializationInfo
			}
		};

		VertexInputDescription vertexInputDescription = Vertex::getVertexDescription();
		VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {
			VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,       // VkStructureType                                sType
			nullptr,                                                         // const void                                    *pNext
			0,                                                               // VkPipelineVertexInputStateCreateFlags          flags
			static_cast<uint32_t>(vertexInputDescription.bindings.size()),   // uint32_t                                       vertexBindingDescriptionCount
			vertexInputDescription.bindings.data(),                          // const VkVertexInputBindingDescription         *pVertexBindingDescriptions
			static_cast<uint32_t>(vertexInputDescription.attributes.size()), // uint32_t                                       vertexAttributeDescriptionCount
			vertexInputDescription.attributes.data()                         // const VkVertexInputAttributeDescription       *pVertexAttributeDescriptions
		};

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = {
			VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,  // VkStructureType                                sType
			nullptr,                                                      // const void                                    *pNext
			0,                                                            // VkPipelineInputAssemblyStateCreateFlags        flags
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,                          // VkPrimitiveTopology                            topology
			VK_FALSE                                                      // VkBool32                                       primitiveRestartEnable
		};

		VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {
			VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,        // VkStructureType                                sType
			nullptr,                                                      // const void                                    *pNext
			0,                                                            // VkPipelineViewportStateCreateFlags             flags
			1,                                                            // uint32_t                                       viewportCount
			nullptr,                                                      // const VkViewport                              *pViewports
			1,                                                            // uint32_t                                       scissorCount
			nullptr                                                       // const VkRect2D                                *pScissors
		};

		VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {
			VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,   // VkStructureType                                sType
			nullptr,                                                      // const void                                    *pNext
			0,                                                            // VkPipelineRasterizationStateCreateFlags        flags
			VK_FALSE,                                                     // VkBool32                                       depthClampEnable
			VK_FALSE,                                                     // VkBool32                                       rasterizerDiscardEnable
			VK_POLYGON_MODE_FILL,                                         // VkPolygonMode                                  polygonMode
			VK_CULL_MODE_BACK_BIT,                                        // VkCullModeFlags                                cullMode
			VK_FRONT_FACE_COUNTER_CLOCKWISE,                              // VkFrontFace                                    frontFace
			VK_FALSE,                                                     // VkBool32                                       depthBiasEnable
			0.0f,                                                         // float                                          depthBiasConstantFactor
			0.0f,                                                         // float                                          depthBiasClamp
			0.0f,                                                         // float                                          depthBiasSlopeFactor
			1.0f                                                          // float                                          lineWidth
		};

		VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = {
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

		VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {
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

		VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = {
			VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,     // VkStructureType                                sType
			nullptr,                                                      // const void                                    *pNext
			0,                                                            // VkPipelineColorBlendStateCreateFlags           flags
			VK_FALSE,                                                     // VkBool32                                       logicOpEnable
			VK_LOGIC_OP_COPY,                                             // VkLogicOp                                      logicOp
			1,                                                            // uint32_t                                       attachmentCount
			&colorBlendAttachmentState,                                   // const VkPipelineColorBlendAttachmentState     *pAttachments
			{ 0.0f, 0.0f, 0.0f, 0.0f }                                    // float                                          blendConstants[4]
		};

		std::vector<VkDynamicState> dynamic_states = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR,
		};

		VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {
			VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,         // VkStructureType                                sType
			nullptr,                                                      // const void                                    *pNext
			0,                                                            // VkPipelineDynamicStateCreateFlags              flags
			static_cast<uint32_t>(dynamic_states.size()),                 // uint32_t                                       dynamicStateCount
			dynamic_states.data()                                         // const VkDynamicState                          *pDynamicStates
		};

		if (!createPipelineLayout(device, descriptorSetLayouts)) {
			return false;
		}

		VkPipelineDepthStencilStateCreateInfo depthStencil{};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = VK_TRUE;
		depthStencil.depthWriteEnable = VK_TRUE;
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.minDepthBounds = 0.0f; // Optional
		depthStencil.maxDepthBounds = 1.0f; // Optional
		depthStencil.stencilTestEnable = VK_FALSE;
		depthStencil.front = {}; // Optional
		depthStencil.back = {}; // Optional

		VkGraphicsPipelineCreateInfo pipelineCreateInfo = {
			VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,       // VkStructureType                                sType
			nullptr,                                               // const void                                    *pNext
			0,                                                     // VkPipelineCreateFlags                          flags
			static_cast<uint32_t>(shaderStageCreateInfos.size()),  // uint32_t                                       stageCount
			shaderStageCreateInfos.data(),                         // const VkPipelineShaderStageCreateInfo         *pStages
			&vertexInputStateCreateInfo,                           // const VkPipelineVertexInputStateCreateInfo    *pVertexInputState;
			&inputAssemblyStateCreateInfo,                         // const VkPipelineInputAssemblyStateCreateInfo  *pInputAssemblyState
			nullptr,                                               // const VkPipelineTessellationStateCreateInfo   *pTessellationState
			&viewportStateCreateInfo,                              // const VkPipelineViewportStateCreateInfo       *pViewportState
			&rasterizationStateCreateInfo,                         // const VkPipelineRasterizationStateCreateInfo  *pRasterizationState
			&multisampleStateCreateInfo,                           // const VkPipelineMultisampleStateCreateInfo    *pMultisampleState
			&depthStencil,                                         // const VkPipelineDepthStencilStateCreateInfo   *pDepthStencilState
			&colorBlendStateCreateInfo,                            // const VkPipelineColorBlendStateCreateInfo     *pColorBlendState
			&dynamicStateCreateInfo,                               // const VkPipelineDynamicStateCreateInfo        *pDynamicState
			pipelineLayout(),                                      // VkPipelineLayout                               layout
			renderPass(),                                          // VkRenderPass                                   renderPass
			0,                                                     // uint32_t                                       subpass
			VK_NULL_HANDLE,                                        // VkPipeline                                     basePipelineHandle
			-1                                                     // int32_t                                        basePipelineIndex
		};

		if (!pipeline.createGraphics(device, VK_NULL_HANDLE, pipelineCreateInfo)) {
			std::cerr << "Error: Could not create graphics pipeline!" << std::endl;
			return false;
		}
		return true;
	}

	bool Material::createPipelineLayout(Vk::Device const& device, std::vector<VkDescriptorSetLayout> const& descriptorSetLayouts) {
		return pipelineLayout.create(
			device,
			0,
			static_cast<uint32_t>(descriptorSetLayouts.size()),
			descriptorSetLayouts.data(),
			0,
			nullptr
		);
	}

	Vk::ShaderModule Material::createShaderModule(Vk::Device const& device, const std::string_view filename) const {
		std::vector<char> code{ readBinaryFile(filename) };

		Vk::ShaderModule shader;
		if (!shader.create(device, code.size(), reinterpret_cast<const uint32_t*>(&code[0]))) {
			throw std::runtime_error("Can't create shader module");
		}

		return shader;
	}
}

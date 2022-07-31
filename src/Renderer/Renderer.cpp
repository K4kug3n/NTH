#include "Renderer/Renderer.hpp"

namespace Nth {
	Renderer::Renderer() :
		m_vulkan(),
		m_renderWindow(m_vulkan) { }

	RenderWindow& Renderer::getWindow(VideoMode const& mode, const std::string_view title) {
		if (!m_renderWindow.create(mode, title)) {
			throw std::runtime_error("Can't create render window");
		}

		m_mainDescriptorLayout = getMainDescriptorLayout();
		m_ssboDescriptorLayout = getSSBODescriptorLayout();

		m_descriptorAllocator.init(m_vulkan.getDevice());

		m_renderWindow.getDescriptor() = m_descriptorAllocator.allocate(m_mainDescriptorLayout);

		std::vector<RenderingResource>& ressources = m_renderWindow.getRenderingRessources();
		for (size_t i = 0; i < ressources.size(); ++i) {
			ressources[i].ssboDescriptor = m_descriptorAllocator.allocate(m_ssboDescriptorLayout);
		}

		m_renderWindow.updateDescriptorSet();

		return m_renderWindow;
	}

	Material& Renderer::createMaterial(const std::string_view vertexShaderName, const std::string_view fragmentShaderName) {		
		std::vector<VkDescriptorSetLayout> vkDescritptorLayouts {
			m_mainDescriptorLayout(),
			m_ssboDescriptorLayout()
		};

		Material material;
		material.createPipeline(m_vulkan.getDevice(), m_renderWindow.getRenderPass(), vertexShaderName, fragmentShaderName, vkDescritptorLayouts);

		return m_materials.emplace_back(std::move(material));
	}

	Vk::DescriptorSetLayout Renderer::getMainDescriptorLayout() const {
		std::vector<VkDescriptorSetLayoutBinding> layoutBindings = {
			{
				0,                                          // uint32_t             binding
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,  // VkDescriptorType     descriptorType
				1,                                          // uint32_t             descriptorCount
				VK_SHADER_STAGE_FRAGMENT_BIT,               // VkShaderStageFlags   stageFlags
				nullptr                                     // const VkSampler     *pImmutableSamplers
			},
			{
				1,                                         // uint32_t           binding
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         // VkDescriptorType   descriptorType
				1,                                         // uint32_t           descriptorCount
				VK_SHADER_STAGE_VERTEX_BIT,                // VkShaderStageFlags stageFlags
				nullptr                                    // const VkSampler *pImmutableSamplers
			}
		};

		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
			VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,  // VkStructureType                      sType
			nullptr,                                              // const void                          *pNext
			0,                                                    // VkDescriptorSetLayoutCreateFlags     flags
			static_cast<uint32_t>(layoutBindings.size()),         // uint32_t                             bindingCount
			layoutBindings.data()                                 // const VkDescriptorSetLayoutBinding  *pBindings
		};

		Vk::DescriptorSetLayout layout;
		if (!layout.create(m_vulkan.getDevice(), descriptorSetLayoutCreateInfo)) {
			throw std::runtime_error("Could not create descriptor set layout!");
		}

		return layout;
	}

	Vk::DescriptorSetLayout Renderer::getSSBODescriptorLayout() const {
		std::vector<VkDescriptorSetLayoutBinding> layoutBindings = {
			{
				0,                                          // uint32_t             binding
				VK_DESCRIPTOR_TYPE_STORAGE_BUFFER ,         // VkDescriptorType     descriptorType
				1,                                          // uint32_t             descriptorCount
				VK_SHADER_STAGE_VERTEX_BIT,                 // VkShaderStageFlags   stageFlags
				nullptr                                     // const VkSampler     *pImmutableSamplers
			}
		};

		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
			VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,  // VkStructureType                      sType
			nullptr,                                              // const void                          *pNext
			0,                                                    // VkDescriptorSetLayoutCreateFlags     flags
			static_cast<uint32_t>(layoutBindings.size()),        // uint32_t                             bindingCount
			layoutBindings.data()                                // const VkDescriptorSetLayoutBinding  *pBindings
		};

		Vk::DescriptorSetLayout layout;
		if (!layout.create(m_vulkan.getDevice(), descriptorSetLayoutCreateInfo)) {
			throw std::runtime_error("Could not create descriptor set layout!");
		}

		return layout;
	}
	void Renderer::waitIdle() const {
		m_vulkan.getDevice().waitIdle();
	}
}

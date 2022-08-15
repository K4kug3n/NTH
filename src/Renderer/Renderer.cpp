#include "Renderer/Renderer.hpp"

#include "Util/Image.hpp"

#include <cstring>
#include <iostream>

namespace Nth {
	Renderer::Renderer() :
		m_vulkan(),
		m_renderWindow(m_vulkan),
		m_resourceIndex(0),
		m_renderingResources(RenderWindow::resourceCount) { }

	RenderWindow& Renderer::getWindow(VideoMode const& mode, const std::string_view title) {
		if (!m_renderWindow.create(mode, title)) {
			throw std::runtime_error("Can't create render window");
		}

		if (!createRenderingResources()) {
			throw std::runtime_error("Can't create rendering ressources");
		}

		if (!createSSBO()) {
			throw std::runtime_error("Can't create ssbo");
		}

		if (!createUniformBuffer()) {
			throw std::runtime_error("Can't create uniform buffer");
		}

		m_mainDescriptorLayout = getMainDescriptorLayout();
		m_ssboDescriptorLayout = getSSBODescriptorLayout();
		m_textureDescriptorLayout = getTextureDescriptorLayout();
		m_lightDescriptorLayout = getLightDescriptorLayout();

		m_descriptorAllocator.init(m_vulkan.getDevice().getHandle());

		for (size_t i = 0; i < m_renderingResources.size(); ++i) {
			m_renderingResources[i].viewerDescriptor = m_descriptorAllocator.allocate(m_mainDescriptorLayout);
			m_renderingResources[i].ssboDescriptor = m_descriptorAllocator.allocate(m_ssboDescriptorLayout);
			m_renderingResources[i].lightDescriptor = m_descriptorAllocator.allocate(m_lightDescriptorLayout);

			m_renderingResources[i].lightBuffer = VulkanBuffer{
				m_vulkan.getDevice(),
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				sizeof(LightGpuObject)
			};
		}

		copyLightData();

		updateDescriptorSet();

		EventHandler& eventHandler = m_renderWindow.getEventHandler();
		eventHandler.onResize.connect([this]() {
			m_vulkan.getDevice().getHandle().waitIdle();

			if (!copyUniformBufferData()) {
				throw std::runtime_error("Can't re-copy uniform data");
			}
		});

		return m_renderWindow;
	}

	VulkanTexture Renderer::createTexture(const std::string_view name) {
		Image image = Image::loadFromFile(name, PixelChannel::Rgba);

		std::vector<char> const& pixels = image.pixels();
		if (pixels.empty()) {
			throw std::runtime_error("Can't read file");
		}

		VulkanTexture texture;

		texture.create(
			m_vulkan.getDevice(),
			image.width(),
			image.height(),
			static_cast<uint32_t>(pixels.size()),
			VK_FORMAT_R8G8B8A8_UNORM,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		texture.createView(VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);

		texture.image.copyByStaging(pixels.data(), static_cast<uint32_t>(pixels.size()), image.width(), image.height(), m_renderingResources[0].commandBuffer);

		texture.descriptorSet = m_descriptorAllocator.allocate(m_textureDescriptorLayout);

		VkDescriptorImageInfo textureInfo = {
			texture.sampler(),                          // VkSampler                      sampler
			texture.image.view(),                       // VkImageView                    imageView
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL    // VkImageLayout                  imageLayout
		};

		std::vector<VkWriteDescriptorSet> descriptorWrites = {
			{
				VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,     // VkStructureType                sType
				nullptr,                                    // const void                    *pNext
				texture.descriptorSet(),                    // VkDescriptorSet                dstSet
				0,                                          // uint32_t                       dstBinding
				0,                                          // uint32_t                       dstArrayElement
				1,                                          // uint32_t                       descriptorCount
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,  // VkDescriptorType               descriptorType
				&textureInfo,                               // const VkDescriptorImageInfo   *pImageInfo
				nullptr,                                    // const VkDescriptorBufferInfo  *pBufferInfo
				nullptr                                     // const VkBufferView            *pTexelBufferView
			}
		};

		// TODO: Check if update methode should be in Device class 
		texture.descriptorSet.update(static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data());

		return texture;
	}

	Material Renderer::createMaterial(const std::string_view vertexShaderName, const std::string_view fragmentShaderName) {		
		std::vector<VkDescriptorSetLayout> vkDescritptorLayouts{
			m_mainDescriptorLayout(),
			m_ssboDescriptorLayout(),
			m_textureDescriptorLayout(),
			m_lightDescriptorLayout()
		};

		Material material;
		material.createPipeline(m_vulkan.getDevice().getHandle(), m_renderWindow.getRenderPass(), vertexShaderName, fragmentShaderName, vkDescritptorLayouts);

		return material;
	}

	void Renderer::createMesh(Mesh& mesh) {
		mesh.vertexBuffer = VulkanBuffer{ 
			m_vulkan.getDevice(),
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			static_cast<uint32_t>(mesh.vertices.size() * sizeof(mesh.vertices[0]))
		};

		mesh.vertexBuffer.copy(mesh.vertices.data(), mesh.vertexBuffer.handle.getSize(), m_renderingResources[0].commandBuffer);

		mesh.indexBuffer = VulkanBuffer{
			m_vulkan.getDevice(),
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			sizeof(mesh.indices[0])* mesh.indices.size()
		};

		mesh.indexBuffer.copy(mesh.indices.data(), mesh.indexBuffer.handle.getSize(), m_renderingResources[0].commandBuffer);
	}

	void Renderer::draw(std::vector<RenderObject> const& objects) {
		m_renderWindow.draw(m_renderingResources[m_resourceIndex], objects);
		
		m_resourceIndex = (m_resourceIndex + 1) % RenderWindow::resourceCount;
	}

	Vk::DescriptorSetLayout Renderer::getMainDescriptorLayout() const {
		std::vector<VkDescriptorSetLayoutBinding> layoutBindings = {
			{
				0,                                         // uint32_t           binding
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
		if (!layout.create(m_vulkan.getDevice().getHandle(), descriptorSetLayoutCreateInfo)) {
			throw std::runtime_error("Could not create descriptor set layout!");
		}

		return layout;
	}

	Vk::DescriptorSetLayout Renderer::getTextureDescriptorLayout() const {
		std::vector<VkDescriptorSetLayoutBinding> layoutBindings = {
			{
				0,                                          // uint32_t             binding
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,  // VkDescriptorType     descriptorType
				1,                                          // uint32_t             descriptorCount
				VK_SHADER_STAGE_FRAGMENT_BIT,               // VkShaderStageFlags   stageFlags
				nullptr                                     // const VkSampler     *pImmutableSamplers
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
		if (!layout.create(m_vulkan.getDevice().getHandle(), descriptorSetLayoutCreateInfo)) {
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
			static_cast<uint32_t>(layoutBindings.size()),         // uint32_t                             bindingCount
			layoutBindings.data()                                 // const VkDescriptorSetLayoutBinding  *pBindings
		};

		Vk::DescriptorSetLayout layout;
		if (!layout.create(m_vulkan.getDevice().getHandle(), descriptorSetLayoutCreateInfo)) {
			throw std::runtime_error("Could not create descriptor set layout!");
		}

		return layout;
	}

	Vk::DescriptorSetLayout Renderer::getLightDescriptorLayout() const {
		std::vector<VkDescriptorSetLayoutBinding> layoutBindings = {
			{
				0,                                         // uint32_t           binding
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         // VkDescriptorType   descriptorType
				1,                                         // uint32_t           descriptorCount
				VK_SHADER_STAGE_FRAGMENT_BIT,                // VkShaderStageFlags stageFlags
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
		if (!layout.create(m_vulkan.getDevice().getHandle(), descriptorSetLayoutCreateInfo)) {
			throw std::runtime_error("Could not create descriptor set layout!");
		}

		return layout;
	}

	void Renderer::waitIdle() const {
		m_vulkan.getDevice().getHandle().waitIdle();
	}

	bool Renderer::createUniformBuffer() {
		for (size_t i = 0; i < m_renderingResources.size(); ++i) {
			m_renderingResources[i].viewerBuffer = VulkanBuffer{
				m_vulkan.getDevice(),
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				sizeof(ViewerGpuObject)
			};
		}

		if (!copyUniformBufferData()) {
			return false;
		}

		return true;
	}

	bool Renderer::copyUniformBufferData() {
		const ViewerGpuObject viewerData = getViewerData();

		for (size_t i = 0; i < m_renderingResources.size(); ++i) {
			RenderingResource& current = m_renderingResources[i];

			current.viewerBuffer.copy(&viewerData, current.viewerBuffer.handle.getSize(), m_renderingResources[0].commandBuffer);
		}

		return true;
	}

	void Renderer::copyLightData() {
		LightGpuObject light = {
			glm::vec4(1.f, 0.5f, 0.f, 1.f)
		};

		for (size_t i = 0; i < m_renderingResources.size(); ++i) {
			RenderingResource& current = m_renderingResources[i];

			current.lightBuffer.copy(&light, current.lightBuffer.handle.getSize(), m_renderingResources[0].commandBuffer);
		}
	}

	ViewerGpuObject Renderer::getViewerData() const {
		ViewerGpuObject ubo{};

		Vector2ui size = m_renderWindow.size();

		ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.proj = glm::perspective(glm::radians(45.0f), static_cast<float>(size.x) / static_cast<float>(size.y), 0.1f, 10.0f);
		ubo.proj[1][1] *= -1;

		return ubo;
	}

	bool Renderer::updateDescriptorSet() {
		for (size_t i = 0; i < m_renderingResources.size(); ++i) {
			VkDescriptorBufferInfo bufferInfo = {
				m_renderingResources[i].viewerBuffer.handle(),             // VkBuffer         buffer
				0,                                                       // VkDeviceSize     offset
				m_renderingResources[i].viewerBuffer.handle.getSize()      // VkDeviceSize     range
			};

			std::vector<VkWriteDescriptorSet> descriptorWrites = {
				{
					VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,    // VkStructureType     sType
					nullptr,                                   // const void         *pNext
					m_renderingResources[i].viewerDescriptor(),  // VkDescriptorSet     dstSet
					0,                                         // uint32_t            dstBinding
					0,                                         // uint32_t            dstArrayElement
					1,                                         // uint32_t            descriptorCount
					VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         // VkDescriptorType    descriptorType
					nullptr,                                   // const VkDescriptorImageInfo  *pImageInfo
					&bufferInfo,                               // const VkDescriptorBufferInfo *pBufferInfo
					nullptr                                    // const VkBufferView *pTexelBufferView
				}
			};

			// TODO: Check if update methode should be in Device class 
			m_renderingResources[i].viewerDescriptor.update(static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data());


			VkDescriptorBufferInfo ssboInfo = {
				m_renderingResources[i].ssbo.handle(),         // VkBuffer         buffer
				0,                                             // VkDeviceSize     offset
				m_renderingResources[i].ssbo.handle.getSize()  // VkDeviceSize     range
			};

			std::vector<VkWriteDescriptorSet> descriptorWrites2 = {
				{
					VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,     // VkStructureType                sType
					nullptr,                                    // const void                    *pNext
					m_renderingResources[i].ssboDescriptor(),   // VkDescriptorSet                dstSet
					0,                                          // uint32_t                       dstBinding
					0,                                          // uint32_t                       dstArrayElement
					1,                                          // uint32_t                       descriptorCount
					VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,          // VkDescriptorType               descriptorType
					nullptr,                                    // const VkDescriptorImageInfo   *pImageInfo
					&ssboInfo,                                  // const VkDescriptorBufferInfo  *pBufferInfo
					nullptr                                     // const VkBufferView            *pTexelBufferView
				},
			};

			m_renderingResources[i].ssboDescriptor.update(static_cast<uint32_t>(descriptorWrites2.size()), descriptorWrites2.data());

			// Light
			VkDescriptorBufferInfo lightInfo = {
				m_renderingResources[i].lightBuffer.handle(),         // VkBuffer         buffer
				0,                                                    // VkDeviceSize     offset
				m_renderingResources[i].lightBuffer.handle.getSize()  // VkDeviceSize     range
			};

			std::vector<VkWriteDescriptorSet> descriptorWrites3 = {
				{
					VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,     // VkStructureType                sType
					nullptr,                                    // const void                    *pNext
					m_renderingResources[i].lightDescriptor(),  // VkDescriptorSet                dstSet
					0,                                          // uint32_t                       dstBinding
					0,                                          // uint32_t                       dstArrayElement
					1,                                          // uint32_t                       descriptorCount
					VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,          // VkDescriptorType               descriptorType
					nullptr,                                    // const VkDescriptorImageInfo   *pImageInfo
					&lightInfo,                                 // const VkDescriptorBufferInfo  *pBufferInfo
					nullptr                                     // const VkBufferView            *pTexelBufferView
				},
			};

			m_renderingResources[i].lightDescriptor.update(static_cast<uint32_t>(descriptorWrites3.size()), descriptorWrites3.data());
		}

		return true;
	}

	bool Renderer::createRenderingResources() {
		for (size_t i = 0; i < m_renderingResources.size(); ++i) {
			if (!m_renderingResources[i].create(m_vulkan.getDevice().getHandle(), m_vulkan.getDevice().presentQueue().index())) {
				std::cerr << "Can't create rendering ressource" << std::endl;
				return false;
			}
		}

		return true;
	}

	bool Renderer::createSSBO() {
		for (size_t i = 0; i < m_renderingResources.size(); ++i) {
			m_renderingResources[i].ssbo = VulkanBuffer{
				m_vulkan.getDevice(),
				VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
				10000 * sizeof(ShaderStorageBufferObject)
			};
		}

		return true;
	}
}

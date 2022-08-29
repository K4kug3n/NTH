#include <Renderer/Renderer.hpp>
#include <Renderer/RenderingResource.hpp>

#include <Math/Angle.hpp>

#include <Util/Image.hpp>

#include <cstring>
#include <iostream>

namespace Nth {
	Renderer::Renderer() :
		m_vulkan(),
		m_renderWindow(m_vulkan),
		m_resourceIndex(0),
		m_renderingResources(Renderer::resourceCount) { }

	RenderWindow& Renderer::getWindow(VideoMode const& mode, const std::string_view title) {
		if (!m_renderWindow.create(mode, title)) {
			throw std::runtime_error("Can't create render window");
		}

		if (!createRenderingResources()) {
			throw std::runtime_error("Can't create rendering ressources");
		}

		m_mainDescriptorLayout = getViewerDescriptorLayout();
		m_modelDescriptorLayout = geModelDescriptorLayout();
		m_textureDescriptorLayout = getTextureDescriptorLayout();
		m_lightDescriptorLayout = getLightDescriptorLayout();

		m_descriptorAllocator.init(m_vulkan.getDevice().getHandle());

		for (size_t i = 0; i < m_renderingResources.size(); ++i) {
			m_renderingResources[i].viewerDescriptor = m_descriptorAllocator.allocate(m_mainDescriptorLayout);
			m_renderingResources[i].modelDescriptor = m_descriptorAllocator.allocate(m_modelDescriptorLayout);
			m_renderingResources[i].lightDescriptor = m_descriptorAllocator.allocate(m_lightDescriptorLayout);

			m_renderingResources[i].lightBuffer = VulkanBuffer{
				m_vulkan.getDevice(),
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
				sizeof(LightGpuObject)
			};

			m_renderingResources[i].viewerBuffer = VulkanBuffer{
				m_vulkan.getDevice(),
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
				sizeof(ViewerGpuObject)
			};

			m_renderingResources[i].modelBuffer = VulkanBuffer{
				m_vulkan.getDevice(),
				VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
				10000 * sizeof(ModelGpuObject)
			};
		}

		updateDescriptorSet();

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

	Material Renderer::createMaterial(MaterialInfos const& infos) {
		std::vector<VkDescriptorSetLayout> vkDescritptorLayouts{
			m_mainDescriptorLayout(),
			m_modelDescriptorLayout(),
			m_lightDescriptorLayout(),
			m_textureDescriptorLayout()
		};

		Material material;
		material.createPipeline(m_vulkan.getDevice().getHandle(), m_renderWindow.getRenderPass(), infos.vertexShaderName, infos.fragmentShaderName, vkDescritptorLayouts);

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
		m_renderWindow.draw(m_renderingResources[m_resourceIndex], objects, light, getViewerData());
		
		m_resourceIndex = (m_resourceIndex + 1) % Renderer::resourceCount;
	}

	Vk::DescriptorSetLayout Renderer::getViewerDescriptorLayout() const {
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

	Vk::DescriptorSetLayout Renderer::geModelDescriptorLayout() const {
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

	ViewerGpuObject Renderer::getViewerData() const {
		ViewerGpuObject ubo{};

		Vector2ui size = m_renderWindow.size();

		ubo.view = camera.getViewMatrix();
		ubo.proj = Matrix4f::Perspective(toRadians(45.0f), static_cast<float>(size.x) / static_cast<float>(size.y), 0.1f, 10.0f);
		ubo.proj.a22 *= -1;

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
				m_renderingResources[i].modelBuffer.handle(),         // VkBuffer         buffer
				0,                                             // VkDeviceSize     offset
				m_renderingResources[i].modelBuffer.handle.getSize()  // VkDeviceSize     range
			};

			std::vector<VkWriteDescriptorSet> descriptorWrites2 = {
				{
					VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,     // VkStructureType                sType
					nullptr,                                    // const void                    *pNext
					m_renderingResources[i].modelDescriptor(),   // VkDescriptorSet                dstSet
					0,                                          // uint32_t                       dstBinding
					0,                                          // uint32_t                       dstArrayElement
					1,                                          // uint32_t                       descriptorCount
					VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,          // VkDescriptorType               descriptorType
					nullptr,                                    // const VkDescriptorImageInfo   *pImageInfo
					&ssboInfo,                                  // const VkDescriptorBufferInfo  *pBufferInfo
					nullptr                                     // const VkBufferView            *pTexelBufferView
				},
			};

			m_renderingResources[i].modelDescriptor.update(static_cast<uint32_t>(descriptorWrites2.size()), descriptorWrites2.data());

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
}

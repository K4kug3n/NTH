#include <Renderer/Renderer.hpp>
#include <Renderer/RenderingResource.hpp>
#include <Renderer/RenderObject.hpp>
#include <Renderer/Model.hpp>
#include <Renderer/Texture.hpp>
#include <Renderer/ShaderBinding.hpp>

#include <Math/Angle.hpp>

#include <Util/Image.hpp>

#include <cstring>
#include <iostream>

namespace Nth {
	Renderer::Renderer() :
		m_vulkan(),
		m_renderWindow(m_vulkan),
		m_resourceIndex(0) { }

	RenderWindow& Renderer::getWindow(VideoMode const& mode, const std::string_view title) {
		if (!m_renderWindow.create(mode, title)) {
			throw std::runtime_error("Can't create render window");
		}

		size_t viewLayoutIndex = addDescriptorSetLayout({ ShaderBinding{ ShaderType::Vertex, BindingType::Uniform, 0, 0 } });
		size_t modelLayoutIndex = addDescriptorSetLayout({ ShaderBinding{ ShaderType::Vertex, BindingType::Storage, 1, 0 } });
		size_t lightLayoutIndex = addDescriptorSetLayout({ ShaderBinding{ ShaderType::Fragment, BindingType::Uniform, 2, 0 } });
		addDescriptorSetLayout({ ShaderBinding{ ShaderType::Fragment, BindingType::Texture, 3, 0 } });

		m_descriptorAllocator.init(m_vulkan.getDevice().getHandle());

		for (size_t i = 0; i < Renderer::resourceCount; ++i) {
			m_viewerDescriptors[i] = m_descriptorAllocator.allocate(m_descriptorSetLayouts[viewLayoutIndex]);
			m_modelDescriptors[i] = m_descriptorAllocator.allocate(m_descriptorSetLayouts[modelLayoutIndex]);
			m_lightDescriptors[i] = m_descriptorAllocator.allocate(m_descriptorSetLayouts[lightLayoutIndex]);

			m_lightBuffers[i] = VulkanBuffer{
				m_vulkan.getDevice(),
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
				sizeof(LightGpuObject)
			};

			m_viewerBuffers[i] = VulkanBuffer{
				m_vulkan.getDevice(),
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
				sizeof(ViewerGpuObject)
			};

			m_modelBuffers[i] = VulkanBuffer{
				m_vulkan.getDevice(),
				VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
				10000 * sizeof(ModelGpuObject)
			};
		}

		updateDescriptorSet();

		return m_renderWindow;
	}

	Material Renderer::createMaterial(MaterialInfos const& infos) {
		std::vector<VkDescriptorSetLayout> vkDescritptorLayouts(m_descriptorSetLayouts.size());
		for (size_t i = 0; i < m_descriptorSetLayouts.size(); ++i) {
			vkDescritptorLayouts[i] = m_descriptorSetLayouts[i]();
		}

		Material material;
		material.createPipeline(m_vulkan.getDevice().getHandle(), m_renderWindow.getRenderPass(), infos.vertexShaderName, infos.fragmentShaderName, vkDescritptorLayouts);

		return material;
	}

	size_t Renderer::registerModel(Model const& model) {
		std::vector<VulkanTexture> textures;
		for (auto const& texture : model.textures()) {
			textures.push_back(registerTexture(texture));
		}

		std::vector<RenderableMesh> meshes;
		for (auto const& mesh : model.meshes) {
			RenderableMesh renderableMesh{ registerMesh(mesh) };

			// TODO: Cleanup
			renderableMesh.textureIndex = 0;
			for (size_t textureIndex : mesh.texturesIndex) {
				if (model.textures()[textureIndex].type == "base_color") {
					renderableMesh.textureIndex = textureIndex;
					break;
				}
			}

			meshes.emplace_back(std::move(renderableMesh));
		}

		m_renderables.emplace_back(std::move(meshes), std::move(textures));

		return m_renderables.size() - 1;
	}

	void Renderer::draw(std::vector<RenderObject> const& objects) {
		RenderingResource& image = m_renderWindow.aquireNextImage();

		ViewerGpuObject viewer = getViewerData();

		// TODO: Move this logic
		image.prepare([this, &objects, &viewer](Vk::CommandBuffer& commandBuffer) {
			std::vector<ModelGpuObject> storageObjects(objects.size());
			for (size_t i = 0; i < storageObjects.size(); ++i) {
				storageObjects[i].model = objects[i].transformMatrix;
			}

			m_modelBuffers[m_resourceIndex].copy(storageObjects.data(), storageObjects.size() * sizeof(ModelGpuObject));

			m_lightBuffers[m_resourceIndex].copy(&light, sizeof(LightGpuObject));

			m_viewerBuffers[m_resourceIndex].copy(&viewer, sizeof(ViewerGpuObject));

			Material* lastMaterial = nullptr;
			for (size_t i = 0; i < objects.size(); ++i) {
				if (objects[i].material != lastMaterial) {
					commandBuffer.bindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, objects[i].material->pipeline());

					VkDescriptorSet vkDescriptorSet = m_viewerDescriptors[m_resourceIndex]();
					commandBuffer.bindDescriptorSets(objects[i].material->pipelineLayout(), 0, 1, &vkDescriptorSet, 0, nullptr);

					VkDescriptorSet vkSsboDescriptorSet = m_modelDescriptors[m_resourceIndex]();
					commandBuffer.bindDescriptorSets(objects[i].material->pipelineLayout(), 1, 1, &vkSsboDescriptorSet, 0, nullptr);

					VkDescriptorSet vkLightDescriptorSet = m_lightDescriptors[m_resourceIndex]();
					commandBuffer.bindDescriptorSets(objects[i].material->pipelineLayout(), 2, 1, &vkLightDescriptorSet, 0, nullptr);

					lastMaterial = objects[i].material;
				}

				VulkanTexture const* lastTexture = nullptr;
				RenderableModel const& model = m_renderables[objects[i].modelIndex];
				for (RenderableMesh const& mesh : model.meshes) {
					VkDeviceSize offset = 0;
					commandBuffer.bindVertexBuffer(mesh.vertexBuffer.handle(), offset);

					commandBuffer.bindIndexBuffer(mesh.indexBuffer.handle(), 0, VK_INDEX_TYPE_UINT32);

					VulkanTexture const& texture{ model.textures[mesh.textureIndex] };
					if (&texture != lastTexture) {
						VkDescriptorSet vkTextureDescriptorSet = texture.descriptorSet();
						commandBuffer.bindDescriptorSets(objects[i].material->pipelineLayout(), 3, 1, &vkTextureDescriptorSet, 0, nullptr);

						lastTexture = &texture;
					}

					commandBuffer.drawIndexed(static_cast<uint32_t>(mesh.indices.size()), 1, 0, 0, static_cast<uint32_t>(i));
				}
			}
		});

		image.present();
		
		m_resourceIndex = (m_resourceIndex + 1) % Renderer::resourceCount;
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
		for (size_t i = 0; i < Renderer::resourceCount; ++i) {
			VkDescriptorBufferInfo bufferInfo = {
				m_viewerBuffers[i].handle(),           // VkBuffer         buffer
				0,                                     // VkDeviceSize     offset
				m_viewerBuffers[i].handle.getSize()    // VkDeviceSize     range
			};

			std::vector<VkWriteDescriptorSet> descriptorWrites = {
				{
					VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,    // VkStructureType     sType
					nullptr,                                   // const void         *pNext
					m_viewerDescriptors[i](),                  // VkDescriptorSet     dstSet
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
			m_viewerDescriptors[i].update(static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data());


			VkDescriptorBufferInfo ssboInfo = {
				m_modelBuffers[i].handle(),         // VkBuffer         buffer
				0,                                  // VkDeviceSize     offset
				m_modelBuffers[i].handle.getSize()  // VkDeviceSize     range
			};

			std::vector<VkWriteDescriptorSet> descriptorWrites2 = {
				{
					VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,     // VkStructureType                sType
					nullptr,                                    // const void                    *pNext
					m_modelDescriptors[i](),                    // VkDescriptorSet                dstSet
					0,                                          // uint32_t                       dstBinding
					0,                                          // uint32_t                       dstArrayElement
					1,                                          // uint32_t                       descriptorCount
					VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,          // VkDescriptorType               descriptorType
					nullptr,                                    // const VkDescriptorImageInfo   *pImageInfo
					&ssboInfo,                                  // const VkDescriptorBufferInfo  *pBufferInfo
					nullptr                                     // const VkBufferView            *pTexelBufferView
				},
			};

			m_modelDescriptors[i].update(static_cast<uint32_t>(descriptorWrites2.size()), descriptorWrites2.data());

			// Light
			VkDescriptorBufferInfo lightInfo = {
				m_lightBuffers[i].handle(),          // VkBuffer         buffer
				0,                                   // VkDeviceSize     offset
				m_lightBuffers[i].handle.getSize()   // VkDeviceSize     range
			};

			std::vector<VkWriteDescriptorSet> descriptorWrites3 = {
				{
					VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,     // VkStructureType                sType
					nullptr,                                    // const void                    *pNext
					m_lightDescriptors[i](),                    // VkDescriptorSet                dstSet
					0,                                          // uint32_t                       dstBinding
					0,                                          // uint32_t                       dstArrayElement
					1,                                          // uint32_t                       descriptorCount
					VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,          // VkDescriptorType               descriptorType
					nullptr,                                    // const VkDescriptorImageInfo   *pImageInfo
					&lightInfo,                                 // const VkDescriptorBufferInfo  *pBufferInfo
					nullptr                                     // const VkBufferView            *pTexelBufferView
				},
			};

			m_lightDescriptors[i].update(static_cast<uint32_t>(descriptorWrites3.size()), descriptorWrites3.data());
		}

		return true;
	}

	size_t Renderer::addDescriptorSetLayout(std::vector<ShaderBinding> const& bindings) {
		// Use "set" information
		std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
		for (auto const& binding : bindings) {
			VkDescriptorSetLayoutBinding layoutBinding;
			layoutBinding.binding = binding.binding;

			switch (binding.bindingType) {
			case BindingType::Uniform:
				layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				break;
			case BindingType::Texture:
				layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				break;
			case BindingType::Storage:
				layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
				break;
			}

			layoutBinding.descriptorCount = 1;

			switch (binding.shaderType) {
			case ShaderType::Vertex:
				layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
				break;
			case ShaderType::Fragment:
				layoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
				break;
			}

			layoutBinding.pImmutableSamplers = nullptr;

			layoutBindings.push_back(std::move(layoutBinding));
		}

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

		m_descriptorSetLayouts.push_back(std::move(layout));

		return m_descriptorSetLayouts.size() - 1;
	}

	RenderableMesh Renderer::registerMesh(Mesh const& mesh) const {
		RenderableMesh registeredMesh;

		registeredMesh.vertexBuffer = VulkanBuffer{
			m_vulkan.getDevice(),
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			static_cast<uint32_t>(mesh.vertices.size() * sizeof(mesh.vertices[0]))
		};

		registeredMesh.vertexBuffer.copy(mesh.vertices.data(), registeredMesh.vertexBuffer.handle.getSize());

		registeredMesh.indexBuffer = VulkanBuffer{
			m_vulkan.getDevice(),
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			sizeof(mesh.indices[0]) * mesh.indices.size()
		};

		registeredMesh.indexBuffer.copy(mesh.indices.data(), registeredMesh.indexBuffer.handle.getSize());

		registeredMesh.indices = mesh.indices;

		return registeredMesh;
	}

	VulkanTexture Renderer::registerTexture(Texture const& texture) {
		VulkanTexture registeredTexture;

		registeredTexture.create(
			m_vulkan.getDevice(),
			texture.width,
			texture.height,
			static_cast<uint32_t>(texture.data.size()),
			VK_FORMAT_R8G8B8A8_UNORM,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		registeredTexture.createView(VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);

		registeredTexture.image.copy(texture.data.data(), static_cast<uint32_t>(texture.data.size()), texture.width, texture.height);

		// TODO: descriptor set layout index hardcoded
		registeredTexture.descriptorSet = m_descriptorAllocator.allocate(m_descriptorSetLayouts[3]);

		VkDescriptorImageInfo textureInfo = {
			registeredTexture.sampler(),                          // VkSampler                      sampler
			registeredTexture.image.view(),                       // VkImageView                    imageView
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL              // VkImageLayout                  imageLayout
		};

		std::vector<VkWriteDescriptorSet> descriptorWrites = {
			{
				VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,     // VkStructureType                sType
				nullptr,                                    // const void                    *pNext
				registeredTexture.descriptorSet(),          // VkDescriptorSet                dstSet
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
		registeredTexture.descriptorSet.update(static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data());

		return registeredTexture;
	}
}

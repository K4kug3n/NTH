#include <Renderer/Renderer.hpp>
#include <Renderer/RenderingResource.hpp>
#include <Renderer/RenderObject.hpp>
#include <Renderer/Model.hpp>
#include <Renderer/Texture.hpp>

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

		size_t viewLayoutIndex = addDescriptorSetLayout({ BindingInfo{ ShaderType::Vertex, BindingType::Uniform, 0, 0 } });
		size_t modelLayoutIndex = addDescriptorSetLayout({ BindingInfo{ ShaderType::Vertex, BindingType::Storage, 1, 0 } });
		size_t lightLayoutIndex = addDescriptorSetLayout({ BindingInfo{ ShaderType::Fragment, BindingType::Uniform, 2, 0 } });
		addDescriptorSetLayout({ BindingInfo{ ShaderType::Fragment, BindingType::Texture, 3, 0 } });

		m_descriptorAllocator.init(m_vulkan.getDevice().getHandle());

		for (size_t i = 0; i < Renderer::resourceCount; ++i) {
			m_viewerBindings[i] = ShaderBinding{ m_descriptorAllocator.allocate(m_descriptorSetLayouts[viewLayoutIndex]) };
			m_modelBindings[i] = ShaderBinding{ m_descriptorAllocator.allocate(m_descriptorSetLayouts[modelLayoutIndex]) };
			m_lightBindings[i] = ShaderBinding{ m_descriptorAllocator.allocate(m_descriptorSetLayouts[lightLayoutIndex]) };

			m_lightBuffers[i] = RenderBuffer{
				m_vulkan.getDevice(),
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
				sizeof(LightGpuObject)
			};

			m_viewerBuffers[i] = RenderBuffer{
				m_vulkan.getDevice(),
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
				sizeof(ViewerGpuObject)
			};

			m_modelBuffers[i] = RenderBuffer{
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
		std::vector<RenderTexture> textures;
		for (auto const& texture : model.textures()) {
			textures.push_back(registerTexture(texture));
		}

		std::vector<RenderMesh> meshes;
		for (auto const& mesh : model.meshes) {
			RenderMesh RenderMesh{ registerMesh(mesh) };

			// TODO: Cleanup
			RenderMesh.textureIndex = 0;
			for (size_t textureIndex : mesh.texturesIndex) {
				if (model.textures()[textureIndex].type == "base_color") {
					RenderMesh.textureIndex = textureIndex;
					break;
				}
			}

			meshes.emplace_back(std::move(RenderMesh));
		}

		m_Renders.emplace_back(std::move(meshes), std::move(textures));

		return m_Renders.size() - 1;
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

					VkDescriptorSet vkDescriptorSet = m_viewerBindings[m_resourceIndex].descriptorSet()();
					commandBuffer.bindDescriptorSets(objects[i].material->pipelineLayout(), 0, 1, &vkDescriptorSet, 0, nullptr);

					VkDescriptorSet vkSsboDescriptorSet = m_modelBindings[m_resourceIndex].descriptorSet()();
					commandBuffer.bindDescriptorSets(objects[i].material->pipelineLayout(), 1, 1, &vkSsboDescriptorSet, 0, nullptr);

					VkDescriptorSet vkLightDescriptorSet = m_lightBindings[m_resourceIndex].descriptorSet()();
					commandBuffer.bindDescriptorSets(objects[i].material->pipelineLayout(), 2, 1, &vkLightDescriptorSet, 0, nullptr);

					lastMaterial = objects[i].material;
				}

				RenderTexture const* lastTexture = nullptr;
				RenderModel const& model = m_Renders[objects[i].modelIndex];
				for (RenderMesh const& mesh : model.meshes) {
					VkDeviceSize offset = 0;
					commandBuffer.bindVertexBuffer(mesh.vertexBuffer.handle(), offset);

					commandBuffer.bindIndexBuffer(mesh.indexBuffer.handle(), 0, VK_INDEX_TYPE_UINT32);

					RenderTexture const& texture{ model.textures[mesh.textureIndex] };
					if (&texture != lastTexture) {
						VkDescriptorSet vkTextureDescriptorSet = texture.binding.descriptorSet()();
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
			UniformBinding viewerUniform{ m_viewerBuffers[i], 0, m_viewerBuffers[i].handle.getSize() };
			m_viewerBindings[i].update({ Binding{ viewerUniform, 0 } });

			StorageBinding modelStorage{ m_modelBuffers[i], 0, m_modelBuffers[i].handle.getSize() };
			m_modelBindings[i].update({ Binding{ modelStorage, 0 } });

			UniformBinding lightUniform{ m_lightBuffers[i], 0, m_lightBuffers[i].handle.getSize() };
			m_lightBindings[i].update({ Binding{ lightUniform, 0 } });
		}

		return true;
	}

	size_t Renderer::addDescriptorSetLayout(std::vector<BindingInfo> const& bindings) {
		// Use "set" information
		std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
		for (auto const& binding : bindings) {
			VkDescriptorSetLayoutBinding layoutBinding;
			layoutBinding.binding = binding.bindingIndex;

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

	ShaderBinding Renderer::allocateShaderBinding(size_t index) {
		assert(index < m_descriptorSetLayouts.size());

		return ShaderBinding(m_descriptorAllocator.allocate(m_descriptorSetLayouts[index]));
	}

	RenderMesh Renderer::registerMesh(Mesh const& mesh) const {
		RenderMesh registeredMesh;

		registeredMesh.vertexBuffer = RenderBuffer{
			m_vulkan.getDevice(),
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			static_cast<uint32_t>(mesh.vertices.size() * sizeof(mesh.vertices[0]))
		};

		registeredMesh.vertexBuffer.copy(mesh.vertices.data(), registeredMesh.vertexBuffer.handle.getSize());

		registeredMesh.indexBuffer = RenderBuffer{
			m_vulkan.getDevice(),
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			sizeof(mesh.indices[0]) * mesh.indices.size()
		};

		registeredMesh.indexBuffer.copy(mesh.indices.data(), registeredMesh.indexBuffer.handle.getSize());

		registeredMesh.indices = mesh.indices;

		return registeredMesh;
	}

	RenderTexture Renderer::registerTexture(Texture const& texture) {
		RenderTexture registeredTexture;

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
		registeredTexture.binding = allocateShaderBinding(3);

		TextureBinding textureBind{ registeredTexture, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
		registeredTexture.binding.update({ Binding{ textureBind, 0 } });

		return registeredTexture;
	}
}

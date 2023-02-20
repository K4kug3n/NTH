#include <Renderer/Renderer.hpp>
#include <Renderer/RenderingResource.hpp>
#include <Renderer/RenderObject.hpp>
#include <Renderer/Model.hpp>
#include <Renderer/Texture.hpp>

#include <Window/Window.hpp>
#include <Window/WindowHandle.hpp>

#include <Maths/Angle.hpp>

#include <Utils/Image.hpp>

#include <cstring>
#include <iostream>

namespace Nth {
	Renderer::Renderer() :
		m_vulkan(),
		m_render_surface(m_vulkan),
		m_resourceIndex(0),
		m_renders(),
		m_descriptor_allocator(),
		m_light_bindings(), 
		m_light_buffers(),
		light(),
		camera(),
		m_window(nullptr) { }

	void Renderer::set_render_on(Window& window) {
		m_window = &window;
		m_render_surface.create(window.handle());
		m_vulkan.create_device(m_render_surface.get_handle());
		m_render_surface.init_render_pipeline(window.size());

		size_t viewLayoutIndex = add_descriptor_set_layout({ BindingInfo{ ShaderType::Vertex, BindingType::Uniform, 0, 0 } });
		size_t modelLayoutIndex = add_descriptor_set_layout({ BindingInfo{ ShaderType::Vertex, BindingType::Storage, 1, 0 } });
		size_t lightLayoutIndex = add_descriptor_set_layout({ BindingInfo{ ShaderType::Fragment, BindingType::Uniform, 2, 0 } });
		add_descriptor_set_layout({ BindingInfo{ ShaderType::Fragment, BindingType::Texture, 3, 0 } });

		m_descriptor_allocator.init(m_vulkan.get_device().get_handle());

		for (size_t i = 0; i < Renderer::resource_count; ++i) {
			m_viewer_bindings[i] = ShaderBinding{ m_descriptor_allocator.allocate(m_descriptor_set_layouts[viewLayoutIndex]) };
			m_model_bindings[i] = ShaderBinding{ m_descriptor_allocator.allocate(m_descriptor_set_layouts[modelLayoutIndex]) };
			m_light_bindings[i] = ShaderBinding{ m_descriptor_allocator.allocate(m_descriptor_set_layouts[lightLayoutIndex]) };

			m_light_buffers[i] = RenderBuffer{
				m_vulkan.get_device(),
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
				sizeof(LightGpuObject)
			};

			m_viewer_buffers[i] = RenderBuffer{
				m_vulkan.get_device(),
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
				sizeof(ViewerGpuObject)
			};

			m_model_buffers[i] = RenderBuffer{
				m_vulkan.get_device(),
				VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
				10000 * sizeof(ModelGpuObject)
			};
		}

		update_descriptor_set();
	}

	Material Renderer::create_material(const MaterialInfos& infos) {
		std::vector<VkDescriptorSetLayout> vk_descritptor_layouts(m_descriptor_set_layouts.size());
		for (size_t i = 0; i < m_descriptor_set_layouts.size(); ++i) {
			vk_descritptor_layouts[i] = m_descriptor_set_layouts[i]();
		}

		Material material;
		material.create_pipeline(m_vulkan.get_device().get_handle(), m_render_surface.get_render_pass(), infos.vertexShaderName, infos.fragmentShaderName, vk_descritptor_layouts);

		return material;
	}

	size_t Renderer::register_model(const Model& model) {
		std::vector<RenderTexture> textures;
		for (const auto& texture : model.textures()) {
			textures.push_back(register_texture(texture));
		}

		std::vector<RenderMesh> meshes;
		for (const auto& mesh : model.meshes) {
			RenderMesh RenderMesh{ register_mesh(mesh) };

			// TODO: Cleanup
			RenderMesh.texture_index = 0;
			for (size_t textureIndex : mesh.textures_index) {
				if (model.textures()[textureIndex].type == "base_color") {
					RenderMesh.texture_index = textureIndex;
					break;
				}
			}

			meshes.emplace_back(std::move(RenderMesh));
		}

		m_renders.emplace_back(std::move(meshes), std::move(textures));

		return m_renders.size() - 1;
	}

	void Renderer::draw(const std::vector<RenderObject>& objects) {
		assert(m_window != nullptr);
		RenderingResource& image = m_render_surface.aquire_next_image(m_window->size());

		ViewerGpuObject viewer = get_viewer_data();

		// TODO: Move this logic
		image.prepare([this, &objects, &viewer](Vk::CommandBuffer& commandBuffer) {
			std::vector<ModelGpuObject> storageObjects(objects.size());
			for (size_t i = 0; i < storageObjects.size(); ++i) {
				storageObjects[i].model = objects[i].transform_matrix;
			}

			m_model_buffers[m_resourceIndex].copy(storageObjects.data(), storageObjects.size() * sizeof(ModelGpuObject));

			m_light_buffers[m_resourceIndex].copy(&light, sizeof(LightGpuObject));

			m_viewer_buffers[m_resourceIndex].copy(&viewer, sizeof(ViewerGpuObject));

			Material* lastMaterial = nullptr;
			for (size_t i = 0; i < objects.size(); ++i) {
				if (objects[i].material != lastMaterial) {
					commandBuffer.bind_pipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, objects[i].material->pipeline());

					VkDescriptorSet vkDescriptorSet = m_viewer_bindings[m_resourceIndex].descriptor_set()();
					commandBuffer.bind_descriptor_sets(objects[i].material->pipeline_layout(), 0, 1, &vkDescriptorSet, 0, nullptr);

					VkDescriptorSet vkSsboDescriptorSet = m_model_bindings[m_resourceIndex].descriptor_set()();
					commandBuffer.bind_descriptor_sets(objects[i].material->pipeline_layout(), 1, 1, &vkSsboDescriptorSet, 0, nullptr);

					VkDescriptorSet vkLightDescriptorSet = m_light_bindings[m_resourceIndex].descriptor_set()();
					commandBuffer.bind_descriptor_sets(objects[i].material->pipeline_layout(), 2, 1, &vkLightDescriptorSet, 0, nullptr);

					lastMaterial = objects[i].material;
				}

				RenderTexture const* last_texture = nullptr;
				const RenderModel& model = m_renders[objects[i].model_index];
				for (const RenderMesh& mesh : model.meshes) {
					VkDeviceSize offset = 0;
					commandBuffer.bind_vertex_buffer(mesh.vertex_buffer.handle(), offset);

					commandBuffer.bind_index_buffer(mesh.index_buffer.handle(), 0, VK_INDEX_TYPE_UINT32);

					const RenderTexture& texture{ model.textures[mesh.texture_index] };
					if (&texture != last_texture) {
						VkDescriptorSet vkTextureDescriptorSet = texture.binding.descriptor_set()();
						commandBuffer.bind_descriptor_sets(objects[i].material->pipeline_layout(), 3, 1, &vkTextureDescriptorSet, 0, nullptr);

						last_texture = &texture;
					}

					commandBuffer.draw_indexed(static_cast<uint32_t>(mesh.indices.size()), 1, 0, 0, static_cast<uint32_t>(i));
				}
			}
		});

		image.present(m_window->size());
		
		m_resourceIndex = (m_resourceIndex + 1) % Renderer::resource_count;
	}

	void Renderer::wait_idle() const {
		m_vulkan.get_device().get_handle().wait_idle();
	}

	ViewerGpuObject Renderer::get_viewer_data() const {
		ViewerGpuObject ubo{};

		Vector2ui size = m_render_surface.size();

		ubo.view = camera.get_view_matrix();
		ubo.proj = Matrix4f::Perspective(to_radians(45.0f), static_cast<float>(size.x) / static_cast<float>(size.y), 0.1f, 10.0f);
		ubo.proj.a22 *= -1;

		return ubo;
	}

	void Renderer::update_descriptor_set() {
		for (size_t i = 0; i < Renderer::resource_count; ++i) {
			UniformBinding viewerUniform{ m_viewer_buffers[i], 0, m_viewer_buffers[i].handle.get_size() };
			m_viewer_bindings[i].update({ Binding{ viewerUniform, 0 } });

			StorageBinding modelStorage{ m_model_buffers[i], 0, m_model_buffers[i].handle.get_size() };
			m_model_bindings[i].update({ Binding{ modelStorage, 0 } });

			UniformBinding lightUniform{ m_light_buffers[i], 0, m_light_buffers[i].handle.get_size() };
			m_light_bindings[i].update({ Binding{ lightUniform, 0 } });
		}
	}

	size_t Renderer::add_descriptor_set_layout(const std::vector<BindingInfo>& bindings) {
		// Use "set" information
		std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
		for (const auto& binding : bindings) {
			VkDescriptorSetLayoutBinding layoutBinding;
			layoutBinding.binding = binding.binding_index;

			switch (binding.binding_type) {
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

			switch (binding.shader_type) {
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
		layout.create(m_vulkan.get_device().get_handle(), descriptorSetLayoutCreateInfo);

		m_descriptor_set_layouts.push_back(std::move(layout));

		return m_descriptor_set_layouts.size() - 1;
	}

	ShaderBinding Renderer::allocate_shader_binding(size_t index) {
		assert(index < m_descriptor_set_layouts.size());

		return ShaderBinding(m_descriptor_allocator.allocate(m_descriptor_set_layouts[index]));
	}

	RenderMesh Renderer::register_mesh(const Mesh& mesh) const {
		RenderMesh registered_mesh;

		registered_mesh.vertex_buffer = RenderBuffer{
			m_vulkan.get_device(),
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			static_cast<uint32_t>(mesh.vertices.size() * sizeof(mesh.vertices[0]))
		};

		registered_mesh.vertex_buffer.copy(mesh.vertices.data(), registered_mesh.vertex_buffer.handle.get_size());

		registered_mesh.index_buffer = RenderBuffer{
			m_vulkan.get_device(),
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			sizeof(mesh.indices[0]) * mesh.indices.size()
		};

		registered_mesh.index_buffer.copy(mesh.indices.data(), registered_mesh.index_buffer.handle.get_size());

		registered_mesh.indices = mesh.indices;

		return registered_mesh;
	}

	RenderTexture Renderer::register_texture(const Texture& texture) {
		RenderTexture registered_texture;

		registered_texture.create(
			m_vulkan.get_device(),
			texture.width,
			texture.height,
			static_cast<uint32_t>(texture.data.size()),
			VK_FORMAT_R8G8B8A8_UNORM,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		registered_texture.createView(VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);

		registered_texture.image.copy(texture.data.data(), static_cast<uint32_t>(texture.data.size()), texture.width, texture.height);

		// TODO: descriptor set layout index hardcoded
		registered_texture.binding = allocate_shader_binding(3);

		TextureBinding textureBind{ registered_texture, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
		registered_texture.binding.update({ Binding{ textureBind, 0 } });

		return registered_texture;
	}
}

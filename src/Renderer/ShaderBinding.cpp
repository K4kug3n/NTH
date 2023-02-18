#include <Renderer/ShaderBinding.hpp>

#include <Renderer/RenderBuffer.hpp>
#include <Renderer/RenderTexture.hpp>

#include <utility>

template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...)->overload<Ts...>;

namespace Nth {
	ShaderBinding::ShaderBinding(Vk::DescriptorSet&& descriptor) :
		m_descriptor_set(std::move(descriptor)) { }

	void ShaderBinding::update(const std::vector<Binding>& bindings) {
		std::vector<VkWriteDescriptorSet> descriptor_writes;

		std::vector<VkDescriptorBufferInfo> buffer_infos(bindings.size());
		std::vector<VkDescriptorImageInfo> image_infos(bindings.size());

		for (const Binding& binding : bindings) {
			VkWriteDescriptorSet write = {
				VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,     // VkStructureType                sType
				nullptr,                                    // const void                    *pNext
				m_descriptor_set(),                          // VkDescriptorSet                dstSet
				binding.dstIndex,                           // uint32_t                       dstBinding
				0,                                          // uint32_t                       dstArrayElement
				1,                                          // uint32_t                       descriptorCount
				VK_DESCRIPTOR_TYPE_MAX_ENUM,                // VkDescriptorType               descriptorType
				nullptr,                                    // const VkDescriptorImageInfo   *pImageInfo
				nullptr,                                    // const VkDescriptorBufferInfo  *pBufferInfo
				nullptr                                     // const VkBufferView            *pTexelBufferView
			};

			std::visit(overload{
				[&descriptor_writes, &write, &buffer_infos](const UniformBinding& uniform) {
					VkDescriptorBufferInfo& ubo_info = buffer_infos.emplace_back();
					ubo_info.buffer = uniform.buffer.handle();
					ubo_info.offset = uniform.offset;
					ubo_info.range = uniform.range;

					write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
					write.pBufferInfo = &ubo_info;
				},
				[&descriptor_writes, &write, &buffer_infos](const StorageBinding& storage) {
					VkDescriptorBufferInfo& ssbo_info = buffer_infos.emplace_back();
					ssbo_info.buffer = storage.buffer.handle();
					ssbo_info.offset = storage.offset;
					ssbo_info.range = storage.range;

					write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
					write.pBufferInfo = &ssbo_info;
				},
				[&descriptor_writes, &write, &image_infos](const TextureBinding& texture) {
					VkDescriptorImageInfo& texture_info = image_infos.emplace_back();
					texture_info.sampler = texture.texture.sampler();
					texture_info.imageView = texture.texture.image.view();
					texture_info.imageLayout = texture.layout;

					write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
					write.pImageInfo = &texture_info;
				}
			}, binding.info);

			descriptor_writes.push_back(std::move(write));
		}

		m_descriptor_set.update(static_cast<uint32_t>(descriptor_writes.size()), descriptor_writes.data());
	}

	const Vk::DescriptorSet& ShaderBinding::descriptor_set() const {
		return m_descriptor_set;
	}
}

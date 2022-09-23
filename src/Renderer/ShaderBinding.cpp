#include <Renderer/ShaderBinding.hpp>

#include <Renderer/RenderBuffer.hpp>
#include <Renderer/RenderTexture.hpp>

#include <utility>

template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...)->overload<Ts...>;

namespace Nth {
	ShaderBinding::ShaderBinding(Vk::DescriptorSet&& descriptor) :
		m_descriptorSet(std::move(descriptor)) { }

	void ShaderBinding::update(const std::vector<Binding>& bindings) {
		std::vector<VkWriteDescriptorSet> descriptorWrites;

		std::vector<VkDescriptorBufferInfo> bufferInfos(bindings.size());
		std::vector<VkDescriptorImageInfo> imageInfos(bindings.size());

		for (Binding const& binding : bindings) {
			VkWriteDescriptorSet write = {
				VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,     // VkStructureType                sType
				nullptr,                                    // const void                    *pNext
				m_descriptorSet(),                          // VkDescriptorSet                dstSet
				binding.dstIndex,                           // uint32_t                       dstBinding
				0,                                          // uint32_t                       dstArrayElement
				1,                                          // uint32_t                       descriptorCount
				VK_DESCRIPTOR_TYPE_MAX_ENUM,                // VkDescriptorType               descriptorType
				nullptr,                                    // const VkDescriptorImageInfo   *pImageInfo
				nullptr,                                    // const VkDescriptorBufferInfo  *pBufferInfo
				nullptr                                     // const VkBufferView            *pTexelBufferView
			};

			std::visit(overload{
				[&descriptorWrites, &write, &bufferInfos](UniformBinding const& uniform) {
					VkDescriptorBufferInfo& uboInfo = bufferInfos.emplace_back();
					uboInfo.buffer = uniform.buffer.handle();
					uboInfo.offset = uniform.offset;
					uboInfo.range = uniform.range;

					write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
					write.pBufferInfo = &uboInfo;
				},
				[&descriptorWrites, &write, &bufferInfos](StorageBinding const& storage) {
					VkDescriptorBufferInfo& ssboInfo = bufferInfos.emplace_back();
					ssboInfo.buffer = storage.buffer.handle();
					ssboInfo.offset = storage.offset;
					ssboInfo.range = storage.range;

					write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
					write.pBufferInfo = &ssboInfo;
				},
				[&descriptorWrites, &write, &imageInfos](TextureBinding const& texture) {
					VkDescriptorImageInfo& textureInfo = imageInfos.emplace_back();
					textureInfo.sampler = texture.texture.sampler();
					textureInfo.imageView = texture.texture.image.view();
					textureInfo.imageLayout = texture.layout;

					write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
					write.pImageInfo = &textureInfo;
				}
			}, binding.info);

			descriptorWrites.push_back(std::move(write));
		}

		m_descriptorSet.update(static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data());
	}

	const Vk::DescriptorSet& ShaderBinding::descriptorSet() const {
		return m_descriptorSet;
	}
}

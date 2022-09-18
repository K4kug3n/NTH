#ifndef NTH_RENDERER_SHADERBINDING_HPP
#define NTH_RENDERER_SHADERBINDING_HPP

#include <Renderer/Vulkan/DescriptorSet.hpp>

#include <cstdint>
#include <variant>
#include <vector>

namespace Nth {
	class VulkanBuffer;
	class VulkanTexture;

	enum class ShaderType {
		Fragment,
		Vertex
	};

	enum class BindingType {
		Uniform,
		Texture,
		Storage
	};

	struct BindingInfo {
		ShaderType shaderType;
		BindingType bindingType;
		uint32_t setIndex;
		uint32_t bindingIndex;
	};

	struct UniformBinding {
		const VulkanBuffer& buffer;
		uint64_t offset;
		uint64_t range;
	};

	struct TextureBinding {
		const VulkanTexture& texture;
		VkImageLayout layout;
	};

	struct StorageBinding {
		const VulkanBuffer& buffer;
		uint64_t offset;
		uint64_t range;
	};

	struct Binding {
		std::variant<UniformBinding, TextureBinding, StorageBinding> info;
		uint32_t dstIndex;
	};

	class ShaderBinding {
	public:
		ShaderBinding() = default;
		ShaderBinding(Vk::DescriptorSet&& descriptor);
		ShaderBinding(const ShaderBinding&) = delete;
		ShaderBinding(ShaderBinding&&) = default;

		void update(const std::vector<Binding>& bindings);
		const Vk::DescriptorSet& descriptorSet() const;

		ShaderBinding& operator=(const ShaderBinding&) = delete;
		ShaderBinding& operator=(ShaderBinding&&) = default;
	private:
		Vk::DescriptorSet m_descriptorSet;
	};
}

#endif
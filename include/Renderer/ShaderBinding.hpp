#ifndef NTH_RENDERER_SHADERBINDING_HPP
#define NTH_RENDERER_SHADERBINDING_HPP

#include <Renderer/Vulkan/DescriptorSet.hpp>

#include <cstdint>
#include <variant>
#include <vector>

namespace Nth {
	class RenderBuffer;
	class RenderTexture;

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
		ShaderType shader_type;
		BindingType binding_type;
		uint32_t set_index;
		uint32_t binding_index;
	};

	struct UniformBinding {
		const RenderBuffer& buffer;
		uint64_t offset;
		uint64_t range;
	};

	struct TextureBinding {
		const RenderTexture& texture;
		VkImageLayout layout;
	};

	struct StorageBinding {
		const RenderBuffer& buffer;
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
		const Vk::DescriptorSet& descriptor_set() const;

		ShaderBinding& operator=(const ShaderBinding&) = delete;
		ShaderBinding& operator=(ShaderBinding&&) = default;
	private:
		Vk::DescriptorSet m_descriptor_set;
	};
}

#endif
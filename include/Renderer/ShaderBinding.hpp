#ifndef NTH_RENDERER_SHADERBINDING_HPP
#define NTH_RENDERER_SHADERBINDING_HPP

#include <cstdint>

namespace Nth {
	enum class ShaderType {
		Fragment,
		Vertex
	};

	enum class BindingType {
		Uniform,
		Texture,
		Storage
	};

	struct ShaderBinding {
		ShaderBinding(ShaderType shaderType, BindingType bindingType, uint32_t set, uint32_t binding);

		ShaderType shaderType;
		BindingType bindingType;
		uint32_t set;
		uint32_t binding;
	};
}

#endif
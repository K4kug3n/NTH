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
		uint32_t binding;
		uint32_t set;
		ShaderType shaderType;
		BindingType bindingType;
	};
}

#endif
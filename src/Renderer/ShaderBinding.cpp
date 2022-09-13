#include <Renderer/ShaderBinding.hpp>


namespace Nth {
	ShaderBinding::ShaderBinding(ShaderType shaderType, BindingType bindingType, uint32_t set, uint32_t binding) :
		shaderType(shaderType),
		bindingType(bindingType),
		set(set),
		binding(binding) { }
}

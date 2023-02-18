#ifndef NTH_RENDERER_RENDEROBJECT_HPP
#define NTH_RENDERER_RENDEROBJECT_HPP

#include <Maths/Matrix4.hpp>

namespace Nth {
	class Material;

	class RenderObject {
	public:
		size_t model_index;
		Material* material;

		Matrix4f transform_matrix;
	};
}

#endif
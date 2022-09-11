#ifndef NTH_RENDERER_RENDEROBJECT_HPP
#define NTH_RENDERER_RENDEROBJECT_HPP

#include <Math/Matrix4.hpp>

namespace Nth {
	class Material;

	class RenderObject {
	public:
		size_t modelIndex;
		Material* material;

		Matrix4f transformMatrix;
	};
}

#endif
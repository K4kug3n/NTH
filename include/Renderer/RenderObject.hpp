#ifndef NTH_RENDERER_RENDEROBJECT_HPP
#define NTH_RENDERER_RENDEROBJECT_HPP

#include <Math/Matrix4.hpp>

namespace Nth {
	struct Mesh;
	class Material;
	class VulkanTexture;

	class RenderObject {
	public:
		Mesh* mesh;
		Material* material;
		VulkanTexture* texture;

		Matrix4f transformMatrix;
	};
}

#endif
#ifndef NTH_RENDERER_RENDEROBJECT_HPP
#define NTH_RENDERER_RENDEROBJECT_HPP

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Nth {
	struct Mesh;
	class Material;
	class VulkanTexture;

	class RenderObject {
	public:
		Mesh* mesh;
		Material* material;
		VulkanTexture* texture;

		glm::mat4 transformMatrix;
	};
}

#endif
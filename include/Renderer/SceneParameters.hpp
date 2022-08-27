#ifndef NTH_RENDERER_SCENEPARAMETERS_HPP
#define NTH_RENDERER_SCENEPARAMETERS_HPP

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct LightGpuObject {
	alignas(16) glm::vec3 viewPos;

	alignas(16) glm::vec4 lightColor;
	alignas(16) glm::vec3 lightPos;
	alignas(4) float ambientStrength;
	alignas(4) float specularStrength;
};

// TODO: Precalculate VP CPU-side
struct ViewerGpuObject {
	glm::mat4 view;
	glm::mat4 proj;
};

struct ModelGpuObject {
	glm::mat4 model;
};

#endif
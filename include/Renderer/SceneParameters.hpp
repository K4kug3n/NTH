#ifndef NTH_RENDERER_SCENEPARAMETERS_HPP
#define NTH_RENDERER_SCENEPARAMETERS_HPP

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct LightGpuObject {
	alignas(16) glm::vec4 ambiantColor;
	alignas(4) float ambientStrength;
	alignas(16) glm::vec3 diffusePos;
	alignas(16) glm::vec4 diffuseColor;
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
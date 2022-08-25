#ifndef NTH_RENDERER_VERTEX_HPP
#define NTH_RENDERER_VERTEX_HPP

#include <vector>

#include <vulkan/vulkan.h>

#include <Math/Vector3.hpp>
#include <Math/Vector2.hpp>

namespace Nth {
	
	struct VertexInputDescription {
		std::vector<VkVertexInputBindingDescription> bindings;
		std::vector<VkVertexInputAttributeDescription> attributes;

		VkPipelineVertexInputStateCreateFlags flags = 0;
	};

	struct Vertex {
		Vector3f pos;
		Vector2f texturePos;
		Vector3f normal;

		static VertexInputDescription getVertexDescription();
	};

}

#endif

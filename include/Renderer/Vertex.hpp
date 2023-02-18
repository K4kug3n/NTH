#ifndef NTH_RENDERER_VERTEX_HPP
#define NTH_RENDERER_VERTEX_HPP

#include <vector>

#include <vulkan/vulkan.h>

#include <Maths/Vector3.hpp>
#include <Maths/Vector2.hpp>

namespace Nth {
	
	struct VertexInputDescription {
		std::vector<VkVertexInputBindingDescription> bindings;
		std::vector<VkVertexInputAttributeDescription> attributes;

		VkPipelineVertexInputStateCreateFlags flags = 0;
	};

	struct Vertex {
		Vector3f pos;
		Vector2f texture_pos;
		Vector3f normal;

		static VertexInputDescription get_vertex_description();
	};

}

#endif

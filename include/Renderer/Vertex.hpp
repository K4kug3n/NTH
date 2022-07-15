#ifndef NTH_RENDERER_VERTEX_HPP
#define NTH_RENDERER_VERTEX_HPP

#include <vector>

#include <vulkan/vulkan.h>

namespace Nth {
	
	struct VertexInputDescription {
		std::vector<VkVertexInputBindingDescription> bindings;
		std::vector<VkVertexInputAttributeDescription> attributes;

		VkPipelineVertexInputStateCreateFlags flags = 0;
	};

	struct Vertex {
		float x, y, z, w;
		float u, v;

		static VertexInputDescription getVertexDescription();
	};

}

#endif

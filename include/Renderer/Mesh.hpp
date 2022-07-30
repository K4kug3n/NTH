#ifndef NTH_RENDERER_MESH_HPP
#define NTH_RENDERER_MESH_HPP

#include "Renderer/Vertex.hpp"

#include "Renderer/VulkanBuffer.hpp"

#include <vector>
#include <string_view>

namespace Nth {

	struct Mesh {
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		VulkanBuffer vertexBuffer;
		VulkanBuffer indexBuffer;

		static Mesh fromOBJ(const std::string_view filename);
	};

}

#endif

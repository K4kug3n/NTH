#ifndef NTH_RENDERER_MESH_HPP
#define NTH_RENDERER_MESH_HPP

#include "Renderer/Vertex.hpp"

#include <vector>
#include <string>

namespace Nth {

	struct Mesh {
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		static Mesh fromOBJ(std::string const& filename);
	};

}

#endif

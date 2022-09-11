#ifndef NTH_RENDERER_MESH_HPP
#define NTH_RENDERER_MESH_HPP

#include <Renderer/Vertex.hpp>

#include <vector>
#include <string_view>

namespace Nth {
	struct Texture;

	struct Mesh {
		Mesh() = default;
		Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<size_t> texturesIndex);

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		std::vector<size_t> texturesIndex;

		static Mesh fromOBJ(const std::string_view filename);
	};

}

#endif

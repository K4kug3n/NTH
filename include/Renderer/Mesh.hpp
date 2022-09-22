#ifndef NTH_RENDERER_MESH_HPP
#define NTH_RENDERER_MESH_HPP

#include <Renderer/Vertex.hpp>

#include <vector>
#include <string_view>

namespace Nth {
	struct Texture;

	// TODO: Be a class
	struct Mesh {
		Mesh() = default;
		Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<size_t> texturesIndex);

		void addTextureIndex(size_t index);

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		std::vector<size_t> texturesIndex;

		static Mesh FromOBJ(const std::string_view filename);
		static Mesh Plane();
	};

}

#endif

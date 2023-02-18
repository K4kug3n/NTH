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
		Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<size_t> textures_index);

		void add_texture_index(size_t index);

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		std::vector<size_t> textures_index;

		static Mesh FromOBJ(std::string_view filename);
		static Mesh Plane();
	};

}

#endif

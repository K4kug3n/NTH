#include <Renderer/Mesh.hpp>

#include <Renderer/Texture.hpp>

#include <tiny_obj_loader.h>

namespace Nth {
	Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<size_t> texturesIndex) :
		vertices(std::move(vertices)),
		indices(std::move(indices)),
		texturesIndex(std::move(texturesIndex)) { }

	void Mesh::addTextureIndex(size_t index) {
		texturesIndex.push_back(index);
	}

	Mesh Mesh::FromOBJ(const std::string_view filename) {
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename.data())) {
			throw std::runtime_error(warn + err);
		}

		Mesh newMesh;

		for (const auto& shape : shapes) {
			for (const auto& index : shape.mesh.indices) {
				Vertex vertex{};

				vertex.pos = {
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
				};

				if (index.texcoord_index >= 0) {
					vertex.texturePos = {
						attrib.texcoords[2 * index.texcoord_index + 0],
						1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
					};
				}

				if (index.normal_index >= 0) {
					vertex.normal = {
						attrib.normals[3 * index.normal_index + 0],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2],
					};
				}
				
				newMesh.vertices.push_back(vertex);
				newMesh.indices.push_back(static_cast<unsigned int>(newMesh.indices.size()));
			}
		}

		return newMesh;
	}

	Mesh Mesh::Plane() {
		Mesh mesh;

		Vertex left_top;
		left_top.pos = { 0.f, 0.f, 0.f };
		left_top.texturePos = { 0.f, 0.f };
		left_top.normal = { 0.f, 0.f, 1.f };
		mesh.vertices.push_back(std::move(left_top));

		Vertex right_top;
		right_top.pos = { 1.f, 0.f, 0.f };
		right_top.texturePos = { 0.f, 0.f };
		right_top.normal = { 0.f, 0.f, 1.f };
		mesh.vertices.push_back(std::move(right_top));

		Vertex left_bot;
		left_bot.pos = { 0.f, 1.f, 0.f };
		left_bot.texturePos = { 0.f, 0.f };
		left_bot.normal = { 0.f, 0.f, 1.f };
		mesh.vertices.push_back(std::move(left_bot));

		Vertex right_bot;
		right_bot.pos = { 1.f, 1.f, 0.f };
		right_bot.texturePos = { 0.f, 0.f };
		right_bot.normal = { 0.f, 0.f, 1.f };
		mesh.vertices.push_back(std::move(right_bot));

		mesh.indices = { 0, 1, 3, 0, 2, 3 };

		return mesh;
	}
}
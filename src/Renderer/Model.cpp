#include <Renderer/Model.hpp>

#include <Renderer/Mesh.hpp>

#include <Utils/Image.hpp>

#include <Maths/AssimpConvertion.hpp>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <stdexcept>
#include <iostream>

namespace Nth {
	Model::Model(const std::filesystem::path& path) {
		load_from_file(path);
	}

	void Model::add_mesh(Mesh&& mesh) {
		meshes.push_back(std::move(mesh));
	}
	
	size_t Model::add_texture(Texture&& texture) {
		for (size_t i = 0; i < m_textures_loaded.size(); ++i) {
			if (m_textures_loaded[i].path == texture.path) {
				return i;
			}
		}
		
		m_textures_loaded.push_back(std::move(texture));
		return m_textures_loaded.size() - 1;
	}

	const std::vector<Texture>& Model::textures() const {
		return m_textures_loaded;
	}

	void Model::load_from_file(const std::filesystem::path& path) {
		Assimp::Importer import;
		const aiScene* scene = import.ReadFile(path.string().c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			throw std::runtime_error("ASSIMP::" + std::string{ import.GetErrorString() });
		}
		m_directory = path.parent_path();

		process_node(scene->mRootNode, scene, aiMatrix4x4{});
	}

	void Model::process_node(aiNode* node, const aiScene* scene, const aiMatrix4x4& parentTransformation) {
		aiMatrix4x4 current_transformation = node->mTransformation * parentTransformation;
		
		for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(process_mesh(mesh, scene, current_transformation));
		}

		// then do the same for each of its children
		for (unsigned int i = 0; i < node->mNumChildren; ++i) {
			process_node(node->mChildren[i], scene, current_transformation);
		}
	}

	Mesh Model::process_mesh(aiMesh* mesh, const aiScene* scene, const aiMatrix4x4& transformation) {
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		std::vector<size_t> textures;

		for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
			Vertex vertex;
			// process vertex positions, normals and texture coordinates
			vertex.pos = to_vector3(transformation * mesh->mVertices[i]);
			vertex.normal = to_vector3(transformation * mesh->mNormals[i]);

			if (mesh->mTextureCoords[0]) { // does the mesh contain texture coordinates?
				vertex.texture_pos = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
			}
			else {
				vertex.texture_pos = { 0.f, 0.f };
			}

			vertices.push_back(vertex);
		}
		// process indices
		for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; ++j) {
				indices.push_back(face.mIndices[j]);
			}
		}
		// process material
		if (mesh->mMaterialIndex >= 0) {
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			std::vector<size_t> diffuseMaps = load_material_textures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

			std::vector<size_t> specularMaps = load_material_textures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

			std::vector<size_t> baseColors = load_material_textures(material, aiTextureType_BASE_COLOR, "base_color");
			textures.insert(textures.end(), baseColors.begin(), baseColors.end());
		}

		return Mesh(vertices, indices, textures);
	}

	std::vector<size_t> Model::load_material_textures(aiMaterial* mat, aiTextureType type, std::string_view type_name) {
		std::vector<size_t> textures_index;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
			aiString str;
			mat->GetTexture(type, i, &str);

			bool skip = false;
			for (size_t j = 0; j < m_textures_loaded.size(); ++j) {
				if (m_textures_loaded[j].path == str.C_Str()) {
					skip = true;
					textures_index.push_back(i);
					break;
				}
			}
			
			if (!skip) {
				Texture texture = texture_from_file(m_directory / std::string{ str.C_Str() });
				texture.type = type_name;
				texture.path = str.C_Str();
				m_textures_loaded.push_back(std::move(texture));

				textures_index.push_back(m_textures_loaded.size() - 1);
			}
		}

		return textures_index;
	}
}
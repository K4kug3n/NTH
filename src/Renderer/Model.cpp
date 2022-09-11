#include <Renderer/Model.hpp>

#include <Renderer/Mesh.hpp>

#include <Util/Image.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <stdexcept>
#include <iostream>

namespace Nth {
	std::vector<Texture> const& Model::textures() const {
		return m_textures_loaded;
	}

	void Model::loadFromFile(std::string_view path) {

		Assimp::Importer import;
		const aiScene* scene = import.ReadFile(path.data(), aiProcess_Triangulate | aiProcess_FlipUVs);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			throw std::runtime_error("ERROR::ASSIMP::" + std::string{ import.GetErrorString() });
		}
		m_directory = path.substr(0, path.find_last_of('/'));

		processNode(scene->mRootNode, scene);
	}

	void Model::processNode(aiNode* node, const aiScene* scene) {
		for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}

		// then do the same for each of its children
		for (unsigned int i = 0; i < node->mNumChildren; ++i) {
			processNode(node->mChildren[i], scene);
		}
	}

	Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		std::vector<size_t> textures;

		for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
			Vertex vertex;
			// process vertex positions, normals and texture coordinates
			vertex.pos = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
			vertex.normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };

			if (mesh->mTextureCoords[0]) { // does the mesh contain texture coordinates?
				vertex.texturePos = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
			}
			else {
				vertex.texturePos = { 0.f, 0.f };
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

			std::vector<size_t> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

			std::vector<size_t> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

			std::vector<size_t> baseColors = loadMaterialTextures(material, aiTextureType_BASE_COLOR, "base_color");
			textures.insert(textures.end(), baseColors.begin(), baseColors.end());
		}

		return Mesh(vertices, indices, textures);
	}

	std::vector<size_t> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string_view typeName) {
		std::vector<size_t> texturesIndex;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
			aiString str;
			mat->GetTexture(type, i, &str);

			bool skip = false;
			for (size_t j = 0; j < m_textures_loaded.size(); ++j) {
				if (m_textures_loaded[j].path == str.C_Str()) {
					skip = true;
					texturesIndex.push_back(i);
					break;
				}
			}
			
			if (!skip) {
				Texture texture = textureFromFile(str.C_Str(), m_directory);
				texture.type = typeName;
				texture.path = str.C_Str();
				m_textures_loaded.push_back(texture);

				texturesIndex.push_back(m_textures_loaded.size() - 1);
			}
		}

		return texturesIndex;
	}

	Texture Model::textureFromFile(std::string_view filename, std::string_view directory) const {
		Image image = Image::loadFromFile(std::string{ directory } + "/" + std::string{ filename }, PixelChannel::Rgba);

		if (image.pixels().empty()) {
			throw std::runtime_error("Can't read file " + std::string{ directory } + "/" + std::string{ filename });
		}

		Texture texture;
		texture.data = image.pixels();
		texture.height = image.height();
		texture.width = image.width();

		return texture;
	}


}
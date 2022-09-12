#ifndef NTH_RENDERER_MODEL_HPP
#define NTH_RENDERER_MODEL_HPP

#include <Renderer/Texture.hpp>

#include <assimp/scene.h>

#include <vector>
#include <string_view>

namespace Nth {
	struct Mesh;

	class Model {
	public:
		Model() = default;

		// TODO: Cleanup
		std::vector<Texture> m_textures_loaded;
		std::vector<Mesh> meshes;
		Texture textureFromFile(std::string_view filename, std::string_view directory) const;

		std::vector<Texture> const& textures() const;

		void loadFromFile(std::string_view path);
	private:
		void processNode(aiNode* node, const aiScene* scene, aiMatrix4x4 const& parentTransformation);
		Mesh processMesh(aiMesh* mesh, const aiScene* scene, aiMatrix4x4 const& transformation);
		std::vector<size_t> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string_view typeName);

		std::string_view m_directory;
	};

}

#endif

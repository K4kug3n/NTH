#ifndef NTH_RENDERER_MODEL_HPP
#define NTH_RENDERER_MODEL_HPP

#include <Renderer/Texture.hpp>

#include <assimp/scene.h>

#include <vector>
#include <filesystem>

namespace Nth {
	struct Mesh;

	class Model {
	public:
		Model() = default;
		Model(std::filesystem::path const& path);

		// TODO: Cleanup
		std::vector<Texture> m_textures_loaded;
		std::vector<Mesh> meshes;

		void addMesh(Mesh&& mesh);
		size_t addTexture(Texture&& texture);

		std::vector<Texture> const& textures() const;

		void loadFromFile(std::filesystem::path const& path);
	private:
		void processNode(aiNode* node, const aiScene* scene, aiMatrix4x4 const& parentTransformation);
		Mesh processMesh(aiMesh* mesh, const aiScene* scene, aiMatrix4x4 const& transformation);
		std::vector<size_t> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string_view typeName);

		std::filesystem::path m_directory;
	};

}

#endif

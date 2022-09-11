#ifndef NTH_RENDERER_MODEL_HPP
#define NTH_RENDERER_MODEL_HPP

#include <Renderer/Texture.hpp>

#include <vector>
#include <string_view>

struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;
enum aiTextureType : int;

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
		void processNode(aiNode* node, const aiScene* scene);
		Mesh processMesh(aiMesh* mesh, const aiScene* scene);
		std::vector<size_t> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string_view typeName);

		std::string_view m_directory;
	};

}

#endif
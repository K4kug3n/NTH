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
		Model(const std::filesystem::path& path);

		// TODO: Cleanup
		std::vector<Texture> m_textures_loaded;
		std::vector<Mesh> meshes;

		void add_mesh(Mesh&& mesh);
		size_t add_texture(Texture&& texture);

		const std::vector<Texture>& textures() const;

		void load_from_file(const std::filesystem::path& path);
	private:
		void process_node(aiNode* node, const aiScene* scene, const aiMatrix4x4& parent_transformation);
		Mesh process_mesh(aiMesh* mesh, const aiScene* scene, const aiMatrix4x4& transformation);
		std::vector<size_t> load_material_textures(aiMaterial* mat, aiTextureType type, std::string_view type_name);

		std::filesystem::path m_directory;
	};

}

#endif

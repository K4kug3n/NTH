#include <Renderer/RenderModel.hpp>

namespace Nth {
	RenderModel::RenderModel(std::vector<RenderMesh>&& meshes, std::vector<RenderTexture>&& textures) :
		meshes(std::move(meshes)),
		textures(std::move(textures)) { }
}

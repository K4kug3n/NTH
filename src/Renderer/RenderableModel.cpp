#include <Renderer/RenderableModel.hpp>

namespace Nth {
	RenderableModel::RenderableModel(std::vector<RenderableMesh>&& meshes, std::vector<VulkanTexture>&& textures) :
		meshes(std::move(meshes)),
		textures(std::move(textures)) { }
}

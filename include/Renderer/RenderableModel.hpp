#ifndef NTH_RENDERER_RENDERABLEMODEL_HPP
#define NTH_RENDERER_RENDERABLEMODEL_HPP

#include <Renderer/VulkanTexture.hpp>
#include <Renderer/VulkanBuffer.hpp>

#include <vector>

namespace Nth {
	struct RenderableMesh {
		VulkanBuffer vertexBuffer;
		VulkanBuffer indexBuffer;

		std::vector<uint32_t> indices;
		size_t textureIndex;
	};

	struct RenderableModel {
		RenderableModel() = default;
		RenderableModel(std::vector<RenderableMesh>&& meshes, std::vector<VulkanTexture>&& textures);

		std::vector<RenderableMesh> meshes;
		std::vector<VulkanTexture> textures;
	};
}

#endif

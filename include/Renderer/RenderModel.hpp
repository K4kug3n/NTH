#ifndef NTH_RENDERER_RENDERMODEL_HPP
#define NTH_RENDERER_RENDERMODEL_HPP

#include <Renderer/RenderTexture.hpp>
#include <Renderer/RenderBuffer.hpp>

#include <vector>

namespace Nth {
	struct RenderMesh {
		RenderBuffer vertexBuffer;
		RenderBuffer indexBuffer;

		std::vector<uint32_t> indices;
		size_t textureIndex;
	};

	struct RenderModel {
		RenderModel() = default;
		RenderModel(std::vector<RenderMesh>&& meshes, std::vector<RenderTexture>&& textures);

		std::vector<RenderMesh> meshes;
		std::vector<RenderTexture> textures;
	};
}

#endif

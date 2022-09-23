#ifndef NTH_RENDERER_SCENEPARAMETERS_HPP
#define NTH_RENDERER_SCENEPARAMETERS_HPP

#include <Math/Matrix4.hpp>
#include <Math/Vector3.hpp>
#include <Math/Vector4.hpp>

namespace Nth {
	struct LightGpuObject {
		alignas(16) Vector3f viewPos;

		alignas(16) Vector4f lightColor;
		alignas(16) Vector3f position;
		alignas(4) float ambientStrength;
		alignas(4) float specularStrength;
	};

	// TODO: Precalculate VP CPU-side
	struct ViewerGpuObject {
		Matrix4f view;
		Matrix4f proj;
	};

	struct ModelGpuObject {
		Matrix4f model;
	};
}

#endif
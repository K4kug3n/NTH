#ifndef NTH_RENDERER_SCENEPARAMETERS_HPP
#define NTH_RENDERER_SCENEPARAMETERS_HPP

#include <Maths/Matrix4.hpp>
#include <Maths/Vector3.hpp>
#include <Maths/Vector4.hpp>

namespace Nth {
	struct LightGpuObject {
		alignas(16) Vector3f view_pos;

		alignas(16) Vector4f light_color;
		alignas(16) Vector3f position;
		alignas(4) float ambient_strength;
		alignas(4) float specular_strength;
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
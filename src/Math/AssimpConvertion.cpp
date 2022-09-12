#include <Math/AssimpConvertion.hpp>

#include <Math/Matrix4.hpp>
#include <Math/Vector3.hpp>

namespace Nth {
	Matrix4f toMatrix4(aiMatrix4x4 const& mat) {
		return Matrix4f{
			mat.a1, mat.b1, mat.c1, mat.d1,
			mat.a2, mat.b2, mat.c2, mat.d2,
			mat.a3, mat.b3, mat.c3, mat.d3,
			mat.a4, mat.b4, mat.c3, mat.d4,
		};
	}

	Vector3f toVector3(aiVector3D const& vec) {
		return Vector3f{ vec.x, vec.y, vec.z };
	}
}
#include <Maths/AssimpConvertion.hpp>

#include <Maths/Matrix4.hpp>
#include <Maths/Vector3.hpp>

namespace Nth {
	Matrix4f to_matrix4(const aiMatrix4x4& mat) {
		return Matrix4f{
			mat.a1, mat.b1, mat.c1, mat.d1,
			mat.a2, mat.b2, mat.c2, mat.d2,
			mat.a3, mat.b3, mat.c3, mat.d3,
			mat.a4, mat.b4, mat.c3, mat.d4,
		};
	}

	Vector3f to_vector3(const aiVector3D& vec) {
		return Vector3f{ vec.x, vec.y, vec.z };
	}
}
#include <Math/AssimpConvertion.hpp>

#include <Math/Matrix4.hpp>
#include <Math/Vector3.hpp>

namespace Nth {
	Matrix4f toMatrix4(aiMatrix4x4 const& mat) {
		return Matrix4f{
			mat[0][0], mat[1][0], mat[2][0], mat[3][0],
			mat[0][1], mat[1][1], mat[2][1], mat[3][1],
			mat[0][2], mat[1][2], mat[2][2], mat[3][2],
			mat[0][3], mat[1][3], mat[2][3], mat[3][3],
		};
	}

	Vector3f toVector3(aiVector3D const& vec) {
		return Vector3f{ vec.x, vec.y, vec.z };
	}
}
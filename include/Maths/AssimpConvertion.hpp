#ifndef NTH_MATHS_ASSIMPCONVERTION_HPP
#define NTH_MATHS_ASSIMPCONVERTION_HPP

#include <assimp/matrix4x4.h>
#include <assimp/vector3.h>

namespace Nth {
	template<typename T> class Matrix4;
	using Matrix4f = Matrix4<float>;

	template<typename T> class Vector3;
	using Vector3f = Vector3<float>;

	Matrix4f to_matrix4(aiMatrix4x4 const& mat);

	Vector3f to_vector3(aiVector3D const& vec);
}

#endif
#include <Renderer/Camera.hpp>

#include <Maths/Matrix4.hpp>

namespace Nth {
	Camera::Camera() :
		position(0.f, 0.f, 0.f),
		type(CameraType::FirstPerson) { }

	Matrix4f Camera::get_view_matrix() const {
		if (type == CameraType::FirstPerson) {
			return Matrix4f::Translation(position);
		}

		return Matrix4f::Translation(position);
	}
}

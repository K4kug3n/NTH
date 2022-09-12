#include <Renderer/Camera.hpp>

#include <Math/Matrix4.hpp>

namespace Nth {
	Camera::Camera() :
		position(0.f, 0.f, 0.f),
		direction(0.f, 0.f, 0.f),
		type(CameraType::FirstPerson) { }

	Matrix4f Camera::getViewMatrix() const {
		if (type == CameraType::FirstPerson) {
			return direction.toRotationMatrix() * Matrix4f::Translation(position);
		}

		return Matrix4f::Translation(position) * direction.toRotationMatrix();
	}
}

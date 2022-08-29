#include <Renderer/Camera.hpp>

#include <Math/Matrix4.hpp>

namespace Nth {
	Matrix4f Camera::getViewMatrix() const {
		return Matrix4f::Translation(position) * direction.toRotationMatrix();
	}
}

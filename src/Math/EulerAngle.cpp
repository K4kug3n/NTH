#include <Math/EulerAngle.hpp>

#include <Math/Matrix4.hpp>
#include <Math/Angle.hpp>

#include <cmath>

namespace Nth {
	EulerAngle::EulerAngle(float yaw, float pitch, float roll) :
		yaw(yaw),
		pitch(pitch),
		roll(roll) { }

	Matrix4f EulerAngle::toRotationMatrix() const {
		float cosY = std::cos(toRadians(yaw));
		float sinY = std::sin(toRadians(yaw));

		float cosP = std::cos(toRadians(pitch));
		float sinP = std::sin(toRadians(pitch));

		float cosR = std::cos(toRadians(roll));
		float sinR = std::sin(toRadians(roll));

		Matrix4f mat = Matrix4f::Identity();
		mat.a11 = cosY * cosR + sinY * sinP * sinR;
		mat.a21 = cosR * sinY * sinP - sinR * cosY;
		mat.a31 = cosP * sinY;

		mat.a12 = cosP * sinR;
		mat.a22 = cosR * cosP;
		mat.a32 = -sinP;

		mat.a13 = sinR * cosY * sinP - sinY * cosR;
		mat.a23 = sinY * sinR + cosR * cosY * sinP;
		mat.a33 = cosP * cosY;

		return mat;
	}
}
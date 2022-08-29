#ifndef NTH_MATH_EULERANGLE_HPP
#define NTH_MATH_EULERANGLE_HPP

namespace Nth {
	template<typename T> class Matrix4;
	using Matrix4f = Matrix4<float>;

	class EulerAngle {
	public:
		EulerAngle() = default;
		EulerAngle(float yaw, float pitch, float roll);
		EulerAngle(EulerAngle const&) = default;
		EulerAngle(EulerAngle&&) = default;
		~EulerAngle() = default;

		float yaw;
		float pitch;
		float roll;

		Matrix4f toRotationMatrix() const;

		EulerAngle& operator=(EulerAngle const&) = default;
		EulerAngle& operator=(EulerAngle&&) = default;
	};
}


#endif
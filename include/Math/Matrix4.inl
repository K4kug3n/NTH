#include "Math/Matrix4.hpp"

#include "Math/Vector3.hpp"
#include "Math/Vector4.hpp"

#include <sstream>

namespace Nth{
	template<typename T>
	Matrix4<T>::Matrix4(T a11, T a12, T a13, T a14,
						T a21, T a22, T a23, T a24,
						T a31, T a32, T a33, T a34,
						T a41, T a42, T a43, T a44) :
		a11(a11), a12(a12), a13(a13), a14(a14),
		a21(a21), a22(a22), a23(a23), a24(a24),
		a31(a31), a32(a32), a33(a33), a34(a34),
		a41(a41), a42(a42), a43(a43), a44(a44) {}

	template<typename T>
	Matrix4<T> Matrix4<T>::operator+(Matrix4 const& mat) const {
		return Matrix4<T>{ a11 + mat.a11, a12 + mat.a12, a13 + mat.a13, a14 + mat.a14,
						a21 + mat.a21, a22 + mat.a22, a23 + mat.a23, a24 + mat.a24, 
						a31 + mat.a31, a32 + mat.a32, a33 + mat.a33, a34 + mat.a34, 
						a41 + mat.a41, a42 + mat.a42, a43 + mat.a43, a44 + mat.a44};
	}

	template<typename T>
	Matrix4<T> Matrix4<T>::operator+(T scale) const {
		return Matrix4<T>{ a11 + scale, a12 + scale, a13 + scale, a14 + scale,
			a21 + scale, a22 + scale, a23 + scale, a24 + scale,
			a31 + scale, a32 + scale, a33 + scale, a34 + scale,
			a41 + scale, a42 + scale, a43 + scale, a44 + scale};
	}

	template<typename T>
	Matrix4<T> Matrix4<T>::operator-(Matrix4 const& mat) const {
		return Matrix4<T>{ a11 - mat.a11, a12 - mat.a12, a13 - mat.a13, a14 - mat.a14,
			a21 - mat.a21, a22 - mat.a22, a23 - mat.a23, a24 - mat.a24,
			a31 - mat.a31, a32 - mat.a32, a33 - mat.a33, a34 - mat.a34,
			a41 - mat.a41, a42 - mat.a42, a43 - mat.a43, a44 - mat.a44};
	}

	template<typename T>
	Matrix4<T> Matrix4<T>::operator-(T scale) const {
		return Matrix4<T>{ a11 - scale, a12 - scale, a13 - scale, a14 - scale,
			a21 - scale, a22 - scale, a23 - scale, a24 - scale,
			a31 - scale, a32 - scale, a33 - scale, a34 - scale,
			a41 - scale, a42 - scale, a43 - scale, a44 - scale};
	}

	template<typename T>
	Matrix4<T> Matrix4<T>::operator*(Matrix4 const& mat) const {
		return Matrix4<T>{ 
			a11 * mat.a11 + a12 * mat.a21 + a13 * mat.a31 + a14 * mat.a41,
			a11 * mat.a12 + a12 * mat.a22 + a13 * mat.a32 + a14 * mat.a42,
			a11 * mat.a13 + a12 * mat.a23 + a13 * mat.a33 + a14 * mat.a43,
			a11 * mat.a14 + a12 * mat.a24 + a13 * mat.a34 + a14 * mat.a44,

			a21* mat.a11 + a22 * mat.a21 + a23 * mat.a31 + a24 * mat.a41,
			a21* mat.a12 + a22 * mat.a22 + a23 * mat.a32 + a24 * mat.a42,
			a21* mat.a13 + a22 * mat.a23 + a23 * mat.a33 + a24 * mat.a43,
			a21* mat.a14 + a22 * mat.a24 + a23 * mat.a34 + a24 * mat.a44,

			a31* mat.a11 + a32 * mat.a21 + a33 * mat.a31 + a34 * mat.a41,
			a31* mat.a12 + a32 * mat.a22 + a33 * mat.a32 + a34 * mat.a42,
			a31* mat.a13 + a32 * mat.a23 + a33 * mat.a33 + a34 * mat.a43,
			a31* mat.a14 + a32 * mat.a24 + a33 * mat.a34 + a34 * mat.a44,

			a41* mat.a11 + a42 * mat.a21 + a43 * mat.a31 + a44 * mat.a41,
			a41* mat.a12 + a42 * mat.a22 + a43 * mat.a32 + a44 * mat.a42,
			a41* mat.a13 + a42 * mat.a23 + a43 * mat.a33 + a44 * mat.a43,
			a41* mat.a14 + a42 * mat.a24 + a43 * mat.a34 + a44 * mat.a44,
		};
	}

	template<typename T>
	Matrix4<T> Matrix4<T>::operator*(T scale) const {
		return Matrix4<T>{ a11 * scale, a12 * scale, a13 * scale, a14 * scale,
			a21 * scale, a22 * scale, a23 * scale, a24 * scale,
			a31 * scale, a32 * scale, a33 * scale, a34 * scale,
			a41 * scale, a42 * scale, a43 * scale, a44 * scale};
	}

	template<typename T>
	Matrix4<T>& Matrix4<T>::operator+=(Matrix4 const& mat) {
		a11 += mat.a11;
		a12 += mat.a12;
		a13 += mat.a13;
		a14 += mat.a14;

		a21 += mat.a21;
		a22 += mat.a22;
		a23 += mat.a23;
		a24 += mat.a24;

		a31 += mat.a31;
		a32 += mat.a32;
		a33 += mat.a33;
		a34 += mat.a34;

		a41 += mat.a41;
		a42 += mat.a42;
		a43 += mat.a43;
		a44 += mat.a44;

		return *this;
	}

	template<typename T>
	Matrix4<T>& Matrix4<T>::operator+=(T scale) {
		a11 += scale;
		a12 += scale;
		a13 += scale;
		a14 += scale;

		a21 += scale;
		a22 += scale;
		a23 += scale;
		a24 += scale;

		a31 += scale;
		a32 += scale;
		a33 += scale;
		a34 += scale;

		a41 += scale;
		a42 += scale;
		a43 += scale;
		a44 += scale;

		return *this;
	}

	template<typename T>
	Matrix4<T>& Matrix4<T>::operator-=(Matrix4 const& mat) {
		a11 -= mat.a11;
		a12 -= mat.a12;
		a13 -= mat.a13;
		a14 -= mat.a14;

		a21 -= mat.a21;
		a22 -= mat.a22;
		a23 -= mat.a23;
		a24 -= mat.a24;

		a31 -= mat.a31;
		a32 -= mat.a32;
		a33 -= mat.a33;
		a34 -= mat.a34;

		a41 -= mat.a41;
		a42 -= mat.a42;
		a43 -= mat.a43;
		a44 -= mat.a44;

		return *this;
	}

	template<typename T>
	Matrix4<T>& Matrix4<T>::operator-=(T scale) {
		a11 -= scale;
		a12 -= scale;
		a13 -= scale;
		a14 -= scale;

		a21 -= scale;
		a22 -= scale;
		a23 -= scale;
		a24 -= scale;

		a31 -= scale;
		a32 -= scale;
		a33 -= scale;
		a34 -= scale;

		a41 -= scale;
		a42 -= scale;
		a43 -= scale;
		a44 -= scale;

		return *this;
	}

	template<typename T>
	Matrix4<T>& Matrix4<T>::operator*=(Matrix4 const& mat) {
		Matrix4 copy{ *this };

		a11 = copy.a11 * mat.a11 + copy.a12 * mat.a21 + copy.a13 * mat.a31 + copy.a14 * mat.a41;
		a12 = copy.a11 * mat.a12 + copy.a12 * mat.a22 + copy.a13 * mat.a32 + copy.a14 * mat.a42;
		a13 = copy.a11 * mat.a13 + copy.a12 * mat.a23 + copy.a13 * mat.a33 + copy.a14 * mat.a43;
		a14 = copy.a11 * mat.a14 + copy.a12 * mat.a24 + copy.a13 * mat.a34 + copy.a14 * mat.a44;

		a21 = copy.a21 * mat.a11 + copy.a22 * mat.a21 + copy.a23 * mat.a31 + copy.a24 * mat.a41;
		a22 = copy.a21 * mat.a12 + copy.a22 * mat.a22 + copy.a23 * mat.a32 + copy.a24 * mat.a42;
		a23 = copy.a21 * mat.a13 + copy.a22 * mat.a23 + copy.a23 * mat.a33 + copy.a24 * mat.a43;
		a24 = copy.a21 * mat.a14 + copy.a22 * mat.a24 + copy.a23 * mat.a34 + copy.a24 * mat.a44;

		a31 = copy.a31 * mat.a11 + copy.a32 * mat.a21 + copy.a33 * mat.a31 + copy.a34 * mat.a41;
		a32 = copy.a31 * mat.a12 + copy.a32 * mat.a22 + copy.a33 * mat.a32 + copy.a34 * mat.a42;
		a33 = copy.a31 * mat.a13 + copy.a32 * mat.a23 + copy.a33 * mat.a33 + copy.a34 * mat.a43;
		a34 = copy.a31 * mat.a14 + copy.a32 * mat.a24 + copy.a33 * mat.a34 + copy.a34 * mat.a44;

		a41 = copy.a41 * mat.a11 + copy.a42 * mat.a21 + copy.a43 * mat.a31 + copy.a44 * mat.a41;
		a42 = copy.a41 * mat.a12 + copy.a42 * mat.a22 + copy.a43 * mat.a32 + copy.a44 * mat.a42;
		a43 = copy.a41 * mat.a13 + copy.a42 * mat.a23 + copy.a43 * mat.a33 + copy.a44 * mat.a43;
		a44 = copy.a41 * mat.a14 + copy.a42 * mat.a24 + copy.a43 * mat.a34 + copy.a44 * mat.a44;

		return *this;
	}

	template<typename T>
	Matrix4<T>& Matrix4<T>::operator*=(T scale) {
		a11 *= scale;
		a12 *= scale;
		a13 *= scale;
		a14 *= scale;

		a21 *= scale;
		a22 *= scale;
		a23 *= scale;
		a24 *= scale;

		a31 *= scale;
		a32 *= scale;
		a33 *= scale;
		a34 *= scale;

		a41 *= scale;
		a42 *= scale;
		a43 *= scale;
		a44 *= scale;

		return *this;
	}

	template<typename T>
	bool Matrix4<T>::operator==(const Matrix4& mat) const {
		return a11 == mat.a11 && a12 == mat.a12 && a13 == mat.a13 && a14 == mat.a14
			&& a21 == mat.a21 && a22 == mat.a22 && a23 == mat.a23 && a24 == mat.a24
			&& a31 == mat.a31 && a32 == mat.a32 && a33 == mat.a33 && a34 == mat.a34
			&& a41 == mat.a41 && a42 == mat.a42 && a43 == mat.a43 && a44 == mat.a44; // Epsilon for float
	}

	template<typename T>
	bool Matrix4<T>::operator!=(Matrix4 const& mat) const {
		return !(*this == mat);
	}

	template<typename T>
	inline std::array<float, 16> Matrix4<T>::toArray() const {
		return std::array<float, 16>{
			a11, a12, a13, a14,
			a21, a22, a23, a24,
			a31, a31, a33, a34,
			a41, a42, a43, a44
		};
	}

	template<typename T>
	std::string Matrix4<T>::toString() const {
		std::stringstream stream;

		stream << "Matrix4(" << a11 << "," << a12 << "," << a13 << "," << a14 << "\n"
			<< "|" << a21 << "," << a22 << "," << a23 << "," << a24 << "\n"
			<< "|" << a31 << "," << a32 << "," << a33 << "," << a34 << "\n"
			<< "|" << a41 << "," << a42 << "," << a43 << "," << a44 << ")";

		return stream.str();
	}

	template<typename T>
	inline Matrix4<T> Matrix4<T>::Identity() {
		return Matrix4<T>{
			static_cast<T>(1.0), static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(0.0),
			static_cast<T>(0.0), static_cast<T>(1.0), static_cast<T>(0.0), static_cast<T>(0.0),
			static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(1.0), static_cast<T>(0.0),
			static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(1.0)
		};
	}

	template<typename T>
	inline Matrix4<T> Matrix4<T>::Translation(Vector3<T> const& translation) {
		return Matrix4<T>{
			static_cast<T>(1.0), static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(0.0),
			static_cast<T>(0.0), static_cast<T>(1.0), static_cast<T>(0.0), static_cast<T>(0.0),
			static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(1.0), static_cast<T>(0.0),
			translation.x, translation.y, translation.z, static_cast<T>(1.0)
		};
	}

	template<typename T>
	Matrix4<T> Matrix4<T>::Perspective(float fov, float aspectRatio, float nearClipping, float farClipping) {
		fov /= static_cast<T>(2.0);

		T const scale{ std::tan(fov) };

		return Matrix4<T>{
			static_cast<T>(1.0) / (aspectRatio * scale), static_cast<T>(0.0),         static_cast<T>(0.0),                                                              static_cast<T>(0.0),
			static_cast<T>(0.0),                         static_cast<T>(1.0) / scale, static_cast<T>(0.0),                                                              static_cast<T>(0.0),
			static_cast<T>(0.0),                         static_cast<T>(0.0),         -(farClipping + nearClipping) / (farClipping - nearClipping),                     -static_cast<T>(1.0),
			static_cast<T>(0.0),                         static_cast<T>(0.0),         -(static_cast<T>(2) * farClipping * nearClipping) / (farClipping - nearClipping),  static_cast<T>(0.0),
		};
	}

	template<typename T>
	inline Matrix4<T> Matrix4<T>::Orthographic(float leftPlane, float rightPlane, float topPlane, float bottomPlane, float nearPlane, float farPlane) {
		return Matrix4(
			2.0f / (rightPlane - leftPlane),
			0.0f,
			0.0f,
			0.0f,

			0.0f,
			2.0f / (bottomPlane - topPlane),
			0.0f,
			0.0f,

			0.0f,
			0.0f,
			1.0f / (nearPlane - farPlane),
			0.0f,

			-(rightPlane + leftPlane) / (rightPlane - leftPlane),
			-(bottomPlane + topPlane) / (bottomPlane - topPlane),
			nearPlane / (nearPlane - farPlane),
			1.0f
		);
	}

	template<typename T>
	Matrix4<T> Matrix4<T>::Rotation(float angle, Vector3<T> axis) {
		axis.normalise();

		float a = angle;
		float c = std::cos(angle);
		float s = std::sin(angle);

		Vector3<T> temp{ axis * (T(1) - c) };

		Matrix4<T> rotate;
		rotate.a11 = c + temp.x * axis.x;
		rotate.a12 = 0 + temp.x * axis.y + s * axis.z;
		rotate.a13 = 0 + temp.x * axis.z - s * axis.y;

		rotate.a21 = 0 + temp.y * axis.x - s * axis.z;
		rotate.a22 = c + temp.y * axis.y;
		rotate.a23 = 0 + temp.y * axis.z + s * axis.x;

		rotate.a31 = 0 + temp.z * axis.x + s * axis.y;
		rotate.a32 = 0 + temp.z * axis.y - s * axis.x;
		rotate.a33 = c + temp.z * axis.z;

		Matrix4<T> result;
		Vector4f result0{ Vector4f{ 1.f, 0.f, 0.f, 0.f } * rotate.a11 + Vector4f{ 0.f, 1.f, 0.f, 0.f } * rotate.a12 + Vector4f{ 0.f, 0.f, 1.f, 0.f } * rotate.a13 };
		result.a11 = result0.x;
		result.a12 = result0.y;
		result.a13 = result0.z;
		result.a14 = result0.w;

		Vector4f result1{ Vector4f{ 1.f, 0.f, 0.f, 0.f } * rotate.a21 + Vector4f{ 0.f, 1.f, 0.f, 0.f } * rotate.a22 + Vector4f{ 0.f, 0.f, 1.f, 0.f } * rotate.a23 };
		result.a21 = result1.x;
		result.a22 = result1.y;
		result.a23 = result1.z;
		result.a24 = result1.w;

		Vector4f result2{ Vector4f{ 1.f, 0.f, 0.f, 0.f } * rotate.a31 + Vector4f{ 0.f, 1.f, 0.f, 0.f } * rotate.a32 + Vector4f{ 0.f, 0.f, 1.f, 0.f } * rotate.a33 };
		result.a31 = result2.x;
		result.a32 = result2.y;
		result.a33 = result2.z;
		result.a34 = result2.w;

		result.a41 = static_cast<T>(0.f);
		result.a42 = static_cast<T>(0.f);
		result.a43 = static_cast<T>(0.f);
		result.a44 = static_cast<T>(1.f);

		return result;
	}

	template<typename T>
	Matrix4<T> Matrix4<T>::Scale(Vector3<T> const& scale) {
		return Matrix4{
			scale.x,             static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(0.0),
			static_cast<T>(0.0), scale.y,             static_cast<T>(0.0), static_cast<T>(0.0),
			static_cast<T>(0.0), static_cast<T>(0.0), scale.z,             static_cast<T>(0.0),
			static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(1.0)
		};
	}

	template<typename T>
	Matrix4<T> operator*(T scalar, const Matrix4<T>& mat) {
		return mat * scalar;
	}
}

template<typename T>
std::ostream& operator<<(std::ostream& out, Nth::Matrix4<T> const& mat) {
	return out << mat.toString();
}
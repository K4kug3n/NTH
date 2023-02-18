#include <Maths/Matrix4.hpp>

#include <Maths/Vector3.hpp>
#include <Maths/Vector4.hpp>

#include <sstream>
#include <cassert>

namespace Nth{
	// TODO: Dedicated class
	template<typename T>
	inline float det_mat3(T a11, T a12, T a13, T a21, T a22, T a23, T a31, T a32, T a33) {
		return a11 * a22 * a33 + a13 * a21 * a32 + a12 * a23 * a31
			- (a13 * a22 * a31) - (a12 * a21 * a33) - (a11 * a23 * a32);
	}

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
	inline float Matrix4<T>::det() const {
		return a11 * (a22 * a33 * a44 - a22 * a34 * a43 - a23 * a32 * a44 + a23 * a34 * a42 + a24 * a32 * a43 - a24 * a33 * a42)
			- a12 * (a21 * a33 * a44 - a21 * a34 * a43 - a23 * a31 * a44 + a23 * a34 * a41 + a24 * a31 * a43 - a24 * a33 * a41)
			+ a13 * (a21 * a32 * a44 - a21 * a34 * a42 - a22 * a31 * a44 + a22 * a34 * a41 + a24 * a31 * a42 - a24 * a32 * a41)
			- a14 * (a21 * a32 * a43 - a21 * a33 * a42 - a22 * a31 * a43 + a22 * a33 * a41 + a23 * a31 * a42 - a23 * a32 * a41);

	}

	template<typename T>
	inline Matrix4<float> Matrix4<T>::adj() const {
		float m11 = det_mat3(a22, a23, a24, a32, a33, a34, a42, a43, a44);
		float m12 = det_mat3(a21, a23, a24, a31, a33, a34, a41, a43, a44);
		float m13 = det_mat3(a21, a22, a24, a31, a32, a34, a41, a42, a44);
		float m14 = det_mat3(a21, a22, a23, a31, a32, a33, a41, a42, a43);

		float m21 = det_mat3(a12, a13, a14, a32, a33, a34, a42, a43, a44);
		float m22 = det_mat3(a11, a13, a14, a31, a33, a34, a41, a43, a44);
		float m23 = det_mat3(a11, a12, a14, a31, a32, a34, a41, a42, a44);
		float m24 = det_mat3(a11, a12, a13, a31, a32, a33, a41, a42, a43);

		float m31 = det_mat3(a12, a13, a14, a22, a23, a24, a42, a43, a44);
		float m32 = det_mat3(a11, a13, a14, a21, a23, a24, a41, a43, a44);
		float m33 = det_mat3(a11, a12, a14, a21, a22, a24, a41, a42, a44);
		float m34 = det_mat3(a11, a12, a13, a21, a22, a23, a41, a42, a43);

		float m41 = det_mat3(a12, a13, a14, a22, a23, a24, a32, a33, a34);
		float m42 = det_mat3(a11, a13, a14, a21, a23, a24, a31, a33, a34);
		float m43 = det_mat3(a11, a12, a14, a21, a22, a24, a31, a32, a34);
		float m44 = det_mat3(a11, a12, a13, a21, a22, a23, a31, a32, a33);

		return Matrix4f{
			m11, -m21, m31, -m41,
			-m12, m22, -m32, m42,
			m13, -m23, m33, -m43,
			-m14, m24, -m34, m44,
		};
	}

	template<typename T>
	inline Matrix4<float> Matrix4<T>::inv() const {
		assert(det() != 0);
		return (1 / det()) * adj();
	}

	template<typename T>
	Matrix4<T> Matrix4<T>::operator+(const Matrix4& mat) const {
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
	Matrix4<T> Matrix4<T>::operator-(const Matrix4& mat) const {
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
	Matrix4<T> Matrix4<T>::operator*(const Matrix4& mat) const {
		return Matrix4<T>{ 
			mat.a11 * a11 + mat.a12 * a21 + mat.a13 * a31 + mat.a14 * a41,
			mat.a11 * a12 + mat.a12 * a22 + mat.a13 * a32 + mat.a14 * a42,
			mat.a11 * a13 + mat.a12 * a23 + mat.a13 * a33 + mat.a14 * a43,
			mat.a11 * a14 + mat.a12 * a24 + mat.a13 * a34 + mat.a14 * a44,

			mat.a21 * a11 + mat.a22 * a21 + mat.a23 * a31 + mat.a24 * a41,
			mat.a21 * a12 + mat.a22 * a22 + mat.a23 * a32 + mat.a24 * a42,
			mat.a21 * a13 + mat.a22 * a23 + mat.a23 * a33 + mat.a24 * a43,
			mat.a21 * a14 + mat.a22 * a24 + mat.a23 * a34 + mat.a24 * a44,

			mat.a31 * a11 + mat.a32 * a21 + mat.a33 * a31 + mat.a34 * a41,
			mat.a31 * a12 + mat.a32 * a22 + mat.a33 * a32 + mat.a34 * a42,
			mat.a31 * a13 + mat.a32 * a23 + mat.a33 * a33 + mat.a34 * a43,
			mat.a31 * a14 + mat.a32 * a24 + mat.a33 * a34 + mat.a34 * a44,

			mat.a41 * a11 + mat.a42 * a21 + mat.a43 * a31 + mat.a44 * a41,
			mat.a41 * a12 + mat.a42 * a22 + mat.a43 * a32 + mat.a44 * a42,
			mat.a41 * a13 + mat.a42 * a23 + mat.a43 * a33 + mat.a44 * a43,
			mat.a41 * a14 + mat.a42 * a24 + mat.a43 * a34 + mat.a44 * a44,
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
	Matrix4<T>& Matrix4<T>::operator+=(const Matrix4& mat) {
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
	Matrix4<T>& Matrix4<T>::operator-=(const Matrix4& mat) {
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
	Matrix4<T>& Matrix4<T>::operator*=(const Matrix4& mat) {
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
	bool Matrix4<T>::operator!=(const Matrix4& mat) const {
		return !(*this == mat);
	}

	template<typename T>
	inline std::array<float, 16> Matrix4<T>::to_array() const {
		return std::array<float, 16>{
			a11, a12, a13, a14,
			a21, a22, a23, a24,
			a31, a31, a33, a34,
			a41, a42, a43, a44
		};
	}

	template<typename T>
	std::string Matrix4<T>::to_string() const {
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
	inline Matrix4<T> Matrix4<T>::Translation(const Vector3<T>& translation) {
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
			static_cast<T>(1.0) / (aspectRatio * scale), static_cast<T>(0.0),          static_cast<T>(0.0),                                          static_cast<T>(0.0),
			static_cast<T>(0.0),                         static_cast<T>(1.0) / scale,  static_cast<T>(0.0),                                          static_cast<T>(0.0),
			static_cast<T>(0.0),                         static_cast<T>(0.0),         -(farClipping) / (farClipping - nearClipping),                -static_cast<T>(1.0),
			static_cast<T>(0.0),                         static_cast<T>(0.0),         -(farClipping * nearClipping) / (farClipping - nearClipping),  static_cast<T>(0.0),
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
	Matrix4<T> Matrix4<T>::Scale(const Vector3<T>& scale) {
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

	template<typename T>
	Vector4<T> operator*(const Matrix4<T>& mat, const Vector4<T>& vec) {
		return Vector4<T>{
			mat.a11 * vec.x + mat.a12 * vec.y + mat.a13 * vec.z + mat.a14 * vec.w,
			mat.a21 * vec.x + mat.a22 * vec.y + mat.a23 * vec.z + mat.a24 * vec.w,
			mat.a31 * vec.x + mat.a32 * vec.y + mat.a33 * vec.z + mat.a34 * vec.w,
			mat.a41 * vec.x + mat.a42 * vec.y + mat.a43 * vec.z + mat.a44 * vec.w,
		};
	}

	template<typename T>
	Vector4<T> operator*(const Vector4<T>& vec, const Matrix4<T>& mat) {
		return Vector4<T>{
			vec.x * mat.a11 + vec.y * mat.a21 + vec.z * mat.a31 + vec.w * mat.a41,
			vec.x * mat.a12 + vec.y * mat.a22 + vec.z * mat.a32 + vec.w * mat.a42,
			vec.x * mat.a13 + vec.y * mat.a23 + vec.z * mat.a33 + vec.w * mat.a43,
			vec.x * mat.a14 + vec.y * mat.a24 + vec.z * mat.a34 + vec.w * mat.a44,
		};
	}
	template<typename T>
	Vector3<T> operator*(const Matrix4<T>& mat, const Vector3<T>& vec) {
		Vector4<T> result{ mat * Vector4<T>{ vec.x, vec.y, vec.z, static_cast<T>(1.f) } };

		return Vector3<T>{ result.x, result.y, result.z };
	}

	template<typename T>
	Vector3<T> operator*(const Vector3<T>& vec, const Matrix4<T>& mat) {
		Vector4<T> result{ Vector4<T>{ vec.x, vec.y, vec.z, static_cast<T>(1.f) } * mat };

		return Vector3<T>{ result.x, result.y, result.z };
	}
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const Nth::Matrix4<T>& mat) {
	return out << mat.to_string();
}
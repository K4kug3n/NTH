#ifndef NTH_MATH_MATRIX4_HPP
#define NTH_MATH_MATRIX4_HPP

#include <string>
#include <array>

namespace Nth {

	template<typename T>
	class Vector3;

	template<typename T>
	class Matrix4 {
	public:
		Matrix4() = default;
		Matrix4(T a11, T a12, T a13, T a14,
				T a21, T a22, T a23, T a24,
				T a31, T a32, T a33, T a34,
				T a41, T a42, T a43, T a44);
		Matrix4(Matrix4<T> const&) = default;
		Matrix4(Matrix4<T>&&) = default;
		~Matrix4() = default;

		Matrix4 operator+(Matrix4 const& mat) const;
		Matrix4 operator+(T scale) const;
		Matrix4 operator-(Matrix4 const& mat) const;
		Matrix4 operator-(T scale) const;
		Matrix4 operator*(Matrix4 const& mat) const;
		Matrix4 operator*(T scale) const;

		Matrix4 & operator+=(Matrix4 const& mat);
		Matrix4 & operator+=(T scale);
		Matrix4 & operator-=(Matrix4 const& mat);
		Matrix4 & operator-=(T scale);
		Matrix4 & operator*=(Matrix4 const& mat);
		Matrix4 & operator*=(T scale);

		bool operator==(Matrix4 const& mat) const;
		bool operator!=(Matrix4 const& mat) const;

		Matrix4& operator=(Matrix4 const&) = default;
		Matrix4& operator=(Matrix4&&) = default;

		std::array<float, 16> toArray() const;
		std::string toString() const;

		T	a11, a12, a13, a14,
			a21, a22, a23, a24,
			a31, a32, a33, a34,
			a41, a42, a43, a44;

		static Matrix4 Identity();
		static Matrix4 Translation(Vector3<T> const& translation);
		static Matrix4 Perspective(float fov, float aspectRatio, float nearClipping, float farClipping);
		static Matrix4 Orthographic(float leftPlane, float rightPlane, float topPlane, float bottomPlane, float nearPlane, float farPlane);
		static Matrix4 Rotation(float angle, Vector3<T> axis);
		static Matrix4 Scale(Vector3<T> const& scale);
	};

	template<typename T>
	Matrix4<T> operator*(T scalar, const Matrix4<T>& mat);

	using Matrix4i = Matrix4<int>;
	using Matrix4ui = Matrix4<unsigned int>;
	using Matrix4f = Matrix4<float>;
	using Matrix4d = Matrix4<double>;
}

template<typename T>
std::ostream& operator<<(std::ostream& out, Nth::Matrix4<T> const& mat);

#include <Math/Matrix4.inl>

#endif



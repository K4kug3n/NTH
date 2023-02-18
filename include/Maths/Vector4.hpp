#ifndef NTH_MATHS_VECTOR4_HPP
#define NTH_MATHS_VECTOR4_HPP

#include <string>

namespace Nth {

	template<typename T>
	class Vector4 {
	public:
		Vector4() = default;
		Vector4(T x, T y, T z, T w);
		Vector4(const Vector4<T>& vec) = default;
		Vector4(Vector4<T>&&) = default;
		~Vector4() = default;

		T dot_product(const Vector4& vec) const;
		T length() const;

		void normalise();

		Vector4 operator+(const Vector4 & vec) const;
		Vector4 operator+(T scale) const;
		Vector4 operator-(const Vector4& vec) const;
		Vector4 operator-(T scale) const;
		Vector4 operator*(const Vector4& vec) const;
		Vector4 operator*(T scale) const;
		Vector4 operator/(T scale) const;

		Vector4& operator+=(const Vector4& vec);
		Vector4& operator+=(T scale);
		Vector4& operator-=(const Vector4& vec);
		Vector4& operator-=(T scale);
		Vector4& operator*=(const Vector4& vec);
		Vector4& operator*=(T scale);
		Vector4& operator/=(T scale);

		bool operator==(const Vector4& vec) const;
		bool operator!=(const Vector4& vec) const;

		Vector4& operator=(const Vector4&) = default;
		Vector4& operator=(Vector4&&) = default;

		std::string to_string() const;

		T x, y, z, w;
	};

	using Vector4i = Vector4<int>;
	using Vector4ui = Vector4<unsigned int>;
	using Vector4f = Vector4<float>;
	using Vector4d = Vector4<double>;
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const Nth::Vector4<T>& vec);

#include <Maths/Vector4.inl>

#endif



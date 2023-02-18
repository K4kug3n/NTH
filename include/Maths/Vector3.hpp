#ifndef NTH_MATHS_VECTOR3_HPP
#define NTH_MATHS_VECTOR3_HPP

#include <string>

namespace Nth {

	template<typename T>
	class Vector3 {
	public:
		Vector3() = default;
		Vector3(T x, T y, T z);
		Vector3(const Vector3<T>&) = default;
		Vector3(Vector3<T>&&) = default;
		~Vector3() = default;

		T dot_product(const Vector3& vec) const;
		T length() const;

		void normalise();

		Vector3 operator+(const Vector3& vec) const;
		Vector3 operator+(T scale) const;
		Vector3 operator-(const Vector3& vec) const;
		Vector3 operator-(T scale) const;
		Vector3 operator*(const Vector3& vec) const;
		Vector3 operator*(T scale) const;
		Vector3 operator/(T scale) const;

		Vector3& operator+=(const Vector3& vec);
		Vector3& operator+=(T scale);
		Vector3& operator-=(const Vector3& vec);
		Vector3& operator-=(T scale);
		Vector3& operator*=(const Vector3& vec);
		Vector3& operator*=(T scale);
		Vector3& operator/=(T scale);

		bool operator==(const Vector3& vec) const;
		bool operator!=(const Vector3& vec) const;

		Vector3& operator=(const Vector3&) = default;
		Vector3& operator=(Vector3&&) = default;

		std::string to_string() const;

		T x, y, z;
	};

	using Vector3i = Vector3<int>;
	using Vector3ui = Vector3<unsigned int>;
	using Vector3f = Vector3<float>;
	using Vector3d = Vector3<double>;
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const Nth::Vector3<T>& vec);

#include <Maths/Vector3.inl>

#endif



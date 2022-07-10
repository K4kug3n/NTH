#ifndef NTH_MATH_VECTOR3_HPP
#define NTH_MATH_VECTOR3_HPP

#include <string>

namespace Nth {

	template<typename T>
	class Vector3 {
	public:
		Vector3() = default;
		Vector3(T x, T y, T z);
		Vector3(Vector3<T> const& vec) = default;
		Vector3(Vector3<T>&&) = default;
		~Vector3() = default;

		T dotProduct(Vector3 const& vec) const;
		T length() const;

		void normalise();

		Vector3 operator+(Vector3 const& vec) const;
		Vector3 operator+(T scale) const;
		Vector3 operator-(Vector3 const& vec) const;
		Vector3 operator-(T scale) const;
		Vector3 operator*(Vector3 const& vec) const;
		Vector3 operator*(T scale) const;
		Vector3 operator/(T scale) const;

		Vector3& operator+=(Vector3 const& vec);
		Vector3& operator+=(T scale);
		Vector3& operator-=(Vector3 const& vec);
		Vector3& operator-=(T scale);
		Vector3& operator*=(Vector3 const& vec);
		Vector3& operator*=(T scale);
		Vector3& operator/=(T scale);

		bool operator==(Vector3 const& vec) const;
		bool operator!=(Vector3 const& vec) const;

		Vector3& operator=(Vector3 const&) = default;
		Vector3& operator=(Vector3&&) = default;

		std::string toString() const;

		T x, y, z;
	};

	using Vector3i = Vector3<int>;
	using Vector3ui = Vector3<unsigned int>;
	using Vector3f = Vector3<float>;
	using Vector3d = Vector3<double>;
}

template<typename T>
std::ostream& operator<<(std::ostream& out, Nth::Vector3<T> const& vec);

#include "Math/Vector3.inl"

#endif



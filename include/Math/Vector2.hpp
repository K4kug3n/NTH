#pragma once

#ifndef NTH_VECTOR2_HPP
#define NTH_VECTOR2_HPP

#include <string>

namespace Nth {

	template<typename T>
	class Vector2 {
	public:
		Vector2() = default;
		Vector2(T x, T y);
		Vector2(Vector2<T> const&) = default;
		Vector2(Vector2<T>&&) = default;
		~Vector2() = default;

		T dotProduct(Vector2 const& vec) const;
		T length() const;

		void normalise();

		Vector2 operator+(const Vector2 & vec) const;
		Vector2 operator+(T scale) const;
		Vector2 operator-(Vector2 const& vec) const;
		Vector2 operator-(T scale) const;
		Vector2 operator*(Vector2 const& vec) const;
		Vector2 operator*(T scale) const;
		Vector2 operator/(T scale) const;

		Vector2& operator+=(Vector2 const& vec);
		Vector2& operator+=(T scale);
		Vector2& operator-=(Vector2 const& vec);
		Vector2& operator-=(T scale);
		Vector2& operator*=(Vector2 const& vec);
		Vector2& operator*=(T scale);
		Vector2& operator/=(T scale);

		bool operator==(Vector2 const& vec) const;
		bool operator!=(Vector2 const& vec) const;

		Vector2& operator=(Vector2 const&) = default;
		Vector2& operator=(Vector2&&) = default;

		std::string toString() const;

		T x, y;
	};

	using Vector2i = Vector2<int>;
	using Vector2ui = Vector2<unsigned int>;
	using Vector2f = Vector2<float>;
	using Vector2d = Vector2<double>;
}

template<typename T>
std::ostream& operator<<(std::ostream& out, Nth::Vector2<T> const& vec);

#include "Math/Vector2.inl"

#endif



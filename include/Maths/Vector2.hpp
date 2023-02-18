#ifndef NTH_MATHS_VECTOR2_HPP
#define NTH_MATHS_VECTOR2_HPP

#include <string>

namespace Nth {

	template<typename T>
	class Vector2 {
	public:
		Vector2() = default;
		Vector2(T x, T y);
		Vector2(const Vector2<T>&) = default;
		Vector2(Vector2<T>&&) = default;
		~Vector2() = default;

		T dotProduct(const Vector2& vec) const;
		T length() const;

		void normalise();

		Vector2 operator+(const Vector2& vec) const;
		Vector2 operator+(T scale) const;
		Vector2 operator-(const Vector2& vec) const;
		Vector2 operator-(T scale) const;
		Vector2 operator*(const Vector2& vec) const;
		Vector2 operator*(T scale) const;
		Vector2 operator/(T scale) const;

		Vector2& operator+=(const Vector2& vec);
		Vector2& operator+=(T scale);
		Vector2& operator-=(const Vector2& vec);
		Vector2& operator-=(T scale);
		Vector2& operator*=(const Vector2& vec);
		Vector2& operator*=(T scale);
		Vector2& operator/=(T scale);

		bool operator==(const Vector2& vec) const;
		bool operator!=(const Vector2& vec) const;

		Vector2& operator=(const Vector2&) = default;
		Vector2& operator=(Vector2&&) = default;

		std::string to_string() const;

		T x, y;
	};

	using Vector2i = Vector2<int>;
	using Vector2ui = Vector2<unsigned int>;
	using Vector2f = Vector2<float>;
	using Vector2d = Vector2<double>;
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const Nth::Vector2<T>& vec);

#include <Maths/Vector2.inl>

#endif



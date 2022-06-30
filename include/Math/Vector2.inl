#include "Math/Vector2.hpp"

#include <sstream>

namespace Nth {
	template<typename T>
	Vector2<T>::Vector2(T x, T y) :
		x(x), y(y){}

	template<typename T>
	T Vector2<T>::dotProduct(Vector2 const& vec) const{
		return x * vec.x + y * vec.y;
	}

	template<typename T>
	T Vector2<T>::length() const {
		return std::sqrt(x * x + y * y);
	}

	template<typename T>
	void Vector2<T>::normalise() {
		*this /= length();
	}

	template<typename T>
	Vector2<T> Vector2<T>::operator+(Vector2 const& vec) const {
		return Vector2<T>{ x + vec.x, y + vec.y };
	}

	template<typename T>
	Vector2<T> Vector2<T>::operator+(T scale) const {
		return Vector2<T>{ x + scale, y + scale };
	}

	template<typename T>
	Vector2<T> Vector2<T>::operator-(Vector2 const& vec) const {
		return Vector2<T>{ x - vec.x, y - vec.y };
	}

	template<typename T>
	Vector2<T> Vector2<T>::operator-(T scale) const {
		return Vector2<T>{ x - scale, y - scale };
	}

	template<typename T>
	Vector2<T> Vector2<T>::operator*(Vector2 const& vec) const {
		return Vector2<T>{ x * vec.x, y * vec.y };
	}

	template<typename T>
	Vector2<T> Vector2<T>::operator*(T scale) const {
		return Vector2<T>{ x * scale, y * scale };
	}

	template<typename T>
	Vector2<T> Vector2<T>::operator/(T scale) const {
		return Vector2{x / scale, y / scale};
	}

	template<typename T>
	Vector2<T>& Vector2<T>::operator+=(Vector2 const& vec) {
		x += vec.x;
		y += vec.y;

		return *this;
	}

	template<typename T>
	Vector2<T>& Vector2<T>::operator+=(T scale) {
		x += scale;
		y += scale;

		return *this;
	}

	template<typename T>
	Vector2<T>& Vector2<T>::operator-=(Vector2 const& vec) {
		x -= vec.x;
		y -= vec.y;

		return *this;
	}

	template<typename T>
	Vector2<T>& Vector2<T>::operator-=(T scale) {
		x -= scale;
		y -= scale;

		return *this;
	}

	template<typename T>
	Vector2<T>& Vector2<T>::operator*=(Vector2 const& vec) {
		x *= vec.x;
		y *= vec.y;

		return *this;
	}

	template<typename T>
	Vector2<T>& Vector2<T>::operator*=(T scale) {
		x *= scale;
		y *= scale;

		return *this;
	}

	template<typename T>
	Vector2<T>& Vector2<T>::operator/=(T scale) {
		x /= scale;
		y /= scale;

		return *this;
	}

	template<typename T>
	bool Vector2<T>::operator==(Vector2 const& vec) const {
		return vec.x == x && vec.y == y;
	}

	template<typename T>
	bool Vector2<T>::operator!=(Vector2 const& vec) const {
		return !(*this == vec);
	}

	template<typename T>
	std::string Vector2<T>::toString() const {
		std::stringstream stream;

		stream << "Vector2(" << x << "," << y << ")";

		return stream.str();
	}

}

template<typename T>
std::ostream& operator<<(std::ostream& out, Nth::Vector2<T> const& vec){
	return out << vec.toString();
}
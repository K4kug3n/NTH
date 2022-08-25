#include <Math/Vector3.hpp>

#include <cmath>
#include <sstream>

namespace Nth {
	template<typename T>
	Vector3<T>::Vector3(T x, T y, T z) :
		x(x), y(y), z(z){}

	template<typename T>
	T Vector3<T>::dotProduct(Vector3 const& vec) const {
		return x * vec.x + y * vec.y + z * vec.z;
	}

	template<typename T>
	T Vector3<T>::length() const {
		return std::sqrt(x * x + y * y + z * z);
	}

	template<typename T>
	void Vector3<T>::normalise() {
		*this /= length();
	}

	template<typename T>
	Vector3<T> Vector3<T>::operator+(Vector3 const& vec) const {
		return Vector3<T>{ x + vec.x, y + vec.y, z + vec.z };
	}

	template<typename T>
	Vector3<T> Vector3<T>::operator+(T scale) const {
		return Vector3<T>{ x + scale, y + scale, z + scale };
	}

	template<typename T>
	Vector3<T> Vector3<T>::operator-(Vector3 const& vec) const {
		return Vector3<T>{ x - vec.x, y - vec.y, z - vec.z };
	}

	template<typename T>
	Vector3<T> Vector3<T>::operator-(T scale) const {
		return Vector3<T>{ x - scale, y - scale, z - scale };
	}

	template<typename T>
	Vector3<T> Vector3<T>::operator*(Vector3 const& vec) const {
		return Vector3<T>{ x * vec.x, y * vec.y, z * vec.z };
	}

	template<typename T>
	Vector3<T> Vector3<T>::operator*(T scale) const {
		return Vector3<T>{ x * scale, y * scale, z * scale };
	}

	template<typename T>
	Vector3<T> Vector3<T>::operator/(T scale) const {
		return Vector3{ x /= scale, y /= scale, z /= scale };
	}

	template<typename T>
	Vector3<T>& Vector3<T>::operator+=(Vector3 const& vec) {
		x += vec.x;
		y += vec.y;
		z += vec.z;

		return *this;
	}

	template<typename T>
	Vector3<T>& Vector3<T>::operator+=(T scale) {
		x += scale;
		y += scale;
		z += scale;

		return *this;
	}

	template<typename T>
	Vector3<T>& Vector3<T>::operator-=(Vector3 const& vec) {
		x -= vec.x;
		y -= vec.y;
		z -= vec.z;

		return *this;
	}

	template<typename T>
	Vector3<T>& Vector3<T>::operator-=(T scale) {
		x -= scale;
		y -= scale;
		z -= scale;

		return *this;
	}

	template<typename T>
	Vector3<T>& Vector3<T>::operator*=(Vector3 const& vec) {
		x *= vec.x;
		y *= vec.y;

		return *this;
	}

	template<typename T>
	Vector3<T>& Vector3<T>::operator*=(T scale) {
		x *= scale;
		y *= scale;
		z *= scale;

		return *this;
	}

	template<typename T>
	Vector3<T>& Vector3<T>::operator/=(T scale) {
		x /= scale;
		y /= scale;
		z /= scale;

		return *this;
	}

	template<typename T>
	bool Vector3<T>::operator==(Vector3 const& vec) const {
		return vec.x == x && vec.y == y && vec.z == z;
	}

	template<typename T>
	inline bool Vector3<T>::operator!=(Vector3 const& vec) const {
		return !(*this == vec);
	}

	template<typename T>
	std::string Vector3<T>::toString() const {
		std::stringstream stream;

		stream << "Vector3(" << x << "," << y << "," << z << ")";

		return stream.str();
	}

}

template<typename T>
std::ostream& operator<<(std::ostream& out, Nth::Vector3<T> const& vec){
	return out << vec.toString();
}
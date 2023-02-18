#include <Maths/Vector4.hpp>

#include <sstream>

namespace Nth {
	template<typename T>
	Vector4<T>::Vector4(T x, T y, T z, T w) :
		x(x), y(y), z(z), w(w){}

	template<typename T>
	T Vector4<T>::dot_product(const Vector4& vec) const {
		return x * vec.x + y * vec.y + z * vec.z + w * vec.w;
	}

	template<typename T>
	T Vector4<T>::length() const {
		return std::sqrt(x * x + y * y + z * z + w * w);
	}

	template<typename T>
	void Vector4<T>::normalise() {
		*this /= length();
	}

	template<typename T>
	Vector4<T> Vector4<T>::operator+(const Vector4& vec) const {
		return Vector4<T>{ x + vec.x, y + vec.y, z + vec.z, w + vec.w };
	}

	template<typename T>
	Vector4<T> Vector4<T>::operator+(T scale) const {
		return Vector4<T>{ x + scale, y + scale, z + scale, w + scale };
	}

	template<typename T>
	Vector4<T> Vector4<T>::operator-(const Vector4& vec) const {
		return Vector4<T>{ x - vec.x, y - vec.y, z - vec.z, w - vec.w };
	}

	template<typename T>
	Vector4<T> Vector4<T>::operator-(T scale) const {
		return Vector4<T>{ x - scale, y - scale, z - scale, w - scale };
	}

	template<typename T>
	Vector4<T> Vector4<T>::operator*(const Vector4& vec) const {
		return Vector4<T>{ x * vec.x, y * vec.y, z * vec.z, w * vec.w };
	}

	template<typename T>
	Vector4<T> Vector4<T>::operator*(T scale) const {
		return Vector4<T>{ x * scale, y * scale, z * scale, w * scale };
	}

	template<typename T>
	Vector4<T> Vector4<T>::operator/(T scale) const {
		return Vector4{ x /= scale, y /= scale, z /= scale, w /= scale };
	}

	template<typename T>
	Vector4<T>& Vector4<T>::operator+=(const Vector4& vec) {
		x += vec.x;
		y += vec.y;
		z += vec.z;
		w += vec.w;

		return *this;
	}

	template<typename T>
	Vector4<T>& Vector4<T>::operator+=(T scale) {
		x += scale;
		y += scale;
		z += scale;
		w += scale;

		return *this;
	}

	template<typename T>
	Vector4<T>& Vector4<T>::operator-=(const Vector4& vec) {
		x -= vec.x;
		y -= vec.y;
		z -= vec.z;
		w -= vec.w;

		return *this;
	}

	template<typename T>
	Vector4<T>& Vector4<T>::operator-=(T scale) {
		x -= scale;
		y -= scale;
		z -= scale;
		w -= scale;

		return *this;
	}

	template<typename T>
	Vector4<T>& Vector4<T>::operator*=(const Vector4& vec) {
		x *= vec.x;
		y *= vec.y;
		z *= vec.z;
		w *= vec.w;

		return *this;
	}

	template<typename T>
	Vector4<T>& Vector4<T>::operator*=(T scale) {
		x *= scale;
		y *= scale;
		z *= scale;
		w *= scale;

		return *this;
	}

	template<typename T>
	Vector4<T>& Vector4<T>::operator/=(T scale) {
		x /= scale;
		y /= scale;
		z /= scale;
		w /= scale;

		return *this;
	}

	template<typename T>
	bool Vector4<T>::operator==(const Vector4& vec) const {
		return vec.x == x && vec.y == y
			&& vec.z == z && vec.w == w;
	}

	template<typename T>
	inline bool Vector4<T>::operator!=(const Vector4& vec) const {
		return !(*this == vec);
	}

	template<typename T>
	std::string Vector4<T>::to_string() const {
		std::stringstream stream;

		stream << "Vector4(" << x << "," << y << "," << z << "," << w << ")";

		return stream.str();
	}
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const Nth::Vector4<T>& vec){
	return out << vec.to_string();
}
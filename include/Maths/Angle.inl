#include <Maths/Angle.hpp>

namespace Nth{
	constexpr float to_radians(float degree) {
		return degree * 3.1415927f / 180.f;
	}
}
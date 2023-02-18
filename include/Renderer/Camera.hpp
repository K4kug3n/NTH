#ifndef NTH_RENDERER_CAMERA_HPP
#define NTH_RENDERER_CAMERA_HPP

#include <Maths/Vector3.hpp>

namespace Nth {
	template<typename T> class Matrix4;
	using Matrix4f = Matrix4<float>;

	enum class CameraType {
		FirstPerson,
		ThirdPerson
	};

	class Camera {
	public:
		Camera();
		Camera(const Camera&) = delete;
		Camera(Camera&&) = default;
		~Camera() = default;

		Vector3f position;
		CameraType type;

		Matrix4f get_view_matrix() const;

		Camera& operator=(const Camera&) = delete;
		Camera& operator=(Camera&&) = default;
	};
}
#endif
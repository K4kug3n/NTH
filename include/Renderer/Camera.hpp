#ifndef NTH_RENDERER_CAMERA_HPP
#define NTH_RENDERER_CAMERA_HPP

#include <Math/Vector3.hpp>
#include <Math/EulerAngle.hpp>

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
		Camera(Camera const&) = delete;
		Camera(Camera&&) = default;
		~Camera() = default;

		Vector3f position;
		EulerAngle direction;
		CameraType type;

		Matrix4f getViewMatrix() const;

		Camera& operator=(Camera const&) = delete;
		Camera& operator=(Camera&&) = default;
	};
}
#endif
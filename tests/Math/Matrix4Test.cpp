#include <catch2/catch.hpp>

#include <Math/Matrix4.hpp>
#include <Math/Angle.hpp>

using namespace Nth;

TEST_CASE("Matrix4", "[Mat4]") {
	SECTION("Operator") {
		Matrix4f identity_1 = Matrix4f::Identity();

		REQUIRE(identity_1 == Matrix4f{ 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f });
		REQUIRE(identity_1 != Matrix4f{ 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f });

		REQUIRE((identity_1 * 1.f) == identity_1);
		REQUIRE((1.f * identity_1) == identity_1);
		REQUIRE((identity_1 * identity_1) == identity_1);

		REQUIRE((identity_1 + 1.f) == Matrix4f{ 2.f, 1.f, 1.f, 1.f, 1.f, 2.f, 1.f, 1.f, 1.f, 1.f, 2.f, 1.f, 1.f, 1.f, 1.f, 2.f });
		REQUIRE((identity_1 - 1.f) == Matrix4f{ 0.f, -1.f, -1.f, -1.f, -1.f, 0.f, -1.f, -1.f, -1.f, -1.f, 0.f, -1.f, -1.f, -1.f, -1.f, 0.f });

		REQUIRE((identity_1 + identity_1) == Matrix4f{ 2.f, 0.f, 0.f, 0.f, 0.f, 2.f, 0.f, 0.f, 0.f, 0.f, 2.f, 0.f, 0.f, 0.f, 0.f, 2.f });
		REQUIRE((identity_1 - identity_1) == Matrix4f{ 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f });
	}

	SECTION("Transformation") {
		Matrix4f translate = Matrix4f::Translation({2.f, 3.f, 4.f });
		REQUIRE(translate == Matrix4f{ 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 2.f, 3.f, 4.f, 1.f });

		Matrix4f rotate = Matrix4f::Rotation(toRadians(90.f), { 0.f, 1.f, 0.f });
		REQUIRE(rotate == Matrix4f{ -4.37113883e-08f, 0.f, -1.f, 0.f, 0.f, 0.999999940f, 0.f, 0.f, 1.f, 0.f, -4.37113883e-08f, 0.f, 0.f, 0.f, 0.f, 1.f });

		Matrix4f perspective = Matrix4f::Perspective(toRadians(45.f), static_cast<float>(1920) / static_cast<float>(1080), 0.1f, 10.f);
		REQUIRE(perspective == Matrix4f{ 1.35799515f, 0.f, 0.f, 0.f, 0.f, 2.41421342f, 0.f, 0.f, 0.f, 0.f, -1.01010108f, -1.f, 0.f, 0.f, -0.101010107f, 0.f });

		Matrix4f scale = Matrix4f::Scale({ 2.f, 3.f, 4.f });
		REQUIRE(scale == Matrix4f{ 2.f, 0.f, 0.f, 0.f, 0.f, 3.f, 0.f, 0.f, 0.f, 0.f, 4.f, 0.f, 0.f, 0.f, 0.f, 1.f });

		Matrix4f orthographic = Matrix4f::Orthographic(1.f, 2.f, 3.f, 4.f, 0.1f, 10.f);
		REQUIRE(orthographic == Matrix4f{ 2.f, 0.f, 0.f, 0.f, 0.f, 2.f, 0.f, 0.f, 0.f, 0.f, -0.101010107f, 0.f, -3.f, -7.f, -0.0101010110f, 1.f });
	}

	SECTION("Product") {
		Matrix4f A{ 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 11.f, 12.f, 13.f, 14.f, 15.f, 16.f };
		Matrix4f B{ 21.f, 22.f, 23.f, 24.f, 25.f, 26.f, 27.f, 28.f, 29.f, 30.f, 31.f, 32.f, 33.f, 34.f, 35.f, 36.f };

		REQUIRE(A * B == Matrix4f{ 650.f, 740.f, 830.f, 920.f, 762.f, 868.f, 974.f, 1080.f, 874.f, 996.f, 1118.f, 1240.f, 986.f, 1124.f, 1262.f, 1400.f });
	}
}
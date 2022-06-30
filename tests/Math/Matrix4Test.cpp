#include <catch2/catch.hpp>

#include "Math/Matrix4.hpp"

using namespace Nth;

TEST_CASE("Matrix4", "[Mat4]") {
	SECTION("Operator") {
		Matrix4f identity_1{ Matrix4f::Identity() };

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
}
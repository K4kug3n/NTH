#include <catch2/catch_all.hpp>

#include "Math/Vector3.hpp"

using namespace Nth;

TEST_CASE("Vector3", "[Vec3]") {
	SECTION("Initialisation") {
		Vector3f vec{ 2.f, 4.f, 6.f };

		REQUIRE(vec.x == 2.f);
		REQUIRE(vec.y == 4.f);
		REQUIRE(vec.z == 6.f);
	}

	SECTION("Transformation") {
		Vector3f vec{ 1.f, 1.f, 1.f };

		REQUIRE(vec.length() == std::sqrt(3.f));

		vec.normalise();

		REQUIRE(vec == Vector3f{ 1.f / std::sqrt(3.f), 1.f / std::sqrt(3.f), 1.f / std::sqrt(3.f) });
	}
}
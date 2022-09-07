#include <catch2/catch_all.hpp>

#include "Math/Vector4.hpp"

using namespace Nth;

TEST_CASE("Vector4", "[Vec4]") {
	SECTION("Initialisation") {
		Vector4f vec{ 2.f, 4.f, 6.f, 7.f };

		REQUIRE(vec.x == 2.f);
		REQUIRE(vec.y == 4.f);
		REQUIRE(vec.z == 6.f);
		REQUIRE(vec.w == 7.f);
	}

	SECTION("Transformation") {
		Vector4f vec{ 1.f, 1.f, 1.f, 1.f };

		REQUIRE(vec.length() == std::sqrt(4.f));

		vec.normalise();

		REQUIRE(vec == Vector4f{ 1.f / std::sqrt(4.f), 1.f / std::sqrt(4.f), 1.f / std::sqrt(4.f), 1.f / std::sqrt(4.f) });
	}
}
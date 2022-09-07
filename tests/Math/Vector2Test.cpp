#include <catch2/catch_all.hpp>

#include "Math/Vector2.hpp"

using namespace Nth;

TEST_CASE("Vector2", "[Vec2]") {
	SECTION("Initialisation") {
		Vector2f vec{ 2.f, 4.f };

		REQUIRE(vec.x == 2.f);
		REQUIRE(vec.y == 4.f);
	}

	SECTION("Transformation") {
		Vector2f vec{ 1.f, 1.f };

		REQUIRE(vec.length() == std::sqrt(2.f));

		vec.normalise();

		REQUIRE(vec == Vector2f{ 1.f / std::sqrt(2.f), 1.f / std::sqrt(2.f) });
	}
}
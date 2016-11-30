#include "../source/applyHomography.hpp"

#include "catch.hpp"

struct Event {
    std::size_t x;
    std::size_t y;
};

TEST_CASE("Apply the homography", "[ApplyHomography]") {
    auto applyHomography = tarsier::make_applyHomography<Event>(
        304,
        240,
        tarsier::Homography{1.0195, -0.0158, 10.1996, 0.0047, 1.0057, 9.9573, 0.0, 0.0},
        [](Event event) -> void {
            REQUIRE(event.x == 111);
            REQUIRE(event.y == 111);
        }
    );
    applyHomography(Event{100, 100});
}

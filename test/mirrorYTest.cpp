#include "../source/mirrorY.hpp"

#include "catch.hpp"

struct Event {
    std::size_t y;
};

TEST_CASE("Invert the y coordinate", "[MirrorY]") {
    auto mirrorY = tarsier::make_mirrorY<Event>(240, [](Event event) -> void {
        REQUIRE(event.y == 100);
    });
    mirrorY(Event{139});
}

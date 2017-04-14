#include "../source/mirrorY.hpp"

#include "catch.hpp"

struct Event {
    uint16_t y;
} __attribute__((packed));

TEST_CASE("Invert the y coordinate", "[MirrorY]") {
    auto mirrorY = tarsier::make_mirrorY<Event, 240>([](Event event) -> void {
        REQUIRE(event.y == 100);
    });
    mirrorY(Event{139});
}

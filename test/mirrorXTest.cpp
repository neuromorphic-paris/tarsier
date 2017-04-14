#include "../source/mirrorX.hpp"

#include "catch.hpp"

struct Event {
    uint16_t x;
} __attribute__((packed));

TEST_CASE("Invert the x coordinate", "[MirrorX]") {
    auto mirrorX = tarsier::make_mirrorX<Event, 304>([](Event event) -> void {
        REQUIRE(event.x == 100);
    });
    mirrorX(Event{203});
}

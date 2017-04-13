#include "../source/mirrorX.hpp"

#include "catch.hpp"

struct Event {
    uint_fast16_t x;
};

TEST_CASE("Invert the x coordinate", "[MirrorX]") {
    auto mirrorX = tarsier::make_mirrorX<Event, 304>([](Event event) -> void {
        REQUIRE(event.x == 100);
    });
    mirrorX(Event{203});
}

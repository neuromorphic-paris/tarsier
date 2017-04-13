#include "../source/shiftY.hpp"

#include "catch.hpp"

struct Event {
    uint_fast16_t y;
};

TEST_CASE("Shift the y coordinate", "[ShiftY]") {
    auto shiftY = tarsier::make_shiftY<Event, 240, -10>([](Event event) -> void {
        REQUIRE(event.y == 0);
    });
    shiftY(Event{5});
    shiftY(Event{10});
}

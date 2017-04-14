#include "../source/shiftX.hpp"

#include "catch.hpp"

struct Event {
    uint64_t x;
} __attribute__((packed));

TEST_CASE("Shift the x coordinate", "[ShiftX]") {
    auto shiftX = tarsier::make_shiftX<Event, 304, 10>([](Event event) -> void {
        REQUIRE(event.x == 210);
    });
    shiftX(Event{300});
    shiftX(Event{200});
}

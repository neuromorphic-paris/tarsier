#include "../source/selectRectangle.hpp"

#include "catch.hpp"

struct Event {
    uint16_t x;
    uint16_t y;
} __attribute__((packed));

TEST_CASE("Filter out events outside the rectangle", "[SelectRectangle]") {
    auto selectRectangle = tarsier::make_selectRectangle<Event, 50, 50, 204, 140>([](Event event) -> void {
        REQUIRE(event.x == 100);
    });
    selectRectangle(Event{300, 200});
    selectRectangle(Event{100, 100});
}

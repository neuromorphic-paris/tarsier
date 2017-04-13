#include "../source/selectDisk.hpp"

#include "catch.hpp"

struct Event {
    uint_fast16_t x;
    uint_fast16_t y;
};

TEST_CASE("Filter out events outside the disk", "[SelectDisk]") {
    auto selectDisk = tarsier::make_selectDisk<Event>(100, 100, 20.0, [](Event event) -> void {
        REQUIRE(event.x == 100);
    });
    selectDisk(Event{200, 200});
    selectDisk(Event{100, 110});
}

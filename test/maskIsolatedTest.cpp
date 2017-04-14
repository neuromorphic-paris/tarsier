#include "../source/maskIsolated.hpp"

#include "catch.hpp"

struct Event {
    uint16_t x;
    uint16_t y;
    uint64_t timestamp;
} __attribute__((packed));

TEST_CASE("Filter out events with low spatial or in time activity", "[MaskIsolated]") {
    auto maskIsolated = tarsier::make_maskIsolated<Event, 304, 240, 10>([](Event event) -> void {
        REQUIRE(event.x == 100);
    });
    maskIsolated(Event{200, 200, 0});
    maskIsolated(Event{200, 202, 1});
    maskIsolated(Event{200, 201, 20});
    maskIsolated(Event{100, 100, 40});
    maskIsolated(Event{100, 101, 41});
}

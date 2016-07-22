#include "../source/maskDisk.hpp"

#include "catch.hpp"

struct Event {
    std::size_t x;
    std::size_t y;
};

TEST_CASE("Filter out events outside the disk", "[MaskDisk]") {
    auto maskDisk = tarsier::make_maskDisk<Event>(std::make_pair(100, 100), 20.0, [](Event event) -> void {
        REQUIRE(event.x == 100);
    });
    maskDisk(Event{200, 200});
    maskDisk(Event{100, 110});
}

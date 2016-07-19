#include "../source/shiftX.hpp"

#include "catch.hpp"

struct Event {
    std::size_t x;
};

TEST_CASE("Shift the x coordinate", "[shiftX]") {
    auto shiftX = tarsier::make_shiftX<Event>(304, 10, [](Event event) -> void {
        REQUIRE(event.x == 210);
    });
    shiftX(Event{300});
    shiftX(Event{200});
}

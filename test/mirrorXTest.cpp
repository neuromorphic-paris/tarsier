#include "../source/mirrorX.hpp"

#include "catch.hpp"

struct Event {
    std::size_t x;
};

TEST_CASE("Invert the x coordinate", "[mirrorX]") {
    auto mirrorX = tarsier::make_mirrorX<Event>(304, [](Event event) -> void {
        REQUIRE(event.x == 100);
    });
    mirrorX(Event{203});
}

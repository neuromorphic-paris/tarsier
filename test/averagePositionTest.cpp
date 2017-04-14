#include "../source/averagePosition.hpp"

#include "catch.hpp"

struct Event {
    uint16_t x;
    uint16_t y;
}  __attribute__((packed));

TEST_CASE("Average the position of the given events", "[AveragePosition]") {
    auto averagePosition = tarsier::make_averagePosition<Event>(0.5, 10, [](tarsier::Position position) -> void {
        REQUIRE(position.x == 100);
        REQUIRE(position.y == 50);
    });
    for (std::size_t index = 0; index < 10; ++index) {
        averagePosition(Event{0, 0});
    }
    averagePosition(Event{200, 100});
}

#include "../source/average_position.hpp"
#include "../third_party/Catch2/single_include/catch.hpp"

struct event {
    uint16_t x;
    uint16_t y;
};

struct position {
    float x;
    float y;
};

TEST_CASE("Average the position of the given events", "[average_position]") {
    auto first_received = false;
    auto average_position = tarsier::make_average_position<event, position>(
        0.0,
        0.0,
        0.5,
        [](event event, float x, float y) -> position {
            return {x, y};
        },
        [&](position position) -> void {
            if (first_received) {
                REQUIRE(position.x == 100);
                REQUIRE(position.y == 50);
            } else {
                first_received = true;
            }
        });
    average_position(event{0, 0});
    average_position(event{200, 100});
}

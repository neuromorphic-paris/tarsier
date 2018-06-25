#include "../source/shift_y.hpp"
#include "../third_party/Catch2/single_include/catch.hpp"

struct event {
    uint64_t y;
};

TEST_CASE("Shift the y coordinate", "[shift_y]") {
    auto shift_y = tarsier::make_shift_y<event>(240, 10, [](event event) -> void { REQUIRE(event.y == 145); });
    shift_y(event{235});
    shift_y(event{135});
}

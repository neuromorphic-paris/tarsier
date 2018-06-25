#include "../source/shift_x.hpp"
#include "../third_party/Catch2/single_include/catch.hpp"

struct event {
    uint64_t x;
};

TEST_CASE("Shift the x coordinate", "[shift_x]") {
    auto shift_x = tarsier::make_shift_x<event>(320, 10, [](event event) -> void { REQUIRE(event.x == 210); });
    shift_x(event{315});
    shift_x(event{200});
}

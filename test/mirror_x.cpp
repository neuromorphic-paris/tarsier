#include "../source/mirror_x.hpp"
#include "../third_party/Catch2/single_include/catch.hpp"

struct event {
    uint16_t x;
};

TEST_CASE("Invert the x coordinate", "[mirror_x]") {
    auto mirror_x = tarsier::make_mirror_x<event>(320, [](event event) -> void { REQUIRE(event.x == 100); });
    mirror_x(event{219});
}

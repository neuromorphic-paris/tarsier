#include "../source/mirror_y.hpp"
#include "../third_party/Catch2/single_include/catch.hpp"

struct event {
    uint16_t y;
};

TEST_CASE("Invert the y coordinate", "[mirror_y]") {
    auto mirror_y = tarsier::make_mirror_y<event>(240, [](event event) -> void { REQUIRE(event.y == 100); });
    mirror_y(event{139});
}

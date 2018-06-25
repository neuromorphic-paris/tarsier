#include "../source/stitch.hpp"
#include "../third_party/Catch2/single_include/catch.hpp"

struct threshold_crossing {
    uint64_t t;
    uint16_t x;
    uint16_t y;
    bool is_second;
};

struct event {
    uint16_t x;
    uint16_t y;
    uint64_t delta_t;
};

TEST_CASE("Stitch an threshold crossings stream", "[stitch]") {
    auto stitch = tarsier::make_stitch<threshold_crossing, event>(
        320,
        240,
        [](threshold_crossing threshold_crossing, uint64_t delta_t) -> event {
            return {threshold_crossing.x, threshold_crossing.y, delta_t};
        },
        [](event event) -> void { REQUIRE(event.delta_t == 200); });
    stitch(threshold_crossing{0, 200, 100, false});
    stitch(threshold_crossing{100, 200, 0, false});
    stitch(threshold_crossing{200, 200, 100, true});
}

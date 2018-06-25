#include "../source/mask_isolated.hpp"
#include "../third_party/Catch2/single_include/catch.hpp"

struct event {
    uint64_t t;
    uint16_t x;
    uint16_t y;
};

TEST_CASE("Filter out events with low spatial or temporal activity", "[mask_isolated]") {
    auto mask_isolated =
        tarsier::make_mask_isolated<event>(320, 240, 10, [](event event) -> void { REQUIRE(event.x == 100); });
    mask_isolated(event{0, 200, 200});
    mask_isolated(event{1, 200, 202});
    mask_isolated(event{20, 200, 201});
    mask_isolated(event{40, 100, 100});
    mask_isolated(event{41, 100, 101});
}

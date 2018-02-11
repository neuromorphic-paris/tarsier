#include "../source/mask_isolated.hpp"
#include "../third_party/Catch2/single_include/catch.hpp"

struct event {
    uint16_t x;
    uint16_t y;
    uint64_t t;
} __attribute__((packed));

TEST_CASE("Filter out events with low spatial or temporal activity", "[mask_isolated]") {
    auto mask_isolated =
        tarsier::make_mask_isolated<event>(304, 240, 10, [](event event) -> void { REQUIRE(event.x == 100); });
    mask_isolated(event{200, 200, 0});
    mask_isolated(event{200, 202, 1});
    mask_isolated(event{200, 201, 20});
    mask_isolated(event{100, 100, 40});
    mask_isolated(event{100, 101, 41});
}

#include "../source/compute_flow.hpp"
#include "../third_party/Catch2/single_include/catch.hpp"

struct event {
    uint16_t x;
    uint16_t y;
    uint64_t t;
} __attribute__((packed));

struct flow {
    uint16_t x;
    uint16_t y;
    uint64_t t;
    double vx;
    double vy;
} __attribute__((packed));

TEST_CASE("Compute the optical flow from the given events", "[compute_flow]") {
    flow expected_flow{
        100,
        100,
        2010000,
        0.00068548387096356686,
        0.00026729559748675557,
    };
    auto flow_generated = false;
    auto compute_flow = tarsier::make_compute_flow<event, flow>(
        304,
        240,
        2,
        1000000,
        10,
        0.1,
        [](event event, double vx, double vy) -> flow {
            return {event.x, event.y, event.t, vx, vy};
        },
        [&](flow flow) -> void {
            flow_generated = true;
            REQUIRE(flow.x == expected_flow.x);
            REQUIRE(flow.y == expected_flow.y);
            REQUIRE(flow.t == expected_flow.t);
            REQUIRE(std::abs(flow.vx - expected_flow.vx) / expected_flow.vx < 1e-3);
            REQUIRE(std::abs(flow.vy - expected_flow.vy) / expected_flow.vy < 1e-3);
        });
    compute_flow(event{100 - 2, 100 - 2, 2000000});
    compute_flow(event{100 - 1, 100 - 2, 2001000});
    compute_flow(event{100 - 0, 100 - 2, 2002000});
    compute_flow(event{100 - 2, 100 - 1, 2003000});
    compute_flow(event{100 + 1, 100 - 2, 2004000});
    compute_flow(event{100 - 1, 100 - 1, 2005000});
    compute_flow(event{100 - 0, 100 - 1, 2006000});
    compute_flow(event{100 - 2, 100 - 0, 2007000});
    compute_flow(event{100 + 1, 100 - 1, 2008000});
    compute_flow(event{100, 100, 2010000});
    REQUIRE(flow_generated);
}

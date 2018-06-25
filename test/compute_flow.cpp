#include "../source/compute_flow.hpp"
#include "../third_party/Catch2/single_include/catch.hpp"

struct event {
    uint64_t t;
    uint16_t x;
    uint16_t y;
};

struct flow {
    uint64_t t;
    uint16_t x;
    uint16_t y;
    float vx;
    float vy;
};

TEST_CASE("Compute the optical flow from the given events", "[compute_flow]") {
    flow expected_flow{
        2010000,
        100,
        100,
        0.0000904721018f,
        0.000232017177f,
    };
    auto flow_generated = false;
    auto compute_flow = tarsier::make_compute_flow<event, flow>(
        320,
        240,
        2,
        1000000,
        10,
        [](event event, float vx, float vy) -> flow {
            return {event.t, event.x, event.y, vx, vy};
        },
        [&](flow flow) -> void {
            flow_generated = true;
            REQUIRE(flow.t == expected_flow.t);
            REQUIRE(flow.x == expected_flow.x);
            REQUIRE(flow.y == expected_flow.y);
            REQUIRE(std::abs(flow.vx - expected_flow.vx) / expected_flow.vx < 1e-3f);
            REQUIRE(std::abs(flow.vy - expected_flow.vy) / expected_flow.vy < 1e-3f);
        });
    compute_flow(event{2000000, 100 - 2, 100 - 2});
    compute_flow(event{2001000, 100 - 1, 100 - 2});
    compute_flow(event{2002000, 100 - 0, 100 - 2});
    compute_flow(event{2003000, 100 - 2, 100 - 1});
    compute_flow(event{2004000, 100 + 1, 100 - 2});
    compute_flow(event{2005000, 100 - 1, 100 - 1});
    compute_flow(event{2006000, 100 - 0, 100 - 1});
    compute_flow(event{2007000, 100 - 2, 100 - 0});
    compute_flow(event{2008000, 100 + 1, 100 - 1});
    compute_flow(event{2010000, 100, 100});
    REQUIRE(flow_generated);
}

#include "../source/compute_activity.hpp"
#include "../third_party/Catch2/single_include/catch.hpp"

struct event {
    uint64_t t;
    uint16_t x;
    uint16_t y;
};

struct activity {
    uint64_t t;
    uint16_t x;
    uint16_t y;
    float potential;
};

TEST_CASE("compute the activity from the given events", "[compute_activity]") {
    std::vector<float> expected_potentials{1.0f, 1.9999000049998332f, 1.0f, 1.9999000049998332f, 1.0000908225624412f};
    std::size_t index = 0;
    auto compute_activity = tarsier::make_compute_activity<event, activity>(
        320,
        240,
        10000,
        [](event event, float potential) -> activity {
            return {event.t, event.x, event.y, potential};
        },
        [&](activity activity) -> void {
            REQUIRE(std::abs(activity.potential - expected_potentials[index]) / expected_potentials[index] < 1e-3);
            ++index;
        });
    compute_activity(event{100000, 100, 100});
    compute_activity(event{100001, 100, 100});
    compute_activity(event{100002, 101, 100});
    compute_activity(event{100003, 101, 100});
    compute_activity(event{200000, 101, 100});
}

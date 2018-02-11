#include "../source/compute_activity.hpp"
#include "../third_party/Catch2/single_include/catch.hpp"

struct event {
    uint16_t x;
    uint16_t y;
    uint64_t t;
} __attribute__((packed));

struct activity {
    uint16_t x;
    uint16_t y;
    uint64_t t;
    double potential;
};

TEST_CASE("compute the activity from the given events", "[compute_activity]") {
    std::vector<double> expected_potentials{1.0, 1.9999000049998332, 1.0, 1.9999000049998332, 1.0000908225624412};
    std::size_t index = 0;
    auto compute_activity = tarsier::make_compute_activity<event, activity>(
        304,
        240,
        10000,
        [](event event, double potential) -> activity {
            return {event.x, event.y, event.t, potential};
        },
        [&](activity activity) -> void {
            REQUIRE(std::abs(activity.potential - expected_potentials[index]) / expected_potentials[index] < 1e-3);
            ++index;
        });
    compute_activity(event{100, 100, 100000});
    compute_activity(event{100, 100, 100001});
    compute_activity(event{101, 100, 100002});
    compute_activity(event{101, 100, 100003});
    compute_activity(event{101, 100, 200000});
}

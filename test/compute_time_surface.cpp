#include "../source/compute_time_surface.hpp"
#include "../third_party/Catch2/single_include/catch.hpp"
#include <algorithm>

const uint16_t spatial_window = 2;
const auto projections_size = (2 * spatial_window + 1) * (2 * spatial_window + 1);

struct event {
    uint16_t x;
    uint16_t y;
    uint64_t t;
    bool polarity;
} __attribute__((packed));

struct time_surface {
    uint16_t x;
    uint16_t y;
    uint64_t t;
    std::array<double, projections_size> true_projections;
    std::array<double, projections_size> false_projections;
} __attribute__((packed));

TEST_CASE("Compute time surfaces from events", "[compute_time_surface]") {
    time_surface expected_time_surface{100, 100, 2010000};
    expected_time_surface.true_projections[2] = 0.00033546262790251185;
    expected_time_surface.true_projections[3] = 0.0024787521766663585;
    expected_time_surface.true_projections[7] = 0.018315638888734179;
    expected_time_surface.true_projections[8] = 0.1353352832366127;
    expected_time_surface.false_projections[1] = 0.00012340980408667956;
    expected_time_surface.false_projections[5] = 0.00091188196555451624;
    expected_time_surface.false_projections[6] = 0.006737946999085467;
    expected_time_surface.false_projections[10] = 0.049787068367863944;
    expected_time_surface.false_projections[12] = 1.0;
    std::size_t count = 0;
    auto compute_time_surface = tarsier::make_compute_time_surface<event, bool, time_surface, spatial_window>(
        304,
        240,
        10000,
        1000,
        [](event event, std::array<std::pair<double, bool>, projections_size> projections_and_polarities) {
            time_surface time_surface{event.x, event.y, event.t};
            for (std::size_t index = 0; index < projections_size; ++index) {
                if (projections_and_polarities[index].second) {
                    time_surface.true_projections[index] = projections_and_polarities[index].first;
                } else {
                    time_surface.false_projections[index] = projections_and_polarities[index].first;
                }
            }
            return time_surface;
        },
        [&](time_surface time_surface) {
            ++count;
            if (count == 10) {
                REQUIRE(time_surface.x == expected_time_surface.x);
                REQUIRE(time_surface.y == expected_time_surface.y);
                REQUIRE(time_surface.t == expected_time_surface.t);
                for (std::size_t index = 0; index < projections_size; ++index) {
                    REQUIRE(
                        std::abs(time_surface.true_projections[index] - expected_time_surface.true_projections[index])
                        <= 1e-3 * expected_time_surface.true_projections[index]);
                    REQUIRE(
                        std::abs(time_surface.false_projections[index] - expected_time_surface.false_projections[index])
                        <= 1e-3 * expected_time_surface.false_projections[index]);
                }
            }
        });
    compute_time_surface(event{100 - 2, 100 - 2, 2000000, true});
    compute_time_surface(event{100 - 1, 100 - 2, 2001000, false});
    compute_time_surface(event{100 - 0, 100 - 2, 2002000, true});
    compute_time_surface(event{100 - 2, 100 - 1, 2003000, false});
    compute_time_surface(event{100 + 1, 100 - 2, 2004000, true});
    compute_time_surface(event{100 - 1, 100 - 1, 2005000, false});
    compute_time_surface(event{100 - 0, 100 - 1, 2006000, true});
    compute_time_surface(event{100 - 2, 100 - 0, 2007000, false});
    compute_time_surface(event{100 + 1, 100 - 1, 2008000, true});
    compute_time_surface(event{100, 100, 2010000, false});
}

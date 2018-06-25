#include "../source/compute_time_surface.hpp"
#include "../third_party/Catch2/single_include/catch.hpp"
#include <algorithm>

const uint16_t spatial_window = 2;
const auto projections_size = (2 * spatial_window + 1) * (2 * spatial_window + 1);

struct event {
    uint64_t t;
    uint16_t x;
    uint16_t y;
    bool polarity;
};

struct time_surface {
    uint64_t t;
    uint16_t x;
    uint16_t y;
    std::array<float, projections_size> true_projections;
    std::array<float, projections_size> false_projections;
};

TEST_CASE("Compute time surfaces from events", "[compute_time_surface]") {
    time_surface expected_time_surface{2010000, 100, 100};
    expected_time_surface.true_projections[2] = 0.00033546262790251185f;
    expected_time_surface.true_projections[3] = 0.0024787521766663585f;
    expected_time_surface.true_projections[7] = 0.018315638888734179f;
    expected_time_surface.true_projections[8] = 0.1353352832366127f;
    expected_time_surface.false_projections[1] = 0.00012340980408667956f;
    expected_time_surface.false_projections[5] = 0.00091188196555451624f;
    expected_time_surface.false_projections[6] = 0.006737946999085467f;
    expected_time_surface.false_projections[10] = 0.049787068367863944f;
    expected_time_surface.false_projections[12] = 1.0f;
    std::size_t count = 0;
    auto compute_time_surface = tarsier::make_compute_time_surface<event, bool, time_surface, spatial_window>(
        320,
        240,
        10000,
        1000,
        [](event event, std::array<std::pair<float, bool>, projections_size> projections_and_polarities) {
            time_surface time_surface{event.t, event.x, event.y};
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
    compute_time_surface(event{2000000, 100 - 2, 100 - 2, true});
    compute_time_surface(event{2001000, 100 - 1, 100 - 2, false});
    compute_time_surface(event{2002000, 100 - 0, 100 - 2, true});
    compute_time_surface(event{2003000, 100 - 2, 100 - 1, false});
    compute_time_surface(event{2004000, 100 + 1, 100 - 2, true});
    compute_time_surface(event{2005000, 100 - 1, 100 - 1, false});
    compute_time_surface(event{2006000, 100 - 0, 100 - 1, true});
    compute_time_surface(event{2007000, 100 - 2, 100 - 0, false});
    compute_time_surface(event{2008000, 100 + 1, 100 - 1, true});
    compute_time_surface(event{2010000, 100, 100, false});
}

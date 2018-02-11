#include "../source/track_blob.hpp"
#include "../third_party/Catch2/single_include/catch.hpp"

struct event {
    uint16_t x;
    uint16_t y;
} __attribute__((packed));

struct blob {
    double x;
    double y;
    double squared_sigma_x;
    double sigma_xy;
    double squared_sigma_y;
} __attribute__((packed));

TEST_CASE("Average the events with a Gaussian blob", "[track_blob]") {
    blob expected_blob{0.2, 0.1, 50, 30, 10};
    auto first_received = false;
    auto track_blob = tarsier::make_track_blob<event, blob>(
        0.0,
        0.0,
        10.0,
        10.0,
        0.0,
        0.999,
        0.999,
        [](event event, double x, double y, double squared_sigma_x, double sigma_xy, double squared_sigma_y)
            -> blob {
            return {x, y, squared_sigma_x, sigma_xy, squared_sigma_y};
        },
        [&](blob blob) -> void {
            if (first_received) {
                REQUIRE(std::abs(blob.x - expected_blob.x) / expected_blob.x < 1e-3);
                REQUIRE(std::abs(blob.y - expected_blob.y) / expected_blob.y < 1e-3);
                REQUIRE(
                    std::abs(blob.squared_sigma_x - expected_blob.squared_sigma_x) / expected_blob.squared_sigma_x
                    < 1e-3);
                REQUIRE(std::abs(blob.sigma_xy - expected_blob.sigma_xy) / expected_blob.sigma_xy < 1e-3);
                REQUIRE(
                    std::abs(blob.squared_sigma_y - expected_blob.squared_sigma_y) / expected_blob.squared_sigma_y
                    < 1e-3);
            } else {
                first_received = true;
            }
        });
    track_blob(event{0, 0});
    track_blob(event{200, 100});
}

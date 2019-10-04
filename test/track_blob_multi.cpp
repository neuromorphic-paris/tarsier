#include "../source/track_blob_multi.hpp"
#include "../third_party/Catch2/single_include/catch.hpp"
#include <vector>
struct Event {
    uint16_t x;
    uint16_t y;
};

struct Blob {
    float x;
    float y;
    float sigma_x_squared;
    float sigma_xy;
    float sigma_y_squared;
};

struct MultiBlobs {
    uint16_t id;
    std::vector<Blob> blobs;
};

TEST_CASE("Average the events with multiple Gaussian blobs", "[track_blob_multi]") {
    MultiBlobs multi_blobs_initial{0, {{2.0f, 2.0f, 0.16f, 0.0f, 0.16f},
                                       {7.0f, 2.0f, 0.16f, 0.0f, 0.16f}}};
    MultiBlobs expected_multi_blobs_0{0, {{2.5f, 2.5f, 0.58f, 0.5f, 0.58f},
                                          {7.0f, 2.0f, 0.16f, 0.0f, 0.16f}}};
    MultiBlobs expected_multi_blobs_1{1, {{2.5f, 2.5f, 0.58f, 0.5f, 0.58f},
                                          {7.5f, 2.5f, 0.58f, 0.5f, 0.58f}}};

    auto first_received = false;
    auto track_blob_multi = tarsier::make_track_blob_multi<Event, MultiBlobs>(
        multi_blobs_initial,
        0.001f,
        0.5f,
        0.5f,
        [](Event event, MultiBlobs multi_blobs) -> MultiBlobs { return {multi_blobs}; },
        [&](MultiBlobs multi_blobs) -> void {
            if (first_received) {
                REQUIRE(multi_blobs.id == expected_multi_blobs_1.id);

                REQUIRE(std::abs(multi_blobs.blobs[0].x - expected_multi_blobs_1.blobs[0].x) / expected_multi_blobs_1.blobs[0].x < 1e-3f);
                REQUIRE(std::abs(multi_blobs.blobs[0].y - expected_multi_blobs_1.blobs[0].y) / expected_multi_blobs_1.blobs[0].y < 1e-3f);
                REQUIRE(std::abs(multi_blobs.blobs[0].sigma_x_squared - expected_multi_blobs_1.blobs[0].sigma_x_squared) / expected_multi_blobs_1.blobs[0].sigma_x_squared < 1e-3f);
                REQUIRE(std::abs(multi_blobs.blobs[0].sigma_xy - expected_multi_blobs_1.blobs[0].sigma_xy) / (expected_multi_blobs_1.blobs[0].sigma_xy + 1e-3f) < 1e-3f);
                REQUIRE(std::abs(multi_blobs.blobs[0].sigma_y_squared - expected_multi_blobs_1.blobs[0].sigma_y_squared) / expected_multi_blobs_1.blobs[0].sigma_y_squared < 1e-3f);

                REQUIRE(std::abs(multi_blobs.blobs[1].x - expected_multi_blobs_1.blobs[1].x) / expected_multi_blobs_1.blobs[1].x < 1e-3f);
                REQUIRE(std::abs(multi_blobs.blobs[1].y - expected_multi_blobs_1.blobs[1].y) / expected_multi_blobs_1.blobs[1].y < 1e-3f);
                REQUIRE(std::abs(multi_blobs.blobs[1].sigma_x_squared - expected_multi_blobs_1.blobs[1].sigma_x_squared) / expected_multi_blobs_1.blobs[1].sigma_x_squared < 1e-3f);
                REQUIRE(std::abs(multi_blobs.blobs[1].sigma_xy - expected_multi_blobs_1.blobs[1].sigma_xy) / (expected_multi_blobs_1.blobs[1].sigma_xy + 1e-3f) < 1e-3f);
                REQUIRE(std::abs(multi_blobs.blobs[1].sigma_y_squared - expected_multi_blobs_1.blobs[1].sigma_y_squared) / expected_multi_blobs_1.blobs[1].sigma_y_squared < 1e-3f);
            } else {
                REQUIRE(multi_blobs.id == expected_multi_blobs_0.id);

                REQUIRE(std::abs(multi_blobs.blobs[0].x - expected_multi_blobs_0.blobs[0].x) / expected_multi_blobs_0.blobs[0].x < 1e-3f);
                REQUIRE(std::abs(multi_blobs.blobs[0].y - expected_multi_blobs_0.blobs[0].y) / expected_multi_blobs_0.blobs[0].y < 1e-3f);
                REQUIRE(std::abs(multi_blobs.blobs[0].sigma_x_squared - expected_multi_blobs_0.blobs[0].sigma_x_squared) / expected_multi_blobs_0.blobs[0].sigma_x_squared < 1e-3f);
                REQUIRE(std::abs(multi_blobs.blobs[0].sigma_xy - expected_multi_blobs_0.blobs[0].sigma_xy) / (expected_multi_blobs_0.blobs[0].sigma_xy + 1e-3f) < 1e-3f);
                REQUIRE(std::abs(multi_blobs.blobs[0].sigma_y_squared - expected_multi_blobs_0.blobs[0].sigma_y_squared) / expected_multi_blobs_0.blobs[0].sigma_y_squared < 1e-3f);

                REQUIRE(std::abs(multi_blobs.blobs[1].x - expected_multi_blobs_0.blobs[1].x) / expected_multi_blobs_0.blobs[1].x < 1e-3f);
                REQUIRE(std::abs(multi_blobs.blobs[1].y - expected_multi_blobs_0.blobs[1].y) / expected_multi_blobs_0.blobs[1].y < 1e-3f);
                REQUIRE(std::abs(multi_blobs.blobs[1].sigma_x_squared - expected_multi_blobs_0.blobs[1].sigma_x_squared) / expected_multi_blobs_0.blobs[1].sigma_x_squared < 1e-3f);
                REQUIRE(std::abs(multi_blobs.blobs[1].sigma_xy - expected_multi_blobs_0.blobs[1].sigma_xy) / (expected_multi_blobs_0.blobs[1].sigma_xy + 1e-3f) < 1e-3f);
                REQUIRE(std::abs(multi_blobs.blobs[1].sigma_y_squared - expected_multi_blobs_0.blobs[1].sigma_y_squared) / expected_multi_blobs_0.blobs[1].sigma_y_squared < 1e-3f);

                first_received = true;
            }
        });
    track_blob_multi(Event{3, 3});
    track_blob_multi(Event{8, 3});
}

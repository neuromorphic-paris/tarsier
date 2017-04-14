#include "../source/trackBlobs.hpp"

#include "catch.hpp"

struct Event {
    uint64_t x;
    uint64_t y;
    uint64_t timestamp;
};

TEST_CASE("Track gaussian blobs of incoming events", "[TrackBlobs]") {
    auto promotedStep = false;
    auto updatedStep = false;
    auto demotedStep = false;
    auto hiddenPromotedStep = false;
    auto hiddenUpdatedStep = false;
    auto deletedStep = false;

    auto trackBlobs = tarsier::make_trackBlobs<Event>(
        {
            tarsier::Blob{25, 25, 70, 0, 70},
            tarsier::Blob{75, 25, 70, 0, 70},
            tarsier::Blob{25, 75, 70, 0, 70},
            tarsier::Blob{75, 75, 70, 0, 70},
        },
         1e3, // activityDecay
           0, // minimumProbability
        0.38, // promotionActivity
         0.2, // deletionActivity
         0.9, // meanInertia
         0.9, // covarianceInertia
         0.2, // repulsionStrength
          10, // repulsionLength
         0.2, // attractionStrength
          30, // attractionResetDistance
        1000, // pairwiseCalculationsToSkip
        [&promotedStep](std::size_t id, const tarsier::Blob& blob) {
            REQUIRE(id == 4);
            promotedStep = true;
        },
        [&updatedStep](std::size_t id, const tarsier::Blob& blob) {
            REQUIRE(id == 4);
            updatedStep = true;
        },
        [&demotedStep](std::size_t id, const tarsier::Blob& blob) {
            REQUIRE(id == 4);
            demotedStep = true;
        },
        [&hiddenPromotedStep](std::size_t id, const tarsier::Blob& blob) {
            hiddenPromotedStep = true;
        },
        [&hiddenUpdatedStep](std::size_t id, const tarsier::Blob& blob) {
            hiddenUpdatedStep = true;
        },
        [](std::size_t id, const tarsier::Blob&) {},
        [&deletedStep](std::size_t id, const tarsier::Blob& blob) {
            REQUIRE(id == 4);
            deletedStep = true;
        }
    );

    for (uint64_t timestamp = 0; timestamp < 20000; timestamp += 10) {
        int_fast64_t xOffset = 0;
        int_fast64_t yOffset = 0;
        if (timestamp % 40 == 0) {
            xOffset = 5;
        } else if (timestamp % 40 == 10) {
            yOffset = 5;
        } else if (timestamp % 40 == 20) {
            xOffset = -5;
        } else {
            yOffset = -5;
        }
        trackBlobs(Event{static_cast<uint64_t>(25 + xOffset), static_cast<uint64_t>(25 + yOffset), timestamp});
    }
    for (uint64_t timestamp = 20000; timestamp < 40000; timestamp += 10) {
        int64_t xOffset = 0;
        int64_t yOffset = 0;
        if (timestamp % 40 == 0) {
            xOffset = 15;
        } else if (timestamp % 40 == 10) {
            yOffset = 15;
        } else if (timestamp % 40 == 20) {
            xOffset = -15;
        } else {
            yOffset = -15;
        }
        uint64_t x = 25;
        uint64_t y = 25;
        if (timestamp % 30 == 0) {
            x += 50;
        } else if (timestamp % 30 == 10) {
            y += 50;
        } else {
            x += 50;
            y += 50;
        }
        trackBlobs(Event{static_cast<uint64_t>(x + xOffset), static_cast<uint64_t>(y + yOffset), timestamp});
    }

    REQUIRE(promotedStep);
    REQUIRE(updatedStep);
    REQUIRE(demotedStep);
    REQUIRE(hiddenPromotedStep);
    REQUIRE(hiddenUpdatedStep);
    REQUIRE(deletedStep);
}

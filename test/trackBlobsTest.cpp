#include "../source/trackBlobs.hpp"

#include "catch.hpp"

struct Event {
    std::size_t x;
    std::size_t y;
    int64_t timestamp;
};

TEST_CASE("Track gaussian blobs of incoming events", "[TrackBlobs]") {
    auto promotedStep = false;
    auto updatedStep = false;
    auto demotedStep = false;
    auto deletedStep = false;
    auto trackBlobs = tarsier::make_trackBlobs<Event>(
        {
            tarsier::Blob{25, 25, 70, 0, 70},
            tarsier::Blob{75, 25, 70, 0, 70},
            tarsier::Blob{25, 75, 70, 0, 70},
            tarsier::Blob{75, 75, 70, 0, 70},
        },
           0, // initialTimestamp
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
           0, // eventsToInhibit
        [&promotedStep](std::size_t id, const tarsier::Blob& blob) {
            REQUIRE(id == 0);
            promotedStep = true;
        },
        [&updatedStep](std::size_t id, const tarsier::Blob& blob) {
            REQUIRE(id == 0);
            updatedStep = true;
        },
        [&demotedStep](std::size_t id, const tarsier::Blob& blob) {
            REQUIRE(id == 0);
            demotedStep = true;
        },
        [&deletedStep](std::size_t id, const tarsier::Blob& blob) {
            REQUIRE(id == 0);
            deletedStep = true;
        }
    );

    for (auto timestamp = static_cast<int64_t>(0); timestamp < 20000; timestamp += 10) {
        auto xOffset = static_cast<int64_t>(0);
        auto yOffset = static_cast<int64_t>(0);
        if (timestamp % 40 == 0) {
            xOffset = 5;
        } else if (timestamp % 40 == 10) {
            yOffset = 5;
        } else if (timestamp % 40 == 20) {
            xOffset = -5;
        } else {
            yOffset = -5;
        }
        trackBlobs(Event{static_cast<std::size_t>(25 + xOffset), static_cast<std::size_t>(25 + yOffset), timestamp});
    }
    for (auto timestamp = static_cast<int64_t>(20000); timestamp < 40000; timestamp += 10) {
        auto xOffset = static_cast<int64_t>(0);
        auto yOffset = static_cast<int64_t>(0);
        if (timestamp % 40 == 0) {
            xOffset = 15;
        } else if (timestamp % 40 == 10) {
            yOffset = 15;
        } else if (timestamp % 40 == 20) {
            xOffset = -15;
        } else {
            yOffset = -15;
        }
        auto x = static_cast<std::size_t>(25);
        auto y = static_cast<std::size_t>(25);
        if (timestamp % 30 == 0) {
            x += 50;
        } else if (timestamp % 30 == 10) {
            y += 50;
        } else {
            x += 50;
            y += 50;
        }
        trackBlobs(Event{x + xOffset, y + yOffset, timestamp});
    }

    REQUIRE(promotedStep);
    REQUIRE(updatedStep);
    REQUIRE(demotedStep);
    REQUIRE(deletedStep);
}

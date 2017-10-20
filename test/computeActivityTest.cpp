#include "../source/computeActivity.hpp"

#include "catch.hpp"

struct Event {
  uint64_t timestamp;
} __attribute__((packed));

struct ActivityEvent {
  uint64_t timestamp;
  double activity;
};

const uint64_t lifespan = 30000;

std::vector<double> expectedActivities;

TEST_CASE("Compute the event activity in a scene", "[ComputeActivity]") {
  auto activityEventGenerated = false;
  auto computeActivity =
      tarsier::make_computeActivity<Event, ActivityEvent, lifespan>(
          [](Event event, double activity) -> ActivityEvent {
            return ActivityEvent{event.timestamp, activity};
          },
          [&activityEventGenerated](ActivityEvent activityEvent) -> void {
            activityEventGenerated = true;
            expectedActivities.push_back(activityEvent.activity);
          });

  // Activity = 1
  computeActivity(Event{10000});
  REQUIRE(std::abs(expectedActivities[0] - 1.) < 0.001);

  // Activity = 1.36787944117
  computeActivity(Event{40000});
  REQUIRE(std::abs(expectedActivities[1] - 1.36787944117) < 0.001);

  // Activity = 1.50321472441
  computeActivity(Event{70000});
  REQUIRE(std::abs(expectedActivities[2] - 1.50321472441) < 0.001);

  REQUIRE(activityEventGenerated);
}

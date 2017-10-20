#include "../source/computeActivity.hpp"

#include "catch.hpp"

#include <iostream>

struct Event {
  uint64_t timestamp;
} __attribute__((packed));

struct ActivityEvent {
  uint64_t timestamp;
  double activity;
};

const uint64_t lifespan = 30000;

int counter = 0;

TEST_CASE("Compute the event activity in a scene", "[ComputeActivity]") {
  auto activityEventGenerated = false;
  auto computeActivity =
      tarsier::make_computeActivity<Event, ActivityEvent, lifespan>(
          [](Event event, double activity) -> ActivityEvent {
            return ActivityEvent{event.timestamp, activity};
          },
          [&activityEventGenerated](ActivityEvent activityEvent) -> void {
            activityEventGenerated = true;
            // REQUIRE(std::abs(activityEvent.activity - 11.87109416558) <
            // 0.01);

            if (counter == 2) {
              REQUIRE(std::abs(activityEvent.activity - 1.87109416558) < 0.01);
            }
            counter++;
          });

  // Activity = 1
  computeActivity(Event{10000});
  // Activity = 1.36787944117
  computeActivity(Event{40000});
  // Activity = 1.87109416558
  computeActivity(Event{70000});
  REQUIRE(activityEventGenerated);
}

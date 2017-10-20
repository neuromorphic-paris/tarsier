#include "../source/computeActivity.hpp"

#include "catch.hpp"

struct Event {
  uint16_t x;
  uint16_t y;
  uint64_t timestamp;
} __attribute__((packed));

struct ActivityEvent {
  uint64_t x;
  uint64_t y;
  uint64_t timestamp;
  double activity;
};

const uint64_t lifespan = 30000;

TEST_CASE("Compute the event activity in a scene", "[ComputeActivity]") {
  auto activityEventGenerated = false;
  auto computeActivity =
      tarsier::make_computeActivity<Event, ActivityEvent, lifespan>(
          [](Event event, double activity) -> ActivityEvent {
            return ActivityEvent{event.x, event.y, event.timestamp, activity};
          },
          [&activityEventGenerated](ActivityEvent activityEvent) -> void {
            activityEventGenerated = true;
            // REQUIRE(std::abs(activityEvent.activity - 1.87109416558) < 0.01);
            REQUIRE(std::abs(activityEvent.activity - 1.87109416558) < 0.01);
          });
  computeActivity(Event{100, 200, 10000});
  computeActivity(Event{100, 100, 40000});
  // computeActivity(Event{100, 100, 70000});
  REQUIRE(activityEventGenerated);
}

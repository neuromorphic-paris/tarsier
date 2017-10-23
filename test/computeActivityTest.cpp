#include "../source/computeActivity.hpp"

#include "catch.hpp"

struct Event {
  uint64_t timestamp;
  bool polarity;
} __attribute__((packed));

struct ActivityEvent {
  uint64_t timestamp;
  double activity;
  double activityON;
  double activityOFF;
};

const uint64_t lifespan = 30000;

std::vector<double> expectedActivitiesON;
std::vector<double> expectedActivitiesOFF;

TEST_CASE("Compute the event activity in a scene", "[ComputeActivity]") {
  auto activityEventGenerated = false;
  auto computeActivity =
      tarsier::make_computeActivity<Event, ActivityEvent, lifespan>(
          [](Event event, double activity, double activityON,
             double activityOFF) -> ActivityEvent {
            return ActivityEvent{event.timestamp, activity, activityON,
                                 activityOFF};
          },
          [&activityEventGenerated](ActivityEvent activityEvent) -> void {
            activityEventGenerated = true;
            expectedActivitiesON.push_back(activityEvent.activityON);
            expectedActivitiesOFF.push_back(activityEvent.activityOFF);
          });

  // Activity = 1
  // ON - event
  computeActivity(Event{10000, true});
  REQUIRE(expectedActivitiesON[0] == 1);
  REQUIRE(expectedActivitiesOFF[0] == 0);

  // OFF - event
  computeActivity(Event{15000, false});
  REQUIRE(expectedActivitiesON[1] == 1);
  REQUIRE(expectedActivitiesOFF[1] == 1);

  // Activity = 1.36787944117
  computeActivity(Event{40000, true});
  REQUIRE(std::abs(expectedActivitiesON[2] - 1.36787944117) < 0.001);
  REQUIRE(expectedActivitiesOFF[2] == 1);

  computeActivity(Event{45000, false});
  REQUIRE(std::abs(expectedActivitiesON[3] - 1.36787944117) < 0.001);
  REQUIRE(std::abs(expectedActivitiesOFF[3] - 1.36787944117) < 0.001);

  // Activity = 1.50321472441
  computeActivity(Event{70000, true});
  REQUIRE(std::abs(expectedActivitiesON[4] - 1.50321472441) < 0.001);
  REQUIRE(std::abs(expectedActivitiesOFF[4] - 1.36787944117) < 0.001);

  computeActivity(Event{75000, false});
  REQUIRE(std::abs(expectedActivitiesON[5] - 1.50321472441) < 0.001);
  REQUIRE(std::abs(expectedActivitiesOFF[5] - 1.50321472441) < 0.001);

  REQUIRE(activityEventGenerated);
}

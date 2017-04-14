#include "../source/computeFlow.hpp"

#include "catch.hpp"

struct Event {
    uint16_t x;
    uint16_t y;
    uint64_t timestamp;
} __attribute__((packed));

struct FlowEvent {
    uint64_t x;
    uint64_t y;
    uint64_t timestamp;
    double vx;
    double vy;
};

TEST_CASE("Compute the optical flow from the given events", "[ComputeFlow]") {
    auto flowEventGenerated = false;
    auto computeFlow = tarsier::make_computeFlow<Event, FlowEvent, 304, 240, 2, 10, 1000000>(
        [](Event event, double vx, double vy) -> FlowEvent {
            return FlowEvent{event.x, event.y, event.timestamp, vx, vy};
        },
        [&flowEventGenerated](FlowEvent flowEvent) -> void {
            flowEventGenerated = true;
            REQUIRE(std::abs(flowEvent.vx - 0.000090472103004291901) < std::numeric_limits<double>::epsilon());
            REQUIRE(std::abs(flowEvent.vy - 0.00023201716738197433) < std::numeric_limits<double>::epsilon());
        }
    );
    computeFlow(Event{100 - 2, 100 - 2, 2000000});
    computeFlow(Event{100 - 1, 100 - 2, 2001000});
    computeFlow(Event{100 - 0, 100 - 2, 2002000});
    computeFlow(Event{100 - 2, 100 - 1, 2003000});
    computeFlow(Event{100 + 1, 100 - 2, 2004000});
    computeFlow(Event{100 - 1, 100 - 1, 2005000});
    computeFlow(Event{100 - 0, 100 - 1, 2006000});
    computeFlow(Event{100 - 2, 100 - 0, 2007000});
    computeFlow(Event{100 + 1, 100 - 1, 2008000});
    computeFlow(Event{100, 100, 2010000});
    REQUIRE(flowEventGenerated);
}

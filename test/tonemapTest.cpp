#include "../source/tonemap.hpp"

#include "catch.hpp"

struct HighDynamicRangeEvent {
    std::size_t x;
    std::size_t y;
    uint64_t timeDelta;
};

struct Event {
    std::size_t x;
    std::size_t y;
    float exposure;
};

TEST_CASE("Map tones with scale globally and fixed", "[MapTones]") {
    auto tonemap = tarsier::make_tonemap<HighDynamicRangeEvent, Event, 304, 240>(
        1.0,
        [](HighDynamicRangeEvent highDynamicRangeEvent, float exposure) -> Event {
            return Event{highDynamicRangeEvent.x, highDynamicRangeEvent.y, exposure};
        },
        [](Event event) -> void {}
    );
}

TEST_CASE("Map tones with scale globally and adaptive", "[MapTones]") {
    auto tonemap = tarsier::make_tonemap<HighDynamicRangeEvent, Event, 304, 240>(
        1.0,
        1.0,
        0.0646,
        0.0539,
        [](HighDynamicRangeEvent highDynamicRangeEvent, float exposure) -> Event {
            return Event{highDynamicRangeEvent.x, highDynamicRangeEvent.y, exposure};
        },
        [](Event event) -> void {}
    );
}

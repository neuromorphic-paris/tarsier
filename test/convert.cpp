#include "../source/convert.hpp"
#include "../third_party/Catch2/single_include/catch.hpp"

struct event {
    bool is_increase;
};

struct converted_event {
    bool polarity;
};

TEST_CASE("Convert an event type to another", "[convert]") {
    auto convert = tarsier::make_convert<event>(
        [](event event) -> converted_event { return {event.is_increase}; },
        [](converted_event converted_event) -> void { REQUIRE(converted_event.polarity); });
    convert(event{true});
}

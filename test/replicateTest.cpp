#include "../source/replicate.hpp"

#include "catch.hpp"

struct Event {};


TEST_CASE("Replicate an event and trigger several callbacks", "[Replicate]") {
    auto count = static_cast<std::size_t>(0);
    auto replicate = tarsier::make_replicate<Event>(
        [&count](Event) {
            ++count;
        },
        [&count](Event) {
            ++count;
        },
        [&count](Event) {
            ++count;
        }
    );
    replicate(Event{});
    REQUIRE(count == 3);
}

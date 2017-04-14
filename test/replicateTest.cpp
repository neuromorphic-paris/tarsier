#include "../source/replicate.hpp"

#include "catch.hpp"

struct Event {} __attribute__((packed));

TEST_CASE("Replicate an event and trigger several callbacks", "[Replicate]") {
    std::size_t count = 0;
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

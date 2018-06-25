#include "../source/replicate.hpp"
#include "../third_party/Catch2/single_include/catch.hpp"

struct event {};

TEST_CASE("Replicate an event and trigger several callbacks", "[replicate]") {
    std::size_t count = 0;
    auto replicate =
        tarsier::make_replicate<event>([&count](event) { ++count; }, [&](event) { ++count; }, [&](event) { ++count; });
    replicate(event{});
    REQUIRE(count == 3);
}

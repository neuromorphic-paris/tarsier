#include "../source/merge.hpp"
#include "../third_party/Catch2/single_include/catch.hpp"

struct event {
    uint64_t t;
};

TEST_CASE("Merge two streams", "[merge]") {
    std::size_t index = 0;
    auto merge = tarsier::make_merge<2, event>(256, std::chrono::milliseconds(20), [&](event event) -> void {
        REQUIRE(event.t == index);
        ++index;
    });
    merge->push<0>(event{1});
    merge->push<1>(event{0});
}

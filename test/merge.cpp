#include "../source/merge.hpp"
#include "../third_party/Catch2/single_include/catch.hpp"

struct event {
    uint64_t t;
};

TEST_CASE("Merge two streams", "[merge]") {
    auto merge = tarsier::make_merge<2, event>([](event event) -> void { REQUIRE(event.t == 0); });
    merge->push<0>(event{1});
    merge->push<1>(event{0});
}

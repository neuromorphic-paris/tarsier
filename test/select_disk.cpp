#include "../source/select_disk.hpp"
#include "../third_party/Catch2/single_include/catch.hpp"

struct event {
    uint16_t x;
    uint16_t y;
};

TEST_CASE("Filter out events outside the disk", "[select_disk]") {
    auto select_disk =
        tarsier::make_select_disk<event>(100, 100, 20.0, [](event event) -> void { REQUIRE(event.x == 100); });
    select_disk(event{200, 200});
    select_disk(event{100, 110});
}

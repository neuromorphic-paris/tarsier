#include "../source/select_rectangle.hpp"
#include "../third_party/Catch2/single_include/catch.hpp"

struct event {
    uint16_t x;
    uint16_t y;
};

TEST_CASE("Filter out events outside the rectangle", "[select_rectangle]") {
    auto select_rectangle =
        tarsier::make_select_rectangle<event>(50, 50, 204, 140, [](event event) -> void { REQUIRE(event.x == 100); });
    select_rectangle(event{300, 200});
    select_rectangle(event{100, 100});
}

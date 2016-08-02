#include "../source/selectRectangle.hpp"

#include "catch.hpp"

struct Event {
    std::size_t x;
    std::size_t y;
};

TEST_CASE("Filter out events outside the rectangle", "[SelectRectangle]") {
    auto selectRectangle = tarsier::make_selectRectangle<Event>(20, 20, 100, 100, [](Event event) -> void {
        REQUIRE(event.x == 100);
    });
    selectRectangle(Event{200, 200});
    selectRectangle(Event{100, 100});
}

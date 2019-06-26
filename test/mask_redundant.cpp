#include "../source/mask_redundant.hpp"
#include "../third_party/Catch2/single_include/catch.hpp"

struct event {
    uint16_t x;
    uint16_t y;
    uint64_t t;
    bool is_increase;
};

TEST_CASE("Filter successive similar events close in time", "[mask_redundant]") {
    uint16_t n_event_output = 0;
    auto mask_redundant = tarsier::make_mask_redundant<event>(320, 240, 50, [&](event event) -> void {
        switch (n_event_output) {
            case 0:
                REQUIRE(event.t == 100);
                break;
            case 1:
                REQUIRE(event.t == 140);
                break;
            case 2:
                REQUIRE(event.t == 200);
                break;
            case 3:
                REQUIRE(event.t == 210);
                break;
            default:
                throw std::logic_error("Unexpected value");
        }
        n_event_output++;
    });
    mask_redundant(event{101, 51, 100, true});
    mask_redundant(event{101, 51, 120, true});
    mask_redundant(event{101, 52, 140, true});
    mask_redundant(event{201, 21, 200, true});
    mask_redundant(event{201, 21, 210, false});
    REQUIRE(n_event_output == 4);
}

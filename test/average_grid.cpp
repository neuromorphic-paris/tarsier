#include "../source/average_grid.hpp"
#include "../third_party/Catch2/single_include/catch.hpp"
#include <vector>

struct event {
    uint16_t x;
    uint16_t y;
};

struct Position {
    float cx;
    float cy;
    bool valid;
};

typedef std::vector<std::vector<Position>> Grid;

struct Centroids {
    std::vector<std::vector<Position>> grid;
    uint16_t ir;
    uint16_t ic;
};


Grid grid = {{{1, 1, true}, {4, 1, true}, {7, 1, true}},
             {{1, 4, true}, {4, 4, true}, {7, 4, true}},
             {{1, 7, true}, {4, 7, true}, {7, 7, false}}};

TEST_CASE("Average the position of the given events in a grid", "[average_grid]") {
    auto first_received = false;
    auto second_received = false;
    auto third_received = false;
    auto average_grid = tarsier::make_average_grid<event, Grid>(
        grid,
        3.0,
        0.5,
        [](event event, Grid grid, uint16_t ir, uint16_t ic) -> Centroids {
            return {grid, ir, ic};
        },
        [&](Centroids centroids) -> void {
            if (third_received) {
                REQUIRE(centroids.grid[centroids.ir][centroids.ic].cx == 7);
                REQUIRE(centroids.grid[centroids.ir][centroids.ic].cy == 7);
            } else if (second_received) {
                REQUIRE(centroids.grid[centroids.ir][centroids.ic].cx == 4.5);
                REQUIRE(centroids.grid[centroids.ir][centroids.ic].cy == 1.5);
                third_received = true;
            } else if (first_received) {
                REQUIRE(centroids.grid[centroids.ir][centroids.ic].cx == 1.25);
                REQUIRE(centroids.grid[centroids.ir][centroids.ic].cy == 1.25);
                second_received = true;
            } else {
                first_received = true;
            }
        });
    average_grid(event{0, 0});
    average_grid(event{2, 2});
    average_grid(event{5, 2});
    average_grid(event{8, 8});
}

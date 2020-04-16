#pragma once

#include <stdexcept>
#include <utility>
#include <vector>
#include <cmath>

/// tarsier is a collection of event handlers.
namespace tarsier {
    /// average_grid calculates the average positions of the given events within each grid.
    /// An exponential event-wise decay is used as weight.
    template <typename Event, typename Grid, typename EventToGrid, typename HandleGrid>
    class average_grid {
        public:
        average_grid(
            Grid grid,
            float pitch,
            float inertia,
            EventToGrid&& event_to_grid,
            HandleGrid&& handle_grid) :
            _grid(grid),
            _pitch(pitch),
            _inertia(inertia),
            _event_to_grid(std::forward<EventToGrid>(event_to_grid)),
            _handle_grid(std::forward<HandleGrid>(handle_grid)) {
            if (_inertia < 0 || _inertia > 1) {
                throw std::logic_error("inertia must be in the range [0, 1]");
            }
        }
        average_grid(const average_grid&) = delete;
        average_grid(average_grid&&) = default;
        average_grid& operator=(const average_grid&) = delete;
        average_grid& operator=(average_grid&&) = default;
        virtual ~average_grid() {}

        /// operator() handles an event.
        virtual void operator()(Event event) {
            const uint16_t ic = std::floor(event.x / _pitch);
            const uint16_t ir = std::floor(event.y / _pitch);
            if (_grid[ir][ic].valid) {
                _grid[ir][ic].cx = _inertia * _grid[ir][ic].cx + (1 - _inertia) * event.x;
                _grid[ir][ic].cy = _inertia * _grid[ir][ic].cy + (1 - _inertia) * event.y;
            }
            _handle_grid(_event_to_grid(event, _grid, ir, ic));
        }

        protected:
        Grid _grid;
        const float _pitch;
        const float _inertia;
        EventToGrid _event_to_grid;
        HandleGrid _handle_grid;
    };

    /// make_average_grid creates an average_grid from functors.
    template <typename Event, typename Grid, typename EventToGrid, typename HandleGrid>
    inline average_grid<Event, Grid, EventToGrid, HandleGrid> make_average_grid(
        Grid grid,
        float pitch,
        float inertia,
        EventToGrid&& event_to_grid,
        HandleGrid&& handle_grid) {
        return average_grid<Event, Grid, EventToGrid, HandleGrid>(
            grid,
            pitch,
            inertia,
            std::forward<EventToGrid>(event_to_grid),
            std::forward<HandleGrid>(handle_grid));
    }
}

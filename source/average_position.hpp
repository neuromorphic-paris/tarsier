#pragma once

#include <stdexcept>
#include <utility>

/// tarsier is a collection of event handlers.
namespace tarsier {
    /// average_position calculates the average position of the given events.
    /// An exponential event-wise decay is used as weight.
    template <typename Event, typename Position, typename EventToPosition, typename HandlePosition>
    class average_position {
        public:
        average_position(
            double x,
            double y,
            double inertia,
            EventToPosition EventToposition,
            HandlePosition handle_position) :
            _x(x),
            _y(y),
            _inertia(inertia),
            _EventToposition(std::forward<EventToPosition>(EventToposition)),
            _handle_position(std::forward<HandlePosition>(handle_position)) {
            if (_inertia < 0 || _inertia > 1) {
                throw std::logic_error("inertia must be in the range [0, 1]");
            }
        }
        average_position(const average_position&) = delete;
        average_position(average_position&&) = default;
        average_position& operator=(const average_position&) = delete;
        average_position& operator=(average_position&&) = default;
        virtual ~average_position() {}

        /// operator() handles an event.
        virtual void operator()(Event event) {
            _x = _inertia * _x + (1 - _inertia) * event.x;
            _y = _inertia * _y + (1 - _inertia) * event.y;
            _handle_position(_EventToposition(event, _x, _y));
        }

        protected:
        double _x;
        double _y;
        const double _inertia;
        EventToPosition _EventToposition;
        HandlePosition _handle_position;
    };

    /// make_average_position creates an average_position from functors.
    template <typename Event, typename Position, typename EventToPosition, typename HandlePosition>
    average_position<Event, Position, EventToPosition, HandlePosition> make_average_position(
        double x,
        double y,
        double inertia,
        EventToPosition EventToposition,
        HandlePosition handle_position) {
        return average_position<Event, Position, EventToPosition, HandlePosition>(
            x,
            y,
            inertia,
            std::forward<EventToPosition>(EventToposition),
            std::forward<HandlePosition>(handle_position));
    }
}

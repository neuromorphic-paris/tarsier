#pragma once

#include <stdexcept>

/// tarsier is a collection of event handlers.
namespace tarsier {

    /// Position represents a position in pixel coordinates.
    struct Position {

        /// x represents the coordinate of the flow on the sensor grid alongside the horizontal axis.
        /// x is 0 on the left, and increases from left to right.
        double x;

        /// y represents the coordinate of the flow on the sensor grid alongside the vertical axis.
        /// y is 0 on the bottom, and increases bottom to top.
        double y;
    };

    /// AveragePosition calculates the average position of the given events.
    /// An exponential event-wise decay is used as weight.
    template <typename Event, typename HandlePosition>
    class AveragePosition {
        public:
            AveragePosition(const double& inertia, const std::size_t& eventsToReceive, HandlePosition handlePosition) :
                _inertia(inertia),
                _eventsToReceive(eventsToReceive),
                _handlePosition(std::forward<HandlePosition>(handlePosition))
            {
                if (_inertia < 0 || _inertia > 1) {
                    throw std::logic_error("inertia must be in the range [0, 1]");
                }
            }
            AveragePosition(const AveragePosition&) = delete;
            AveragePosition(AveragePosition&&) = default;
            AveragePosition& operator=(const AveragePosition&) = delete;
            AveragePosition& operator=(AveragePosition&&) = default;
            virtual ~AveragePosition() {}

            /// operator() handles an event.
            virtual void operator()(Event event) {
                _position.x = _inertia * _position.x + (1 - _inertia) * event.x;
                _position.y = _inertia * _position.y + (1 - _inertia) * event.y;
                if (_eventsToReceive == 0) {
                    _handlePosition(_position);
                } else {
                    --_eventsToReceive;
                }
            }

        protected:
            const double _inertia;
            std::size_t _eventsToReceive;
            HandlePosition _handlePosition;
            Position _position;
    };

    /// make_averagePosition creates an AveragePosition from a functor.
    template<typename Event, typename HandlePosition>
    AveragePosition<Event, HandlePosition> make_averagePosition(
        const double& inertia,
        const std::size_t& eventsToReceive,
        HandlePosition handlePosition
    ) {
        return AveragePosition<Event, HandlePosition>(inertia, eventsToReceive, std::forward<HandlePosition>(handlePosition));
    }
}

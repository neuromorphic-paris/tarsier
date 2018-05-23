#pragma once

#include <cmath>
#include <utility>

/// tarsier is a collection of event handlers.
namespace tarsier {
    /// select_disk propagates only the events within the given disk.
    template <typename Event, typename HandleEvent>
    class select_disk {
        public:
        select_disk(float x, float y, float radius, HandleEvent handle_event) :
            _x(x),
            _y(y),
            _squared_radius(std::pow(radius, 2)),
            _handle_event(std::forward<HandleEvent>(handle_event)) {}
        select_disk(const select_disk&) = delete;
        select_disk(select_disk&&) = default;
        select_disk& operator=(const select_disk&) = delete;
        select_disk& operator=(select_disk&&) = default;
        virtual ~select_disk() {}

        /// operator() handles an event.
        virtual void operator()(Event event) {
            if (std::pow(event.x - _x, 2) + std::pow(event.y - _y, 2) < _squared_radius) {
                _handle_event(event);
            }
        }

        protected:
        const float _x;
        const float _y;
        const float _squared_radius;
        HandleEvent _handle_event;
    };

    /// make_select_disk creates a select_disk from a functor.
    template <typename Event, typename HandleEvent>
    select_disk<Event, HandleEvent> make_select_disk(float x, float y, float radius, HandleEvent handle_event) {
        return select_disk<Event, HandleEvent>(x, y, radius, std::forward<HandleEvent>(handle_event));
    }
}

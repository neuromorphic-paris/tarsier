#pragma once

#include <cstdint>
#include <utility>

/// tarsier is a collection of event handlers.
namespace tarsier {

    /// mirror_x inverts the x coordinate.
    template <typename Event, typename HandleEvent>
    class mirror_x {
        public:
        mirror_x(uint16_t width, HandleEvent handle_event) :
            _width(width),
            _handle_event(std::forward<HandleEvent>(handle_event)) {}
        mirror_x(const mirror_x&) = delete;
        mirror_x(mirror_x&&) = default;
        mirror_x& operator=(const mirror_x&) = delete;
        mirror_x& operator=(mirror_x&&) = default;
        virtual ~mirror_x() {}

        /// operator() handles an event.
        virtual void operator()(Event event) {
            event.x = _width - 1 - event.x;
            _handle_event(event);
        }

        protected:
        const uint16_t _width;
        HandleEvent _handle_event;
    };

    /// make_mirror_x creates a mirror_x from a functor.
    template <typename Event, typename HandleEvent>
    inline mirror_x<Event, HandleEvent> make_mirror_x(uint16_t width, HandleEvent handle_event) {
        return mirror_x<Event, HandleEvent>(width, std::forward<HandleEvent>(handle_event));
    }
}

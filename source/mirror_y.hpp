#pragma once

#include <cstdint>
#include <utility>

/// tarsier is a collection of event handlers.
namespace tarsier {

    /// mirror_y inverts the y coordinate.
    template <typename Event, typename HandleEvent>
    class mirror_y {
        public:
        mirror_y(uint16_t height, HandleEvent handle_event) :
            _height(height),
            _handle_event(std::forward<HandleEvent>(handle_event)) {}
        mirror_y(const mirror_y&) = delete;
        mirror_y(mirror_y&&) = default;
        mirror_y& operator=(const mirror_y&) = delete;
        mirror_y& operator=(mirror_y&&) = default;
        virtual ~mirror_y() {}

        /// operator() handles an event.
        virtual void operator()(Event event) {
            event.y = _height - 1 - event.y;
            _handle_event(event);
        }

        protected:
        const uint16_t _height;
        HandleEvent _handle_event;
    };

    /// make_mirror_y creates a mirror_y from a functor.
    template <typename Event, typename HandleEvent>
    inline mirror_y<Event, HandleEvent> make_mirror_y(uint16_t height, HandleEvent handle_event) {
        return mirror_y<Event, HandleEvent>(height, std::forward<HandleEvent>(handle_event));
    }
}

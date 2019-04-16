#pragma once

#include <cstdint>
#include <utility>

/// tarsier is a collection of event handlers.
namespace tarsier {

    /// select_rectangle propagates only the events within the given rectangular window.
    template <typename Event, typename HandleEvent>
    class select_rectangle {
        public:
        select_rectangle(uint16_t left, uint16_t bottom, uint16_t width, uint16_t height, HandleEvent handle_event) :
            _left(left),
            _bottom(bottom),
            _right(left + width),
            _top(bottom + height),
            _handle_event(std::forward<HandleEvent>(handle_event)) {}
        select_rectangle(const select_rectangle&) = delete;
        select_rectangle(select_rectangle&&) = default;
        select_rectangle& operator=(const select_rectangle&) = delete;
        select_rectangle& operator=(select_rectangle&&) = default;
        virtual ~select_rectangle() {}

        /// operator() handles an event.
        virtual void operator()(Event event) {
            if (event.x >= _left && event.x < _right && event.y >= _bottom && event.y < _top) {
                _handle_event(event);
            }
        }

        protected:
        const uint16_t _left;
        const uint16_t _bottom;
        const uint16_t _right;
        const uint16_t _top;
        HandleEvent _handle_event;
    };

    /// make_select_rectangle creates a select_rectangle from a functor.
    template <typename Event, typename HandleEvent>
    select_rectangle<Event, HandleEvent> inline make_select_rectangle(
        uint16_t left,
        uint16_t bottom,
        uint16_t width,
        uint16_t height,
        HandleEvent handle_event) {
        return select_rectangle<Event, HandleEvent>(
            left, bottom, width, height, std::forward<HandleEvent>(handle_event));
    }
}

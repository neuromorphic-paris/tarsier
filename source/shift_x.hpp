#pragma once

#include <cstdint>
#include <utility>

/// tarsier is a collection of event handlers.
namespace tarsier {

    /// shift_x translates the x coordinate.
    template <typename Event, typename HandleEvent>
    class shift_x {
        public:
        shift_x(uint16_t width, int32_t shift, HandleEvent handle_event) :
            _width(width),
            _shift(shift),
            _handle_event(std::forward<HandleEvent>(handle_event)) {}
        shift_x(const shift_x&) = delete;
        shift_x(shift_x&&) = default;
        shift_x& operator=(const shift_x&) = delete;
        shift_x& operator=(shift_x&&) = default;
        virtual ~shift_x() {}

        /// operator() handles an event.
        virtual void operator()(Event event) {
            const auto shifted = static_cast<int32_t>(event.x) + _shift;
            if (shifted >= 0 && shifted < _width) {
                event.x = shifted;
                _handle_event(event);
            }
        }

        protected:
        const uint16_t _width;
        const int32_t _shift;
        HandleEvent _handle_event;
    };

    /// make_shift_x creates a shift_x from a functor.
    template <typename Event, typename HandleEvent>
    inline shift_x<Event, HandleEvent> make_shift_x(uint16_t width, int32_t shift, HandleEvent handle_event) {
        return shift_x<Event, HandleEvent>(width, shift, std::forward<HandleEvent>(handle_event));
    }
}

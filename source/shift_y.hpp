#pragma once

#include <cstdint>
#include <utility>

/// tarsier is a collection of event handlers.
namespace tarsier {

    /// shift_y translates the x coordinate.
    template <typename Event, typename HandleEvent>
    class shift_y {
        public:
        shift_y(uint16_t height, int32_t shift, HandleEvent handle_event) :
            _height(height),
            _shift(shift),
            _handle_event(std::forward<HandleEvent>(handle_event)) {}
        shift_y(const shift_y&) = delete;
        shift_y(shift_y&&) = default;
        shift_y& operator=(const shift_y&) = delete;
        shift_y& operator=(shift_y&&) = default;
        virtual ~shift_y() {}

        /// operator() handles an event.
        virtual void operator()(Event event) {
            const auto shifted = static_cast<int32_t>(event.y) + _shift;
            if (shifted >= 0 && shifted < _height) {
                event.y = shifted;
                _handle_event(event);
            }
        }

        protected:
        const uint16_t _height;
        const int32_t _shift;
        HandleEvent _handle_event;
    };

    /// make_shift_y creates a shift_y from a functor.
    template <typename Event, typename HandleEvent>
    inline shift_y<Event, HandleEvent> make_shift_y(uint16_t height, int32_t shift, HandleEvent handle_event) {
        return shift_y<Event, HandleEvent>(height, shift, std::forward<HandleEvent>(handle_event));
    }
}

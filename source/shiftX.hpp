#pragma once

#include <cstdint>
#include <utility>

/// tarsier is a collection of event handlers.
namespace tarsier {

    /// ShiftX translates the x coordinate.
    template <typename Event, typename HandleEvent>
    class ShiftX {
        public:
            ShiftX(std::size_t width, int64_t shift, HandleEvent handleEvent) :
                _width(width),
                _shift(shift),
                _handleEvent(std::forward<HandleEvent>(handleEvent))
            {
            }
            ShiftX(const ShiftX&) = delete;
            ShiftX(ShiftX&&) = default;
            ShiftX& operator=(const ShiftX&) = delete;
            ShiftX& operator=(ShiftX&&) = default;
            virtual ~ShiftX() {}

            /// operator() handles an event.
            virtual void operator()(Event event) {
                const auto shifted = static_cast<int64_t>(event.x) + _shift;
                if (shifted >= 0 && static_cast<std::size_t>(shifted) < _width) {
                    event.x = shifted;
                    _handleEvent(event);
                }
            }

        protected:
            std::size_t _width;
            int64_t _shift;
            HandleEvent _handleEvent;
    };

    /// make_shiftX creates a ShiftX from a functor.
    template<typename Event, typename HandleEvent>
    ShiftX<Event, HandleEvent> make_shiftX(std::size_t width, int64_t shift, HandleEvent handleEvent) {
        return ShiftX<Event, HandleEvent>(width, shift, std::forward<HandleEvent>(handleEvent));
    }
}

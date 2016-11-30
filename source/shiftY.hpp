#pragma once

#include <cstdint>
#include <utility>

/// tarsier is a collection of event handlers.
namespace tarsier {

    /// ShiftY translates the y coordinate.
    template <typename Event, typename HandleEvent>
    class ShiftY {
        public:
            ShiftY(std::size_t height, int64_t shift, HandleEvent handleEvent) :
                _height(height),
                _shift(shift),
                _handleEvent(std::forward<HandleEvent>(handleEvent))
            {
            }
            ShiftY(const ShiftY&) = delete;
            ShiftY(ShiftY&&) = default;
            ShiftY& operator=(const ShiftY&) = delete;
            ShiftY& operator=(ShiftY&&) = default;
            virtual ~ShiftY() {}

            /// operator() handles an event.
            virtual void operator()(Event event) {
                const auto shifted = static_cast<int64_t>(event.y) + _shift;
                if (shifted >= 0 && static_cast<std::size_t>(shifted) < _height) {
                    event.y = shifted;
                    _handleEvent(std::move(event));
                }
            }

        protected:
            const std::size_t _height;
            const int64_t _shift;
            HandleEvent _handleEvent;
    };

    /// make_shiftY creates a ShiftY from a functor.
    template<typename Event, typename HandleEvent>
    ShiftY<Event, HandleEvent> make_shiftY(std::size_t height, int64_t shift, HandleEvent handleEvent) {
        return ShiftY<Event, HandleEvent>(height, shift, std::forward<HandleEvent>(handleEvent));
    }
}

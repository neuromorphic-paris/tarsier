#pragma once

#include <cstdint>
#include <utility>

/// tarsier is a collection of event handlers.
namespace tarsier {

    /// ShiftX translates the x coordinate.
    template <typename Event, uint_fast16_t width, int32_t shift, typename HandleEvent>
    class ShiftX {
        public:
            ShiftX(HandleEvent handleEvent) :
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
                const auto shifted = static_cast<int32_t>(event.x) + shift;
                if (shifted >= 0 && static_cast<uint_fast16_t>(shifted) < width) {
                    event.x = shifted;
                    _handleEvent(std::move(event));
                }
            }

        protected:
            HandleEvent _handleEvent;
    };

    /// make_shiftX creates a ShiftX from a functor.
    template<typename Event, uint_fast16_t width, int64_t shift, typename HandleEvent>
    ShiftX<Event, width, shift, HandleEvent> make_shiftX(HandleEvent handleEvent) {
        return ShiftX<Event, width, shift, HandleEvent>(std::forward<HandleEvent>(handleEvent));
    }
}

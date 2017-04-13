#pragma once

#include <cstdint>
#include <utility>

/// tarsier is a collection of event handlers.
namespace tarsier {

    /// ShiftY translates the y coordinate.
    template <typename Event, uint64_t height, int64_t shift, typename HandleEvent>
    class ShiftY {
        public:
            ShiftY(HandleEvent handleEvent) :
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
                const auto shifted = static_cast<int64_t>(event.y) + shift;
                if (shifted >= 0 && static_cast<uint64_t>(shifted) < height) {
                    event.y = shifted;
                    _handleEvent(std::move(event));
                }
            }

        protected:
            HandleEvent _handleEvent;
    };

    /// make_shiftY creates a ShiftY from a functor.
    template<typename Event, uint64_t height, int64_t shift, typename HandleEvent>
    ShiftY<Event, height, shift, HandleEvent> make_shiftY(HandleEvent handleEvent) {
        return ShiftY<Event, height, shift, HandleEvent>(std::forward<HandleEvent>(handleEvent));
    }
}

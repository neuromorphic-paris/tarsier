#pragma once

#include <cstdint>
#include <utility>
#include <array>
#include <limits>

/// tarsier is a collection of event handlers.
namespace tarsier {

    /// MaskIsolated propagates only events that are not isolated spatially or in time.
    template <typename Event, std::size_t width, std::size_t height, uint64_t decay, typename HandleEvent>
    class MaskIsolated {
        public:
            MaskIsolated(HandleEvent handleEvent) :
                _handleEvent(std::forward<HandleEvent>(handleEvent))
            {
            }
            MaskIsolated(const MaskIsolated&) = delete;
            MaskIsolated(MaskIsolated&&) = default;
            MaskIsolated& operator=(const MaskIsolated&) = delete;
            MaskIsolated& operator=(MaskIsolated&&) = default;
            virtual ~MaskIsolated() {}

            /// operator() handles an event.
            virtual void operator()(Event event) {
                const auto index = event.x + event.y * width;
                _timestamps[index] = event.timestamp + decay;
                if (
                    (event.x > 0 && _timestamps[index - 1] > event.timestamp)
                    || (event.x < width - 1 && _timestamps[index + 1] > event.timestamp)
                    || (event.y > 0 && _timestamps[index - width] > event.timestamp)
                    || (event.y < height - 1 && _timestamps[index + width] > event.timestamp)
                ) {
                    _handleEvent(std::move(event));
                }
            }

        protected:
            HandleEvent _handleEvent;
            std::array<uint64_t, width * height> _timestamps;
    };

    /// make_maskIsolated creates a MaskIsolated from a functor.
    template<typename Event, std::size_t width, std::size_t height, uint64_t decay, typename HandleEvent>
    MaskIsolated<Event, width, height, decay, HandleEvent> make_maskIsolated(HandleEvent handleEvent) {
        return MaskIsolated<Event, width, height, decay, HandleEvent>(std::forward<HandleEvent>(handleEvent));
    }
}

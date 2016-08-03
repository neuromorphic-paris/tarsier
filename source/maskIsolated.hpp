#pragma once

#include <cstdint>
#include <utility>
#include <vector>
#include <limits>

/// tarsier is a collection of event handlers.
namespace tarsier {

    /// MaskIsolated propagates only events that are not isolated spatially or in time.
    template <typename Event, typename HandleEvent>
    class MaskIsolated {
        public:
            MaskIsolated(std::size_t width, std::size_t height, uint64_t decay, HandleEvent handleEvent) :
                _width(width),
                _height(height),
                _decay(decay),
                _handleEvent(std::forward<HandleEvent>(handleEvent)),
                _timestamps(width * height, std::numeric_limits<int64_t>::lowest())
            {
            }
            MaskIsolated(const MaskIsolated&) = delete;
            MaskIsolated(MaskIsolated&&) = default;
            MaskIsolated& operator=(const MaskIsolated&) = delete;
            MaskIsolated& operator=(MaskIsolated&&) = default;
            virtual ~MaskIsolated() {}

            /// operator() handles an event.
            virtual void operator()(Event event) {
                const auto index = event.x + event.y * _width;
                _timestamps[index] = event.timestamp + _decay;
                if (event.x > 0 && _timestamps[index - 1] > event.timestamp) {
                    _handleEvent(event);
                    return;
                }
                if (event.x < _width - 1 && _timestamps[index + 1] > event.timestamp) {
                    _handleEvent(event);
                    return;
                }
                if (event.y > 0 && _timestamps[index - _width] > event.timestamp) {
                    _handleEvent(event);
                    return;
                }
                if (event.y < _height - 1 && _timestamps[index + _width] > event.timestamp) {
                    _handleEvent(event);
                    return;
                }
            }

        protected:
            const std::size_t _width;
            const std::size_t _height;
            const uint64_t _decay;
            HandleEvent _handleEvent;
            std::vector<int64_t> _timestamps;
    };

    /// make_maskIsolated creates a MaskIsolated from a functor.
    template<typename Event, typename HandleEvent>
    MaskIsolated<Event, HandleEvent> make_maskIsolated(std::size_t width, std::size_t height, uint64_t decay, HandleEvent handleEvent) {
        return MaskIsolated<Event, HandleEvent>(width, height, decay, std::forward<HandleEvent>(handleEvent));
    }
}

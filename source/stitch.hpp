#pragma once

#include <cstdint>
#include <utility>
#include <vector>

/// tarsier is a collection of event handlers.
namespace tarsier {

    /// Stitch turns a stream of threshold crossings into a stream of time differences.
    /// EventFromThresholdCrossing must have the following signature:
    ///     eventFromThresholdCrossing(const ThresholdCrossing& secondThresholdCrossing, uint64_t timeDelta) -> Event
    template <typename ThresholdCrossing, typename Event, typename EventFromThresholdCrossing, typename HandleEvent>
    class Stitch {
        public:
            Stitch(std::pair<std::size_t, std::size_t> size, EventFromThresholdCrossing eventFromThresholdCrossing, HandleEvent handleEvent) :
                _width(size.first),
                _eventFromThresholdCrossing(std::forward<EventFromThresholdCrossing>(eventFromThresholdCrossing)),
                _handleEvent(std::forward<HandleEvent>(handleEvent)),
                _areTriggeredAndTimestamps(size.first * size.second)
            {
            }
            Stitch(const Stitch&) = delete;
            Stitch(Stitch&&) = default;
            Stitch& operator=(const Stitch&) = delete;
            Stitch& operator=(Stitch&&) = default;
            virtual ~Stitch() {}

            /// operator() handles a threshold crossing.
            virtual void operator()(ThresholdCrossing thresholdCrossing) {
                auto& isTriggeredAndTimestamp = _areTriggeredAndTimestamps[thresholdCrossing.x + thresholdCrossing.y * _width];
                if (!isTriggeredAndTimestamp.first) {
                    if (!thresholdCrossing.isSecond) {
                        isTriggeredAndTimestamp.first = true;
                        isTriggeredAndTimestamp.second = thresholdCrossing.timestamp;
                    }
                } else {
                    if (thresholdCrossing.isSecond) {
                        isTriggeredAndTimestamp.first = false;
                        _handleEvent(_eventFromThresholdCrossing(
                            thresholdCrossing,
                            static_cast<uint64_t>(thresholdCrossing.timestamp - isTriggeredAndTimestamp.second)
                        ));
                    } else {
                        isTriggeredAndTimestamp.second = thresholdCrossing.timestamp;
                    }
                }
            }

        protected:
            std::size_t _width;
            EventFromThresholdCrossing _eventFromThresholdCrossing;
            HandleEvent _handleEvent;
            std::vector<std::pair<bool, int64_t>> _areTriggeredAndTimestamps;
    };

    /// make_stitch creates a Stitch from functors.
    template <typename ThresholdCrossing, typename Event, typename EventFromThresholdCrossing, typename HandleEvent>
    Stitch<ThresholdCrossing, Event, EventFromThresholdCrossing, HandleEvent> make_stitch(
        std::pair<std::size_t, std::size_t> size,
        EventFromThresholdCrossing eventFromThresholdCrossing,
        HandleEvent handleEvent
    ) {
        return Stitch<ThresholdCrossing, Event, EventFromThresholdCrossing, HandleEvent>(
            std::move(size),
            std::forward<EventFromThresholdCrossing>(eventFromThresholdCrossing),
            std::forward<HandleEvent>(handleEvent)
        );
    }
}

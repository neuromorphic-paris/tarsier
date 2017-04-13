#pragma once

#include <cstdint>
#include <utility>
#include <vector>

/// tarsier is a collection of event handlers.
namespace tarsier {

    /// Stitch turns a stream of threshold crossings into a stream of time differences.
    /// EventFromThresholdCrossing must have the following signature:
    ///     eventFromThresholdCrossing(const ThresholdCrossing& secondThresholdCrossing, const uint_fast64_t& timeDelta) -> Event
    template <typename ThresholdCrossing, typename Event, uint_fast16_t width, uint_fast16_t height, typename EventFromThresholdCrossing, typename HandleEvent>
    class Stitch {
        public:
            Stitch(EventFromThresholdCrossing eventFromThresholdCrossing, HandleEvent handleEvent) :
                _eventFromThresholdCrossing(std::forward<EventFromThresholdCrossing>(eventFromThresholdCrossing)),
                _handleEvent(std::forward<HandleEvent>(handleEvent)),
                _areTriggeredAndTimestamps(width * height, {false, 0})
            {
            }
            Stitch(const Stitch&) = delete;
            Stitch(Stitch&&) = default;
            Stitch& operator=(const Stitch&) = delete;
            Stitch& operator=(Stitch&&) = default;
            virtual ~Stitch() {}

            /// operator() handles a threshold crossing.
            virtual void operator()(const ThresholdCrossing& thresholdCrossing) {
                auto& isTriggeredAndTimestamp = _areTriggeredAndTimestamps[thresholdCrossing.x + thresholdCrossing.y * width];
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
                            static_cast<uint_fast64_t>(thresholdCrossing.timestamp) - isTriggeredAndTimestamp.second
                        ));
                    } else {
                        isTriggeredAndTimestamp.second = thresholdCrossing.timestamp;
                    }
                }
            }

        protected:
            EventFromThresholdCrossing _eventFromThresholdCrossing;
            HandleEvent _handleEvent;
            std::vector<std::pair<bool, uint_fast64_t>> _areTriggeredAndTimestamps;
    };

    /// make_stitch creates a Stitch from functors.
    template <typename ThresholdCrossing, typename Event, uint_fast16_t width, uint_fast16_t height, typename EventFromThresholdCrossing, typename HandleEvent>
    Stitch<ThresholdCrossing, Event, width, height, EventFromThresholdCrossing, HandleEvent> make_stitch(
        EventFromThresholdCrossing eventFromThresholdCrossing,
        HandleEvent handleEvent
    ) {
        return Stitch<ThresholdCrossing, Event, width, height, EventFromThresholdCrossing, HandleEvent>(
            std::forward<EventFromThresholdCrossing>(eventFromThresholdCrossing),
            std::forward<HandleEvent>(handleEvent)
        );
    }
}

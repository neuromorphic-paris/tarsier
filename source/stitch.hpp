#pragma once

#include <cstdint>
#include <utility>
#include <vector>

/// tarsier is a collection of event handlers.
namespace tarsier {

    /// stitch turns a stream of threshold crossings into a stream of time deltas.
    template <typename ThresholdCrossing, typename Event, typename ThresholdCrossingToEvent, typename HandleEvent>
    class stitch {
        public:
        stitch(
            uint16_t width,
            uint16_t height,
            ThresholdCrossingToEvent threshold_crossing_to_event,
            HandleEvent handle_event) :
            _width(width),
            _height(height),
            _threshold_crossing_to_event(std::forward<ThresholdCrossingToEvent>(threshold_crossing_to_event)),
            _handle_event(std::forward<HandleEvent>(handle_event)),
            _are_triggered_and_ts(width * height, {false, 0}) {}
        stitch(const stitch&) = delete;
        stitch(stitch&&) = default;
        stitch& operator=(const stitch&) = delete;
        stitch& operator=(stitch&&) = default;
        virtual ~stitch() {}

        /// operator() handles a threshold crossing.
        virtual void operator()(ThresholdCrossing threshold_crossing) {
            auto& is_triggered_and_t = _are_triggered_and_ts[threshold_crossing.x + threshold_crossing.y * _width];
            if (!is_triggered_and_t.first) {
                if (!threshold_crossing.is_second) {
                    is_triggered_and_t.first = true;
                    is_triggered_and_t.second = threshold_crossing.t;
                }
            } else {
                if (threshold_crossing.is_second) {
                    is_triggered_and_t.first = false;
                    _handle_event(_threshold_crossing_to_event(
                        threshold_crossing, threshold_crossing.t - is_triggered_and_t.second));
                } else {
                    is_triggered_and_t.second = threshold_crossing.t;
                }
            }
        }

        protected:
        const uint16_t _width;
        const uint16_t _height;
        ThresholdCrossingToEvent _threshold_crossing_to_event;
        HandleEvent _handle_event;
        std::vector<std::pair<bool, uint64_t>> _are_triggered_and_ts;
    };

    /// make_stitch creates a stitch from functors.
    template <typename ThresholdCrossing, typename Event, typename ThresholdCrossingToEvent, typename HandleEvent>
    inline stitch<ThresholdCrossing, Event, ThresholdCrossingToEvent, HandleEvent> make_stitch(
        uint16_t width,
        uint16_t height,
        ThresholdCrossingToEvent threshold_crossing_to_event,
        HandleEvent handle_event) {
        return stitch<ThresholdCrossing, Event, ThresholdCrossingToEvent, HandleEvent>(
            width,
            height,
            std::forward<ThresholdCrossingToEvent>(threshold_crossing_to_event),
            std::forward<HandleEvent>(handle_event));
    }
}

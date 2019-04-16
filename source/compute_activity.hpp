#pragma once

#include <cmath>
#include <cstdint>
#include <utility>
#include <vector>

/// tarsier is a collection of event handlers.
namespace tarsier {
    /// compute_activity evaluates the activity at each pixel, using an exponential decay.
    template <typename Event, typename Activity, typename EventToActivity, typename HandleActivity>
    class compute_activity {
        public:
        compute_activity(
            uint16_t width,
            uint16_t height,
            float decay,
            EventToActivity event_to_activity,
            HandleActivity handle_activity) :
            _width(width),
            _decay(decay),
            _event_to_activity(std::forward<EventToActivity>(event_to_activity)),
            _handle_activity(std::forward<HandleActivity>(handle_activity)),
            _potentials_and_ts(width * height, {0.0f, 0}) {}
        compute_activity(const compute_activity&) = delete;
        compute_activity(compute_activity&&) = default;
        compute_activity& operator=(const compute_activity&) = delete;
        compute_activity& operator=(compute_activity&&) = default;
        virtual ~compute_activity() {}

        /// operator() handles an event.
        virtual void operator()(Event event) {
            auto& potential_and_t = _potentials_and_ts[event.x + event.y * _width];
            potential_and_t.first =
                potential_and_t.first * std::exp(-static_cast<float>(event.t - potential_and_t.second) / _decay) + 1;
            potential_and_t.second = event.t;
            _handle_activity(_event_to_activity(event, potential_and_t.first));
        }

        protected:
        const uint16_t _width;
        const float _decay;
        EventToActivity _event_to_activity;
        HandleActivity _handle_activity;
        std::vector<std::pair<float, uint64_t>> _potentials_and_ts;
    };

    /// make_compute_activity creates a compute_activity from functors.
    template <typename Event, typename Activity, typename EventToActivity, typename HandleActivity>
    inline compute_activity<Event, Activity, EventToActivity, HandleActivity> make_compute_activity(
        uint16_t width,
        uint16_t height,
        float decay,
        EventToActivity event_to_activity,
        HandleActivity handle_activity) {
        return compute_activity<Event, Activity, EventToActivity, HandleActivity>(
            width,
            height,
            decay,
            std::forward<EventToActivity>(event_to_activity),
            std::forward<HandleActivity>(handle_activity));
    }
}

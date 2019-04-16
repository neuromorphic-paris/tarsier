#pragma once

#include <array>
#include <cmath>
#include <cstdint>
#include <utility>
#include <vector>

/// tarsier is a collection of event handlers.
namespace tarsier {
    /// compute_time_surface extracts time surfaces from events.
    template <
        typename Event,
        typename Polarity,
        typename TimeSurface,
        uint16_t spatial_window,
        typename EventToTimeSurface,
        typename HandleTimeSurface>
    class compute_time_surface {
        public:
        compute_time_surface(
            uint16_t width,
            uint16_t height,
            uint64_t temporal_window,
            float decay,
            EventToTimeSurface event_to_time_surface,
            HandleTimeSurface handle_time_surface) :
            _width(width),
            _height(height),
            _temporal_window(temporal_window),
            _decay(decay),
            _event_to_time_surface(std::forward<EventToTimeSurface>(event_to_time_surface)),
            _handle_time_surface(std::forward<HandleTimeSurface>(handle_time_surface)),
            _ts_and_polarities(width * height, {0, false}) {}
        compute_time_surface(const compute_time_surface&) = delete;
        compute_time_surface(compute_time_surface&&) = default;
        compute_time_surface& operator=(const compute_time_surface&) = delete;
        compute_time_surface& operator=(compute_time_surface&&) = default;
        virtual ~compute_time_surface() {}

        /// operator() handles an event.
        virtual void operator()(Event event) {
            {
                const auto index = event.x + event.y * _width;
                _ts_and_polarities[index].first = event.t;
                _ts_and_polarities[index].second = event.polarity;
            }
            const auto t_threshold = (event.t <= _temporal_window ? 0 : event.t - _temporal_window);
            std::array<std::pair<float, Polarity>, (spatial_window * 2 + 1) * (spatial_window * 2 + 1)>
                projections_and_polarities;
            for (uint16_t y = (event.y <= spatial_window ? 0 : event.y - spatial_window);
                 y <= (event.y >= _height - 1 - spatial_window ? _height - 1 : event.y + spatial_window);
                 ++y) {
                for (uint16_t x = (event.x <= spatial_window ? 0 : event.x - spatial_window);
                     x <= (event.x >= _width - 1 - spatial_window ? _width - 1 : event.x + spatial_window);
                     ++x) {
                    const auto t_and_polarity = _ts_and_polarities[x + y * _width];
                    if (t_and_polarity.first > t_threshold) {
                        projections_and_polarities
                            [x + spatial_window - event.x + (y + spatial_window - event.y) * (2 * spatial_window + 1)] =
                                {std::exp(-static_cast<float>(event.t - t_and_polarity.first) / _decay),
                                 t_and_polarity.second};
                    }
                }
            }
            _handle_time_surface(_event_to_time_surface(event, projections_and_polarities));
        }

        protected:
        const uint16_t _width;
        const uint16_t _height;
        const uint64_t _temporal_window;
        const float _decay;
        EventToTimeSurface _event_to_time_surface;
        HandleTimeSurface _handle_time_surface;
        std::vector<std::pair<uint64_t, Polarity>> _ts_and_polarities;
    };

    /// make_compute_time_surface creates a compute_time_surface from functors.
    template <
        typename Event,
        typename Polarity,
        typename TimeSurface,
        uint16_t spatial_window,
        typename EventToTimeSurface,
        typename HandleTimeSurface>
    inline compute_time_surface<Event, Polarity, TimeSurface, spatial_window, EventToTimeSurface, HandleTimeSurface>
    make_compute_time_surface(
        uint16_t width,
        uint16_t height,
        uint64_t temporal_window,
        float decay,
        EventToTimeSurface event_to_time_surface,
        HandleTimeSurface handle_time_surface) {
        return compute_time_surface<
            Event,
            Polarity,
            TimeSurface,
            spatial_window,
            EventToTimeSurface,
            HandleTimeSurface>(
            width,
            height,
            temporal_window,
            decay,
            std::forward<EventToTimeSurface>(event_to_time_surface),
            std::forward<HandleTimeSurface>(handle_time_surface));
    }
}

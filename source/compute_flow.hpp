#pragma once

#include <cmath>
#include <cstdint>
#include <utility>
#include <vector>

/// tarsier is a collection of event handlers.
namespace tarsier {
    /// compute_flow evaluates the optical flow.
    template <typename Event, typename Flow, typename EventToFlow, typename HandleFlow>
    class compute_flow {
        public:
        compute_flow(
            uint16_t width,
            uint16_t height,
            uint16_t spatial_window,
            uint64_t temporal_window,
            std::size_t minimum_number_of_events,
            double maximum_speed,
            EventToFlow EventToflow,
            HandleFlow handle_flow) :
            _width(width),
            _height(height),
            _spatial_window(spatial_window),
            _temporal_window(temporal_window),
            _minimum_number_of_events(minimum_number_of_events),
            _squared_maximum_speed(std::pow(maximum_speed, 2)),
            _EventToflow(std::forward<EventToFlow>(EventToflow)),
            _handle_flow(std::forward<HandleFlow>(handle_flow)),
            _ts(width * height, 0) {}
        compute_flow(const compute_flow&) = delete;
        compute_flow(compute_flow&&) = default;
        compute_flow& operator=(const compute_flow&) = delete;
        compute_flow& operator=(compute_flow&&) = default;
        virtual ~compute_flow() {}

        /// operator() handles an event.
        virtual void operator()(Event event) {
            _ts[event.x + event.y * _width] = event.t;
            const auto t_threshold = (event.t <= _temporal_window ? 0 : event.t - _temporal_window);
            std::vector<point_t> points;
            for (uint16_t y = (event.y <= _spatial_window ? 0 : event.y - _spatial_window);
                 y <= (event.y >= _height - 1 - _spatial_window ? _height - 1 : event.y + _spatial_window);
                 ++y) {
                for (uint16_t x = (event.x <= _spatial_window ? 0 : event.x - _spatial_window);
                     x <= (event.x >= _width - 1 - _spatial_window ? _width - 1 : event.x + _spatial_window);
                     ++x) {
                    const auto t = _ts[x + y * _width];
                    if (t > t_threshold) {
                        points.push_back(point_t{
                            static_cast<double>(x),
                            static_cast<double>(y),
                            static_cast<double>(t),
                        });
                    }
                }
            }
            if (points.size() >= _minimum_number_of_events) {
                auto x_mean = 0.0;
                auto y_mean = 0.0;
                auto t_mean = 0.0;
                for (auto point : points) {
                    x_mean += point.x / points.size();
                    y_mean += point.y / points.size();
                    t_mean += point.t / points.size();
                }
                auto x_squared_sum = 0.0;
                auto y_squared_sum = 0.0;
                auto xy_sum = 0.0;
                auto xt_sum = 0.0;
                auto yt_sum = 0.0;
                for (auto point : points) {
                    const auto x_delta = point.x - x_mean;
                    const auto y_delta = point.y - y_mean;
                    const auto t_delta = point.t - t_mean;
                    x_squared_sum += std::pow(x_delta, 2);
                    y_squared_sum += std::pow(y_delta, 2);
                    xy_sum += x_delta * y_delta;
                    xt_sum += x_delta * t_delta;
                    yt_sum += y_delta * t_delta;
                }
                const auto determinant = x_squared_sum * y_squared_sum - std::pow(xy_sum, 2);
                const auto vx = determinant / (y_squared_sum * xt_sum - xy_sum * yt_sum);
                const auto vy = determinant / (x_squared_sum * yt_sum - xy_sum * xt_sum);
                if (std::pow(vx, 2.0) + std::pow(vy, 2.0) < _squared_maximum_speed) {
                    _handle_flow(_EventToflow(event, vx, vy));
                }
            }
        }

        protected:
        /// point_t represents a point in xyt space.
        struct point_t {
            double x;
            double y;
            double t;
        };

        const uint16_t _width;
        const uint16_t _height;
        const uint16_t _spatial_window;
        const uint64_t _temporal_window;
        const std::size_t _minimum_number_of_events;
        const double _squared_maximum_speed;
        EventToFlow _EventToflow;
        HandleFlow _handle_flow;
        std::vector<uint64_t> _ts;
    };

    /// make_compute_flow creates an optical flow estimator from functors.
    template <typename Event, typename Flow, typename EventToFlow, typename HandleFlow>
    compute_flow<Event, Flow, EventToFlow, HandleFlow> make_compute_flow(
        uint16_t width,
        uint16_t height,
        uint16_t spatial_window,
        uint64_t temporal_window,
        std::size_t minimum_number_of_events,
        double maximum_speed,
        EventToFlow EventToflow,
        HandleFlow handle_flow) {
        return compute_flow<Event, Flow, EventToFlow, HandleFlow>(
            width,
            height,
            spatial_window,
            temporal_window,
            minimum_number_of_events,
            maximum_speed,
            std::forward<EventToFlow>(EventToflow),
            std::forward<HandleFlow>(handle_flow));
    }
}

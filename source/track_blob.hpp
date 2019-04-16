#pragma once

#include <cmath>
#include <stdexcept>
#include <utility>

/// tarsier is a collection of event handlers.
namespace tarsier {
    /// track_blob averages the incoming events with a gaussian blob.
    template <typename Event, typename Blob, typename EventToBlob, typename HandleBlob>
    class track_blob {
        public:
        track_blob(
            float x,
            float y,
            float sigma_x_squared,
            float sigma_xy,
            float sigma_y_squared,
            float position_inertia,
            float variance_inertia,
            EventToBlob event_to_blob,
            HandleBlob handle_blob) :
            _x(x),
            _y(y),
            _sigma_x_squared(sigma_x_squared),
            _sigma_xy(sigma_xy),
            _sigma_y_squared(sigma_y_squared),
            _position_inertia(position_inertia),
            _variance_inertia(variance_inertia),
            _event_to_blob(std::forward<EventToBlob>(event_to_blob)),
            _handle_blob(std::forward<HandleBlob>(handle_blob)) {
            if (_position_inertia < 0 || _position_inertia > 1) {
                throw std::logic_error("position_inertia must be in the range [0, 1]");
            }
            if (_variance_inertia < 0 || _variance_inertia > 1) {
                throw std::logic_error("variance_inertia must be in the range [0, 1]");
            }
        }
        track_blob(const track_blob&) = delete;
        track_blob(track_blob&&) = default;
        track_blob& operator=(const track_blob&) = delete;
        track_blob& operator=(track_blob&&) = default;
        virtual ~track_blob() {}

        /// operator() handles an event.
        virtual void operator()(Event event) {
            const auto x_delta = event.x - _x;
            const auto y_delta = event.y - _y;
            _x = _position_inertia * _x + (1 - _position_inertia) * event.x;
            _y = _position_inertia * _y + (1 - _position_inertia) * event.y;
            _sigma_x_squared = _variance_inertia * _sigma_x_squared + (1 - _variance_inertia) * x_delta * x_delta;
            _sigma_xy = _variance_inertia * _sigma_xy + (1 - _variance_inertia) * x_delta * y_delta;
            _sigma_y_squared = _variance_inertia * _sigma_y_squared + (1 - _variance_inertia) * y_delta * y_delta;
            _handle_blob(_event_to_blob(event, _x, _y, _sigma_x_squared, _sigma_xy, _sigma_y_squared));
        }

        /// x returns the blob's center's x coordinate.
        float x() const {
            return _x;
        }

        /// y returns the blob's center's y coordinate.
        float y() const {
            return _y;
        }

        /// sigma_x_squared returns the blob's variance along the x axis.
        float sigma_x_squared() const {
            return _sigma_x_squared;
        }

        /// sigma_xy returns the blob's covariance.
        float sigma_xy() const {
            return _sigma_xy;
        }

        /// sigma_y_squared returns the blob's variance along the y axis.
        float sigma_y_squared() const {
            return _sigma_y_squared;
        }

        protected:
        float _x;
        float _y;
        float _sigma_x_squared;
        float _sigma_xy;
        float _sigma_y_squared;
        const float _position_inertia;
        const float _variance_inertia;
        EventToBlob _event_to_blob;
        HandleBlob _handle_blob;
    };

    /// make_track_blob creates a track_blob from functors.
    template <typename Event, typename Blob, typename EventToBlob, typename HandleBlob>
    inline track_blob<Event, Blob, EventToBlob, HandleBlob> make_track_blob(
        float x,
        float y,
        float sigma_x_squared,
        float sigma_xy,
        float sigma_y_squared,
        float position_inertia,
        float variance_inertia,
        EventToBlob event_to_blob,
        HandleBlob handle_blob) {
        return track_blob<Event, Blob, EventToBlob, HandleBlob>(
            x,
            y,
            sigma_x_squared,
            sigma_xy,
            sigma_y_squared,
            position_inertia,
            variance_inertia,
            std::forward<EventToBlob>(event_to_blob),
            std::forward<HandleBlob>(handle_blob));
    }
}

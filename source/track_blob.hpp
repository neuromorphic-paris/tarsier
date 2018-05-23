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
            float squared_sigma_x,
            float sigma_xy,
            float squared_sigma_y,
            float position_inertia,
            float variance_inertia,
            EventToBlob event_to_blob,
            HandleBlob handle_blob) :
            _x(x),
            _y(y),
            _squared_sigma_x(squared_sigma_x),
            _sigma_xy(sigma_xy),
            _squared_sigma_y(squared_sigma_y),
            _position_inertia(position_inertia),
            _variance_inertia(variance_inertia),
            _event_to_blob(std::forward<EventToBlob>(event_to_blob)),
            _handle_blob(std::forward<HandleBlob>(handle_blob)) {}
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
            _squared_sigma_x = _variance_inertia * _squared_sigma_x + (1 - _variance_inertia) * x_delta * x_delta;
            _sigma_xy = _variance_inertia * _sigma_xy + (1 - _variance_inertia) * x_delta * y_delta;
            _squared_sigma_y = _variance_inertia * _squared_sigma_y + (1 - _variance_inertia) * y_delta * y_delta;
            _handle_blob(_event_to_blob(event, _x, _y, _squared_sigma_x, _sigma_xy, _squared_sigma_y));
        }

        /// x returns the blob's center's x coordinate.
        float x() const {
            return _x;
        }

        /// y returns the blob's center's y coordinate.
        float y() const {
            return _y;
        }

        /// squared_sigma_x returns the blob's variance along the x axis.
        float squared_sigma_x() const {
            return _squared_sigma_x;
        }

        /// sigma_xy returns the blob's covariance.
        float sigma_xy() const {
            return _sigma_xy;
        }

        /// squared_sigma_y returns the blob's variance along the y axis.
        float squared_sigma_y() const {
            return _squared_sigma_y;
        }

        protected:
        float _x;
        float _y;
        float _squared_sigma_x;
        float _sigma_xy;
        float _squared_sigma_y;
        const float _position_inertia;
        const float _variance_inertia;
        EventToBlob _event_to_blob;
        HandleBlob _handle_blob;
    };

    /// make_track_blob creates a track_blob from functors.
    template <typename Event, typename Blob, typename EventToBlob, typename HandleBlob>
    track_blob<Event, Blob, EventToBlob, HandleBlob> make_track_blob(
        float x,
        float y,
        float squared_sigma_x,
        float sigma_xy,
        float squared_sigma_y,
        float position_inertia,
        float variance_inertia,
        EventToBlob event_to_blob,
        HandleBlob handle_blob) {
        return track_blob<Event, Blob, EventToBlob, HandleBlob>(
            x,
            y,
            squared_sigma_x,
            sigma_xy,
            squared_sigma_y,
            position_inertia,
            variance_inertia,
            std::forward<EventToBlob>(event_to_blob),
            std::forward<HandleBlob>(handle_blob));
    }
}

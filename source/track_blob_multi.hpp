#pragma once

#include <cmath>
#include <stdexcept>
#include <utility>
#include <vector>

/// tarsier is a collection of event handlers.
namespace tarsier {
    /// track_blob_multi averages the incoming events with a gaussian blob.
    template <typename Event, typename MultiBlobs, typename EventToBlob, typename HandleBlob>
    class track_blob_multi {
        public:
        track_blob_multi(
            MultiBlobs multi_blobs,
            float prob_threshold,
            float position_inertia,
            float variance_inertia,
            EventToBlob&& event_to_blob,
            HandleBlob&& handle_blob) :
            _multi_blobs(multi_blobs),
            _prob_threshold(prob_threshold),
            _position_inertia(position_inertia),
            _variance_inertia(variance_inertia),
            _event_to_blob(std::forward<EventToBlob>(event_to_blob)),
            _handle_blob(std::forward<HandleBlob>(handle_blob)) {
            if (_prob_threshold < 0 || _prob_threshold > 1) {
                throw std::logic_error("prob_threshold must be in the range [0, 1]");
            }
            if (_position_inertia < 0 || _position_inertia > 1) {
                throw std::logic_error("position_inertia must be in the range [0, 1]");
            }
            if (_variance_inertia < 0 || _variance_inertia > 1) {
                throw std::logic_error("variance_inertia must be in the range [0, 1]");
            }
        }
        track_blob_multi(const track_blob_multi&) = delete;
        track_blob_multi(track_blob_multi&&) = default;
        track_blob_multi& operator=(const track_blob_multi&) = delete;
        track_blob_multi& operator=(track_blob_multi&&) = default;
        virtual ~track_blob_multi() = default;

        /// operator() handles an event.
        virtual void operator()(Event event) {
            uint16_t max_id = 0;
            float max_prob = 0.0f;
            // compute probability of each tracker
            for (uint16_t i = 0; i < _multi_blobs.blobs.size(); i++) {
                auto sigma_x_squared = _multi_blobs.blobs[i].sigma_x_squared;
                auto sigma_xy = _multi_blobs.blobs[i].sigma_xy;
                auto sigma_y_squared = _multi_blobs.blobs[i].sigma_y_squared;

                const auto det = sigma_x_squared * sigma_y_squared - sigma_xy * sigma_xy;
                const auto x_delta = event.x - _multi_blobs.blobs[i].x;
                const auto y_delta = event.y - _multi_blobs.blobs[i].y;
                const auto exp_power = -0.5 / det * (x_delta * x_delta * sigma_y_squared +
                                                     y_delta * y_delta * sigma_x_squared);
                float prob = std::pow(det, -0.5) * std::exp(exp_power) / (2 * M_PI);
                
                if (prob > max_prob) {
                    max_prob = prob;
                    max_id = i;
                }
            }

            // update tracker
            if (max_prob >= _prob_threshold) {
                _multi_blobs.id = max_id;
                const auto x_delta = event.x - _multi_blobs.blobs[max_id].x;
                const auto y_delta = event.y - _multi_blobs.blobs[max_id].y;
                _multi_blobs.blobs[max_id].x = _position_inertia * _multi_blobs.blobs[max_id].x + (1 - _position_inertia) * event.x;
                _multi_blobs.blobs[max_id].y = _position_inertia * _multi_blobs.blobs[max_id].y + (1 - _position_inertia) * event.y;
                _multi_blobs.blobs[max_id].sigma_x_squared = _variance_inertia * _multi_blobs.blobs[max_id].sigma_x_squared + (1 - _variance_inertia) * x_delta * x_delta;
                _multi_blobs.blobs[max_id].sigma_xy = _variance_inertia * _multi_blobs.blobs[max_id].sigma_xy + (1 - _variance_inertia) * x_delta * y_delta;
                _multi_blobs.blobs[max_id].sigma_y_squared = _variance_inertia * _multi_blobs.blobs[max_id].sigma_y_squared + (1 - _variance_inertia) * y_delta * y_delta;
            }

            // TODO: compute tracker activity

            _handle_blob(_event_to_blob(event, _multi_blobs));
        }

        protected:
        MultiBlobs _multi_blobs;
        const float _prob_threshold;
        const float _position_inertia;
        const float _variance_inertia;
        EventToBlob _event_to_blob;
        HandleBlob _handle_blob;
    };

    /// make_track_blob_multi creates a track_blob_multi from functors.
    template <typename Event, typename MultiBlobs, typename EventToBlob, typename HandleBlob>
    inline track_blob_multi<Event, MultiBlobs, EventToBlob, HandleBlob> make_track_blob_multi(
        MultiBlobs multi_blobs,
        float prob_threshold,
        float position_inertia,
        float variance_inertia,
        EventToBlob&& event_to_blob,
        HandleBlob&& handle_blob) {
        return track_blob_multi<Event, MultiBlobs, EventToBlob, HandleBlob>(
            multi_blobs,
            prob_threshold,
            position_inertia,
            variance_inertia,
            std::forward<EventToBlob>(event_to_blob),
            std::forward<HandleBlob>(handle_blob));
    }
}

#pragma once

#include <cstdint>
#include <utility>

#include <vector>

/// tarsier is a collection of event handlers.
namespace tarsier {

    /// mask_redundant inverts the x coordinate.
    template <typename Event, typename HandleEvent>
    class mask_redundant {
        public:
        mask_redundant(uint16_t width, uint16_t height, uint64_t duration, HandleEvent handle_event) :
            _width(width),
            _height(height),
            _duration(duration),
            _handle_event(std::forward<HandleEvent>(handle_event)),
            _ts(width * height * 2) {}
        mask_redundant(const mask_redundant&) = delete;
        mask_redundant(mask_redundant&&) = default;
        mask_redundant& operator=(const mask_redundant&) = delete;
        mask_redundant& operator=(mask_redundant&&) = default;
        virtual ~mask_redundant() {}

        /// operator() handles an event.
        virtual void operator()(Event event) {
            auto index = (event.x + event.y * _width) * 2 + (event.is_increase ? 1 : 0);
            if (_ts[index] < event.t - _duration) {
                _ts[index] = event.t;
                _handle_event(event);
            }
        }

        protected:
        const uint16_t _width;
        const uint16_t _height;
        const uint64_t _duration;
        HandleEvent _handle_event;
        std::vector<uint64_t> _ts;
    };

    /// make_mask_redundant creates a mask_redundant from a functor.
    template <typename Event, typename HandleEvent>
    mask_redundant<Event, HandleEvent>
    make_mask_redundant(uint16_t width, uint16_t height, uint64_t duration, HandleEvent handle_event) {
        return mask_redundant<Event, HandleEvent>(width, height, duration, std::forward<HandleEvent>(handle_event));
    }
}

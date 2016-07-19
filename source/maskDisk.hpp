#pragma once

#include <utility>
#include <cmath>

/// tarsier is a collection of event handlers.
namespace tarsier {

    /// MaskDisk propagates only the events within the given disk.
    template <typename Event, typename HandleEvent>
    class MaskDisk {
        public:
            MaskDisk(std::pair<std::size_t, std::size_t> center, double radius, HandleEvent handleEvent) :
                _center(std::move(center)),
                _radius(radius),
                _handleEvent(std::forward<HandleEvent>(handleEvent))
            {
            }
            MaskDisk(const MaskDisk&) = delete;
            MaskDisk(MaskDisk&&) = default;
            MaskDisk& operator=(const MaskDisk&) = delete;
            MaskDisk& operator=(MaskDisk&&) = default;
            virtual ~MaskDisk() {}

            /// operator() handles an event.
            virtual void operator()(Event event) {
                if (
                    std::hypot(
                        static_cast<double>(event.x > _center.first ? event.x - _center.first : _center.first - event.x),
                        static_cast<double>(event.y > _center.second ? event.y - _center.second : _center.second - event.y)
                    ) < _radius
                ) {
                    _handleEvent(event);
                }
            }

        protected:
            std::pair<std::size_t, std::size_t> _center;
            double _radius;
            HandleEvent _handleEvent;
    };

    /// make_maskDisk creates a MaskDisk from a functor.
    template<typename Event, typename HandleEvent>
    MaskDisk<Event, HandleEvent> make_maskDisk(std::pair<std::size_t, std::size_t> center, double radius, HandleEvent handleEvent) {
        return MaskDisk<Event, HandleEvent>(std::move(center), radius, std::forward<HandleEvent>(handleEvent));
    }
}

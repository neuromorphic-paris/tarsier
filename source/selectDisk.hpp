#pragma once

#include <utility>
#include <cmath>

/// tarsier is a collection of event handlers.
namespace tarsier {

    /// SelectDisk propagates only the events within the given disk.
    template <typename Event, typename HandleEvent>
    class SelectDisk {
        public:
            SelectDisk(const double& centerX, const double& centerY, const double& radius, HandleEvent handleEvent) :
                _centerX(centerX),
                _centerY(centerY),
                _squaredRadius(std::pow(radius, 2)),
                _handleEvent(std::forward<HandleEvent>(handleEvent))
            {
            }
            SelectDisk(const SelectDisk&) = delete;
            SelectDisk(SelectDisk&&) = default;
            SelectDisk& operator=(const SelectDisk&) = delete;
            SelectDisk& operator=(SelectDisk&&) = default;
            virtual ~SelectDisk() {}

            /// operator() handles an event.
            virtual void operator()(Event event) {
                if (std::pow(static_cast<double>(event.x) - _centerX, 2) + std::pow(static_cast<double>(event.y) - _centerY, 2) < _squaredRadius) {
                    _handleEvent(event);
                }
            }

        protected:
            const double _centerX;
            const double _centerY;
            const double _squaredRadius;
            HandleEvent _handleEvent;
    };

    /// make_selectDisk creates a SelectDisk from a functor.
    template<typename Event, typename HandleEvent>
    SelectDisk<Event, HandleEvent> make_selectDisk(const double& centerX, const double& centerY, const double& radius, HandleEvent handleEvent) {
        return SelectDisk<Event, HandleEvent>(centerX, centerY, radius, std::forward<HandleEvent>(handleEvent));
    }
}

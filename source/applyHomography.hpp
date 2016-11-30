#pragma once

#include <cstdint>
#include <utility>
#include <cmath>

/// tarsier is a collection of event handlers.
namespace tarsier {

    /// Homography represents an homography matrix.
    /// zz is assumed to be normalized to one (hence it does not appear in the homography parameters).
    struct Homography {
        double xx;
        double xy;
        double xz;
        double yx;
        double yy;
        double yz;
        double zx;
        double zy;
    };

    /// ApplyHomography applies an homography to the events' coordinates.
    /// The events' coordinates are implicitly converted to homogeneous coordinates before applying the homography,
    /// and normalised afterwards.
    template <typename Event, typename HandleEvent>
    class ApplyHomography {
        public:
            ApplyHomography(std::size_t width, std::size_t height, Homography homography, HandleEvent handleEvent) :
                _width(width),
                _height(height),
                _homography(std::move(homography)),
                _handleEvent(std::forward<HandleEvent>(handleEvent))
            {
            }
            ApplyHomography(const ApplyHomography&) = delete;
            ApplyHomography(ApplyHomography&&) = default;
            ApplyHomography& operator=(const ApplyHomography&) = delete;
            ApplyHomography& operator=(ApplyHomography&&) = default;
            virtual ~ApplyHomography() {}

            /// operator() handles an event.
            virtual void operator()(Event event) {
                const auto normalisationFactor = _homography.zx * static_cast<double>(event.x) + _homography.zy * static_cast<double>(event.y) + 1;
                const auto xCandidate = std::round(
                    (_homography.xx * static_cast<double>(event.x) + _homography.xy * static_cast<double>(event.y) + _homography.xz)
                    * normalisationFactor
                );
                const auto yCandidate = std::round(
                    (_homography.yx * static_cast<double>(event.x) + _homography.yy * static_cast<double>(event.y) + _homography.yz)
                    * normalisationFactor
                );
                if (xCandidate >= 0 && xCandidate < _width && yCandidate >= 0 && yCandidate < _height) {
                    event.x = static_cast<decltype(event.x)>(xCandidate);
                    event.y = static_cast<decltype(event.x)>(yCandidate);
                    _handleEvent(std::move(event));
                }
            }

        protected:
            const std::size_t _width;
            const std::size_t _height;
            const Homography _homography;
            HandleEvent _handleEvent;
    };

    /// make_applyHomography creates a ApplyHomography from a functor.
    template<typename Event, typename HandleEvent>
    ApplyHomography<Event, HandleEvent> make_applyHomography(std::size_t width, std::size_t height, Homography homography, HandleEvent handleEvent) {
        return ApplyHomography<Event, HandleEvent>(width, height, std::move(homography), std::forward<HandleEvent>(handleEvent));
    }
}

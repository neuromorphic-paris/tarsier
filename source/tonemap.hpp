#pragma once

#include <utility>
#include <cmath>
#include <stdexcept>
#include <array>
#include <vector>
#include <limits>
#include <map>
#include <functional>

#include <iostream> // @DEBUG

/// tarsier is a collection of event handlers.
namespace tarsier {

    /// Scale determines the tone-mapping algorithm's behaviour.
    enum class Scale {
        globallyAndFixed,
        globallyAndAdaptive,
        locallyAndFixed,
        locallyAndAdaptive
    };

    /// Tonemap generates grey levels from high dynamic range events.
    template <
        typename HighDynamicRangeEvent,
        typename Event,
        std::size_t width,
        std::size_t height,
        Scale scale,
        typename EventFromHighDynamicRangeEvent,
        typename HandleEvent
    >
    class Tonemap;

    /// Tonemap partial specialisation for Scale::globallyAndFixed.
    template <typename HighDynamicRangeEvent,
        typename Event,
        std::size_t width,
        std::size_t height,
        typename EventFromHighDynamicRangeEvent,
        typename HandleEvent
    >
    class Tonemap<HighDynamicRangeEvent, Event, width, height, Scale::globallyAndFixed, EventFromHighDynamicRangeEvent, HandleEvent> {
        public:
            Tonemap(
                const double& alpha,
                EventFromHighDynamicRangeEvent eventFromHighDynamicRangeEvent,
                HandleEvent handleEvent
            ) :
                _alpha(alpha),
                _eventFromHighDynamicRangeEvent(std::forward<EventFromHighDynamicRangeEvent>(eventFromHighDynamicRangeEvent)),
                _handleEvent(std::forward<HandleEvent>(handleEvent)),
                _geometricMeanLogarithm(0.0),
                _numberOfDisplayedEvents(0)
            {
                _exposuresLogarithms.fill(std::numeric_limits<double>::infinity());
            }
            Tonemap(const Tonemap&) = delete;
            Tonemap(Tonemap&&) = default;
            Tonemap& operator=(const Tonemap&) = delete;
            Tonemap& operator=(Tonemap&&) = default;
            virtual ~Tonemap() {}

            /// operator() handles an event.
            virtual void operator()(HighDynamicRangeEvent highDynamicRangeEvent) {
                auto& exposureLogarithm = _exposuresLogarithms[highDynamicRangeEvent.x + highDynamicRangeEvent.y * width];
                if (exposureLogarithm != std::numeric_limits<double>::infinity()) {
                    _geometricMeanLogarithm -= exposureLogarithm;
                } else {
                    ++_numberOfDisplayedEvents;
                }
                exposureLogarithm = std::log(1.0 / static_cast<double>(highDynamicRangeEvent.timeDelta));
                _geometricMeanLogarithm += exposureLogarithm;
                _handleEvent(_eventFromHighDynamicRangeEvent(
                    highDynamicRangeEvent,
                    static_cast<float>(
                        1.0
                        /
                        (1.0 + std::pow(std::exp(_geometricMeanLogarithm / _numberOfDisplayedEvents) * highDynamicRangeEvent.timeDelta, _alpha))
                    )
                ));
            }

        protected:
            const double _alpha;
            EventFromHighDynamicRangeEvent _eventFromHighDynamicRangeEvent;
            HandleEvent _handleEvent;
            std::array<double, width * height> _exposuresLogarithms;
            double _geometricMeanLogarithm;
            std::size_t _numberOfDisplayedEvents;
    };

    /// Tonemap partial specialisation for Scale::globallyAndAdaptive.
    template <typename HighDynamicRangeEvent,
        typename Event,
        std::size_t width,
        std::size_t height,
        typename EventFromHighDynamicRangeEvent,
        typename HandleEvent
    >
    class Tonemap<HighDynamicRangeEvent, Event, width, height, Scale::globallyAndAdaptive, EventFromHighDynamicRangeEvent, HandleEvent> {
        public:
            Tonemap(
                const double& alpha,
                const double& percent,
                const double& target,
                const double& margin,
                EventFromHighDynamicRangeEvent eventFromHighDynamicRangeEvent,
                HandleEvent handleEvent
            ) :
                _alpha(alpha),
                _target(target),
                _margin(margin),
                _eventFromHighDynamicRangeEvent(std::forward<EventFromHighDynamicRangeEvent>(eventFromHighDynamicRangeEvent)),
                _handleEvent(std::forward<HandleEvent>(handleEvent)),
                _maximumNumberOfMinima(std::llround(height * width * 2 * percent / 100.0)),
                _numberOfDisplayedEvents(0),
                _geometricMeanLogarithm(0.0),
                _scaledTarget(std::log(1.0 / _target - 1.0)),
                _scaledLowerBound(std::pow(1.0 / (_target - _margin) - 1.0, 1.0 / _alpha)),
                _scaledUpperBound(std::pow(1.0 / (_target + _margin) - 1.0, 1.0 / _alpha))
            {
                if (alpha <= 0) {
                    throw std::runtime_error("percent must be greater than zero");
                }
                if (percent <= 0) {
                    throw std::runtime_error("percent must be greater than zero");
                }
                if (margin > target) {
                    throw std::runtime_error("margin must be smaller than target");
                }
                //_exposuresLogarithmsAndMinima.fill({std::numeric_limits<double>::infinity(), _minima.end()});
            }
            Tonemap(const Tonemap&) = delete;
            Tonemap(Tonemap&&) = default;
            Tonemap& operator=(const Tonemap&) = delete;
            Tonemap& operator=(Tonemap&&) = default;
            virtual ~Tonemap() {}

            /// operator() handles an event.
            virtual void operator()(HighDynamicRangeEvent highDynamicRangeEvent) {
                const auto index = highDynamicRangeEvent.x + highDynamicRangeEvent.y * width;
                auto& exposureLogarithmAndMinimum = _exposuresLogarithmsAndMinima[index];
                if (exposureLogarithmAndMinimum.first != std::numeric_limits<double>::infinity()) {
                    _geometricMeanLogarithm -= exposureLogarithmAndMinimum.first;
                } else {
                    ++_numberOfDisplayedEvents;
                }
                exposureLogarithmAndMinimum.first = std::log(1.0 / static_cast<double>(highDynamicRangeEvent.timeDelta));
                _geometricMeanLogarithm += exposureLogarithmAndMinimum.first;
                _handleEvent(_eventFromHighDynamicRangeEvent(
                    highDynamicRangeEvent,
                    static_cast<float>(
                        1.0
                        /
                        (1.0 + std::pow(std::exp(_geometricMeanLogarithm / _numberOfDisplayedEvents) * highDynamicRangeEvent.timeDelta, _alpha))
                    )
                ));

                /*
                if (exposureLogarithmAndMinimum.second != _minima.end()) {
                    _minima.erase(exposureLogarithmAndMinimum.second);
                    exposureLogarithmAndMinimum.second = _minima.insert({_geometricMeanLogarithm / _numberOfDisplayedEvents, index});
                } else if (_minima.size() < _maximumNumberOfMinima) {
                    exposureLogarithmAndMinimum.second = _minima.insert({_geometricMeanLogarithm / _numberOfDisplayedEvents, index});
                } else if (exposureLogarithmAndMinimum.first < std::prev(_minima.end())->first) {
                    exposureLogarithmAndMinimum.second = _minima.insert({_geometricMeanLogarithm / _numberOfDisplayedEvents, index});
                    _exposuresLogarithmsAndMinima[std::prev(_minima.end())->second].second = _minima.end();
                    _minima.erase(std::prev(_minima.end()));
                }
                const auto scaledPercent = 1.0 / std::prev(_minima.end())->first;
                if (scaledPercent > 1.0 && (scaledPercent > _scaledLowerBound || scaledPercent < _scaledUpperBound)) {
                    _alpha = _scaledTarget / std::log(scaledPercent);
                    _scaledLowerBound = std::pow(1.0 / (_target - _margin) - 1.0, 1.0 / _alpha);
                    _scaledUpperBound = std::pow(1.0 / (_target + _margin) - 1.0, 1.0 / _alpha);

                    std::cout << _alpha << std::endl; // @DEBUG
                }*/
            }

        protected:
            double _alpha;
            const double _target;
            const double _margin;
            EventFromHighDynamicRangeEvent _eventFromHighDynamicRangeEvent;
            HandleEvent _handleEvent;
            const std::size_t _maximumNumberOfMinima;
            std::size_t _numberOfDisplayedEvents;
            double _geometricMeanLogarithm;
            double _scaledTarget;
            double _scaledLowerBound;
            double _scaledUpperBound;
            std::array<std::pair<
                double,
                std::multimap<double, std::size_t>::const_iterator
            >, width * height> _exposuresLogarithmsAndMinima;
            std::multimap<std::size_t, std::size_t> _minima;
    };

/*

    /// Tonemap partial specialisation for Scale::locallyAndFixed.
    template <typename HighDynamicRangeEvent,
        typename Event,
        std::size_t width,
        std::size_t height,
        typename EventFromHighDynamicRangeEvent,
        typename HandleEvent
    >
    class Tonemap<HighDynamicRangeEvent, Event, width, height, Scale::locallyAndFixed, EventFromHighDynamicRangeEvent, HandleEvent> {
        public:
            Tonemap(EventFromHighDynamicRangeEvent eventFromHighDynamicRangeEvent, HandleEvent handleEvent) :
                _eventFromHighDynamicRangeEvent(std::forward<EventFromHighDynamicRangeEvent>(eventFromHighDynamicRangeEvent)),
                _handleEvent(std::forward<HandleEvent>(handleEvent))
            {
            }
            Tonemap(const Tonemap&) = delete;
            Tonemap(Tonemap&&) = default;
            Tonemap& operator=(const Tonemap&) = delete;
            Tonemap& operator=(Tonemap&&) = default;
            virtual ~Tonemap() {}

        protected:
            EventFromHighDynamicRangeEvent _eventFromHighDynamicRangeEvent;
            HandleEvent _handleEvent;
            std::array<uint64_t, width * height> _timeDeltas;

            /// operator() handles an event.
            virtual void operator()(Event event) {

            }
    };


    /// Tonemap partial specialisation for Scale::locallyAndAdaptive.
    template <typename HighDynamicRangeEvent,
        typename Event,
        std::size_t width,
        std::size_t height,
        typename EventFromHighDynamicRangeEvent,
        typename HandleEvent
    >
    class Tonemap<HighDynamicRangeEvent, Event, width, height, Scale::locallyAndAdaptive, EventFromHighDynamicRangeEvent, HandleEvent> {
        public:
            Tonemap(EventFromHighDynamicRangeEvent eventFromHighDynamicRangeEvent, HandleEvent handleEvent) :
                _eventFromHighDynamicRangeEvent(std::forward<EventFromHighDynamicRangeEvent>(eventFromHighDynamicRangeEvent)),
                _handleEvent(std::forward<HandleEvent>(handleEvent))
            {
            }
            Tonemap(const Tonemap&) = delete;
            Tonemap(Tonemap&&) = default;
            Tonemap& operator=(const Tonemap&) = delete;
            Tonemap& operator=(Tonemap&&) = default;
            virtual ~Tonemap() {}

        protected:
            EventFromHighDynamicRangeEvent _eventFromHighDynamicRangeEvent;
            HandleEvent _handleEvent;
            std::array<uint64_t, width * height> _timeDeltas;

            /// operator() handles an event.
            virtual void operator()(Event event) {

            }
    };

*/

    /// make_tonemap creates a Tonemap from functors.
    /// Specialisation for Scale::globallyAndFixed.
    template <
        typename HighDynamicRangeEvent,
        typename Event,
        std::size_t width,
        std::size_t height,
        typename EventFromHighDynamicRangeEvent,
        typename HandleEvent
    >
    Tonemap<HighDynamicRangeEvent, Event, width, height, Scale::globallyAndFixed, EventFromHighDynamicRangeEvent, HandleEvent> make_tonemap(
        const double& alpha,
        EventFromHighDynamicRangeEvent eventFromHighDynamicRangeEvent,
        HandleEvent handleEvent
    ) {
        return Tonemap<HighDynamicRangeEvent, Event, width, height, Scale::globallyAndFixed, EventFromHighDynamicRangeEvent, HandleEvent>(
            alpha,
            std::forward<EventFromHighDynamicRangeEvent>(eventFromHighDynamicRangeEvent),
            std::forward<HandleEvent>(handleEvent)
        );
    }

    /// make_tonemap creates a Tonemap from functors.
    /// Specialisation for Scale::globallyAndAdaptive.
    template <
        typename HighDynamicRangeEvent,
        typename Event,
        std::size_t width,
        std::size_t height,
        typename EventFromHighDynamicRangeEvent,
        typename HandleEvent
    >
    Tonemap<HighDynamicRangeEvent, Event, width, height, Scale::globallyAndAdaptive, EventFromHighDynamicRangeEvent, HandleEvent> make_tonemap(
        const double& alpha,
        const double& percent,
        const double& target,
        const double& margin,
        EventFromHighDynamicRangeEvent eventFromHighDynamicRangeEvent,
        HandleEvent handleEvent
    ) {
        return Tonemap<HighDynamicRangeEvent, Event, width, height, Scale::globallyAndAdaptive, EventFromHighDynamicRangeEvent, HandleEvent>(
            alpha,
            percent,
            target,
            margin,
            std::forward<EventFromHighDynamicRangeEvent>(eventFromHighDynamicRangeEvent),
            std::forward<HandleEvent>(handleEvent)
        );
    }
}

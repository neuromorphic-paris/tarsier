#pragma once

#include <cstdint>
#include <cmath>
#include <utility>
#include <array>
#include <vector>
#include <limits>
#include <algorithm>

/// tarsier is a collection of event handlers.
namespace tarsier {

    /// ComputeFlow evaluates the optical flow.
    template <
        typename Event,
        typename FlowEvent,
        std::size_t width,
        std::size_t height,
        std::size_t window,
        std::size_t numberOfMostRecentEvents,
        uint64_t lifespan,
        typename FlowEventFromEvent,
        typename HandleFlowEvent
    >
    class ComputeFlow {
        public:
            ComputeFlow(FlowEventFromEvent flowEventFromEvent, HandleFlowEvent handleFlowEvent) :
                _flowEventFromEvent(std::forward<FlowEventFromEvent>(flowEventFromEvent)),
                _handleFlowEvent(std::forward<HandleFlowEvent>(handleFlowEvent)),
                _timestamps(width * height, 0)
            {
            }
            ComputeFlow(const ComputeFlow&) = delete;
            ComputeFlow(ComputeFlow&&) = default;
            ComputeFlow& operator=(const ComputeFlow&) = delete;
            ComputeFlow& operator=(ComputeFlow&&) = default;
            virtual ~ComputeFlow() {}

            /// operator() handles an event.
            virtual void operator()(Event event) {
                _timestamps[event.x + event.y * width] = event.timestamp;
                const auto lifespanThreshold = (event.timestamp <= lifespan ? 0 : event.timestamp - lifespan);
                auto mostRecentXsAndYsAndTimeDeltas = std::vector<std::array<double, 3>>{};
                mostRecentXsAndYsAndTimeDeltas.reserve(window * window);
                for (std::size_t x = (event.x <= window ? 0 : event.x - window); x <= (event.x >= width - 1 - window ? width - 1 : event.x + window); ++x) {
                    for (std::size_t y = (event.y <= window ? 0 : event.y - window); y <= (event.y >= height - 1 - window ? height - 1 : event.y + window); ++y) {
                        const auto timestamp = _timestamps[x + y * width];
                        if (timestamp > lifespanThreshold) {
                            mostRecentXsAndYsAndTimeDeltas.push_back({{
                                static_cast<double>(x),
                                static_cast<double>(y),
                                static_cast<double>(event.timestamp - timestamp),
                            }});
                        }
                    }
                }
                if (mostRecentXsAndYsAndTimeDeltas.size() >= numberOfMostRecentEvents) {
                    std::sort(mostRecentXsAndYsAndTimeDeltas.begin(), mostRecentXsAndYsAndTimeDeltas.end());
                    auto xMean = 0.0;
                    auto yMean = 0.0;
                    auto timeDeltaMean = 0.0;
                    for (
                        auto mostRecentXAndYAndTimeDeltaIterator = mostRecentXsAndYsAndTimeDeltas.begin();
                        mostRecentXAndYAndTimeDeltaIterator != std::next(mostRecentXsAndYsAndTimeDeltas.begin(), numberOfMostRecentEvents);
                        ++ mostRecentXAndYAndTimeDeltaIterator
                    ) {
                        xMean += std::get<0>(*mostRecentXAndYAndTimeDeltaIterator) / numberOfMostRecentEvents;
                        yMean += std::get<1>(*mostRecentXAndYAndTimeDeltaIterator) / numberOfMostRecentEvents;
                        timeDeltaMean += std::get<2>(*mostRecentXAndYAndTimeDeltaIterator) / numberOfMostRecentEvents;
                    }
                    auto xSquaredSum = 0.0;
                    auto ySquaredSum = 0.0;
                    auto xySum = 0.0;
                    auto xTimeDeltaSum = 0.0;
                    auto yTimeDeltaSum = 0.0;
                    for (
                        auto mostRecentXAndYAndTimeDeltaIterator = mostRecentXsAndYsAndTimeDeltas.begin();
                        mostRecentXAndYAndTimeDeltaIterator != std::next(mostRecentXsAndYsAndTimeDeltas.begin(), numberOfMostRecentEvents);
                        ++ mostRecentXAndYAndTimeDeltaIterator
                    ) {
                        const auto xDelta = std::get<0>(*mostRecentXAndYAndTimeDeltaIterator) - xMean;
                        const auto yDelta = std::get<1>(*mostRecentXAndYAndTimeDeltaIterator) - yMean;
                        const auto timeDeltaDelta = std::get<2>(*mostRecentXAndYAndTimeDeltaIterator) - timeDeltaMean;
                        xSquaredSum += std::pow(xDelta, 2);
                        ySquaredSum += std::pow(yDelta, 2);
                        xySum += xDelta * yDelta;
                        xTimeDeltaSum += xDelta * timeDeltaDelta;
                        yTimeDeltaSum += yDelta * timeDeltaDelta;
                    }
                    const auto determinant = xSquaredSum * ySquaredSum - std::pow(xySum, 2);
                    if (std::abs(determinant) > 1) {
                        const auto xCoefficient = (xTimeDeltaSum * ySquaredSum - yTimeDeltaSum * xySum) / determinant;
                        const auto yCoefficient = (yTimeDeltaSum * xSquaredSum - xTimeDeltaSum * xySum) / determinant;
                        const auto invertedSquaredCoefficientsSum = 1.0 / (std::pow(xCoefficient, 2) + std::pow(yCoefficient, 2));
                        if (invertedSquaredCoefficientsSum > 1e-10 && invertedSquaredCoefficientsSum < 4e-6) {
                            _handleFlowEvent(_flowEventFromEvent(
                                event,
                                -xCoefficient * invertedSquaredCoefficientsSum,
                                -yCoefficient * invertedSquaredCoefficientsSum
                            ));
                        }
                    }
                }
            }

        protected:
            FlowEventFromEvent _flowEventFromEvent;
            HandleFlowEvent _handleFlowEvent;
            std::vector<uint64_t> _timestamps;
    };

    /// make_computeStitch creates a Stitch from functors.
    template <
        typename Event,
        typename FlowEvent,
        std::size_t width,
        std::size_t height,
        std::size_t window,
        std::size_t numberOfMostRecentEvents,
        uint64_t lifespan,
        typename FlowEventFromEvent,
        typename HandleFlowEvent
    >
    ComputeFlow<Event, FlowEvent, width, height, window, numberOfMostRecentEvents, lifespan, FlowEventFromEvent, HandleFlowEvent> make_computeFlow(
        FlowEventFromEvent flowEventFromEvent,
        HandleFlowEvent handleFlowEvent
    ) {
        return ComputeFlow<Event, FlowEvent, width, height, window, numberOfMostRecentEvents, lifespan, FlowEventFromEvent, HandleFlowEvent>(
            std::forward<FlowEventFromEvent>(flowEventFromEvent),
            std::forward<HandleFlowEvent>(handleFlowEvent)
        );
    }
}

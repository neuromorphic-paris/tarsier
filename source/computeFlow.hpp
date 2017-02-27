#pragma once

/// tarsier is a collection of event handlers.
namespace tarsier {

    /// Flow represents the parameters of an optical flow change.
    struct Flow {

        /// x represents the coordinate of the flow on the sensor grid alongside the horizontal axis.
        /// x is 0 on the left, and increases from left to right.
        uint16_t x;

        /// y represents the coordinate of the flow on the sensor grid alongside the vertical axis.
        /// y is 0 on the bottom, and increases bottom to top.
        uint16_t y;

        /// theta represents the flow angle in radians, with the horizontal axis as reference.
        /// theta is in the interval ]-Pi, Pi]
        double theta;

        /// intensity represents the flow intensity in pixels per microsecond.
        double intensity;
    }

    /// ComputeFlow evaluates the optical flow.
    template <typename Event, std::size_t width, std::size_t height, typename HandleFlow>
    class ComputeFlow {

        public:
            ComputeFlow(HandleFlow handleFlow) :
                _handleFlow(std::forward<HandleFlow>(handleFlow)),
                _timestamps(width * height, std::numeric_limits<int64_t>::lowest())
            {
            }
            ComputeFlow(const ComputeFlow&) = delete;
            ComputeFlow(ComputeFlow&&) = default;
            ComputeFlow& operator=(const ComputeFlow&) = delete;
            ComputeFlow& operator=(ComputeFlow&&) = default;
            virtual ~ComputeFlow() {}

            /// operator() handles an event.
            virtual void operator()(Event event) {
                _handleFlow(/* computed flow goes here*/);
            }

        protected:
            HandleFlow _handleFlow;
    }
}

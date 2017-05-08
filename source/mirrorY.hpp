#pragma once

#include <cstdint>
#include <utility>

/// tarsier is a collection of event handlers.
namespace tarsier {

    /// MirrorY inverts the y coordinate.
    template <typename Event, uint64_t height, typename HandleEvent>
    class MirrorY {
        public:
            MirrorY(HandleEvent handleEvent) :
                _handleEvent(std::forward<HandleEvent>(handleEvent))
            {
            }
            MirrorY(const MirrorY&) = delete;
            MirrorY(MirrorY&&) = default;
            MirrorY& operator=(const MirrorY&) = delete;
            MirrorY& operator=(MirrorY&&) = default;
            virtual ~MirrorY() {}

            /// operator() handles an event.
            virtual void operator()(Event event) {
                event.y = height - 1 - event.y;
                _handleEvent(event);
            }

        protected:
            HandleEvent _handleEvent;
    };

    /// make_mirrorY creates a MirrorY from a functor.
    template<typename Event, uint64_t height, typename HandleEvent>
    MirrorY<Event, height, HandleEvent> make_mirrorY(HandleEvent handleEvent) {
        return MirrorY<Event, height, HandleEvent>(std::forward<HandleEvent>(handleEvent));
    }
}

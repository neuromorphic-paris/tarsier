#pragma once

#include <utility>

/// tarsier is a collection of event handlers.
namespace tarsier {

    /// MirrorX inverts the x coordinate.
    template <typename Event, std::size_t width, typename HandleEvent>
    class MirrorX {
        public:
            MirrorX(HandleEvent handleEvent) :
                _handleEvent(std::forward<HandleEvent>(handleEvent))
            {
            }
            MirrorX(const MirrorX&) = delete;
            MirrorX(MirrorX&&) = default;
            MirrorX& operator=(const MirrorX&) = delete;
            MirrorX& operator=(MirrorX&&) = default;
            virtual ~MirrorX() {}

            /// operator() handles an event.
            virtual void operator()(Event event) {
                event.x = width - 1 - event.x;
                _handleEvent(std::move(event));
            }

        protected:
            HandleEvent _handleEvent;
    };

    /// make_mirrorX creates a MirrorX from a functor.
    template<typename Event, std::size_t width, typename HandleEvent>
    MirrorX<Event, width, HandleEvent> make_mirrorX(HandleEvent handleEvent) {
        return MirrorX<Event, width, HandleEvent>(std::forward<HandleEvent>(handleEvent));
    }
}

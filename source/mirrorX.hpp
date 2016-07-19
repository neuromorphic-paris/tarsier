#pragma once

#include <utility>

/// tarsier is a collection of event handlers.
namespace tarsier {

    /// MirrorX inverts the x coordinate.
    template <typename Event, typename HandleEvent>
    class MirrorX {
        public:
            MirrorX(std::size_t width, HandleEvent handleEvent) :
                _width(width),
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
                event.x = _width - 1 - event.x;
                _handleEvent(event);
            }

        protected:
            std::size_t _width;
            HandleEvent _handleEvent;
    };

    /// make_mirrorX creates a MirrorX from a functor.
    template<typename Event, typename HandleEvent>
    MirrorX<Event, HandleEvent> make_mirrorX(std::size_t width, HandleEvent handleEvent) {
        return MirrorX<Event, HandleEvent>(width, std::forward<HandleEvent>(handleEvent));
    }
}

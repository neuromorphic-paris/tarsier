#pragma once

#include <utility>

/// tarsier is a collection of event handlers.
namespace tarsier {

    /// MirrorY inverts the y coordinate.
    template <typename Event, typename HandleEvent>
    class MirrorY {
        public:
            MirrorY(std::size_t height, HandleEvent handleEvent) :
                _height(height),
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
                event.y = _height - 1 - event.y;
                _handleEvent(event);
            }

        protected:
            const std::size_t _height;
            HandleEvent _handleEvent;
    };

    /// make_mirrorY creates a MirrorY from a functor.
    template<typename Event, typename HandleEvent>
    MirrorY<Event, HandleEvent> make_mirrorY(std::size_t height, HandleEvent handleEvent) {
        return MirrorY<Event, HandleEvent>(height, std::forward<HandleEvent>(handleEvent));
    }
}

#pragma once

#include <utility>

/// tarsier is a collection of event handlers.
namespace tarsier {

    /// SelectRectangle propagates only the events within the specified rectangular window.
    template <typename Event, typename HandleEvent>
    class SelectRectangle {
        public:
            SelectRectangle(std::size_t left, std::size_t bottom, std::size_t width, std::size_t height, HandleEvent handleEvent) :
                _left(left),
                _bottom(bottom),
                _right(left + width),
                _top(bottom + height),
                _handleEvent(std::forward<HandleEvent>(handleEvent))
            {
            }
            SelectRectangle(const SelectRectangle&) = delete;
            SelectRectangle(SelectRectangle&&) = default;
            SelectRectangle& operator=(const SelectRectangle&) = delete;
            SelectRectangle& operator=(SelectRectangle&&) = default;
            virtual ~SelectRectangle() {}

            /// operator() handles an event.
            virtual void operator()(Event event) {
                if (event.x >= _left && event.x < _right && event.y >= _bottom && event.y < _top) {
                    _handleEvent(std::move(event));
                }
            }

        protected:
            const std::size_t _left;
            const std::size_t _bottom;
            const std::size_t _right;
            const std::size_t _top;
            HandleEvent _handleEvent;
    };

    /// make_selectRectangle creates a SelectRectangle from a functor.
    template<typename Event, typename HandleEvent>
    SelectRectangle<Event, HandleEvent> make_selectRectangle(
        std::size_t left,
        std::size_t bottom,
        std::size_t width,
        std::size_t height,
        HandleEvent handleEvent
    ) {
        return SelectRectangle<Event, HandleEvent>(left, bottom, width, height, std::forward<HandleEvent>(handleEvent));
    }
}

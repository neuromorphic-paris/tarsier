#pragma once

#include <utility>

/// tarsier is a collection of event handlers.
namespace tarsier {

    /// convert maps a type to another.
    template <typename Event, typename EventToConvertedEvent, typename HandleConvertedEvent>
    class convert {
        public:
        convert(EventToConvertedEvent event_to_converted_event, HandleConvertedEvent handle_converted_event) :
            _event_to_converted_event(std::forward<EventToConvertedEvent>(event_to_converted_event)),
            _handle_converted_event(std::forward<HandleConvertedEvent>(handle_converted_event)) {}
        convert(const convert&) = delete;
        convert(convert&&) = default;
        convert& operator=(const convert&) = delete;
        convert& operator=(convert&&) = default;
        virtual ~convert() {}

        /// operator() handles an event.
        virtual void operator()(Event event) {
            _handle_converted_event(_event_to_converted_event(event));
        }

        protected:
        EventToConvertedEvent _event_to_converted_event;
        HandleConvertedEvent _handle_converted_event;
    };

    /// make_convert creates a convert from functors.
    template <typename Event, typename EventToConvertedEvent, typename HandleConvertedEvent>
    inline convert<Event, EventToConvertedEvent, HandleConvertedEvent>
    make_convert(EventToConvertedEvent event_to_converted_event, HandleConvertedEvent handle_converted_event) {
        return convert<Event, EventToConvertedEvent, HandleConvertedEvent>(
            std::forward<EventToConvertedEvent>(event_to_converted_event),
            std::forward<HandleConvertedEvent>(handle_converted_event));
    }
}

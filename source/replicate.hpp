#pragma once

#include <cstdint>
#include <tuple>
#include <utility>

/// tarsier is a collection of event handlers.
namespace tarsier {

    /// replicate triggers several handlers for each event.
    template <typename Event, typename... HandleEventCallbacks>
    class replicate {
        public:
        replicate(HandleEventCallbacks... handle_event_callbacks) :
            _handle_event_callbacks(std::forward<HandleEventCallbacks>(handle_event_callbacks)...) {}
        replicate(const replicate&) = delete;
        replicate(replicate&&) = default;
        replicate& operator=(const replicate&) = delete;
        replicate& operator=(replicate&&) = default;
        virtual ~replicate() {}

        /// operator() handles an event.
        virtual void operator()(Event event) {
            replicate<Event, HandleEventCallbacks...>::trigger<0>(std::forward<Event>(event));
        }

        protected:
        /// trigger calls the n-th event callback.
        template <std::size_t index>
            typename std::enable_if < index<sizeof...(HandleEventCallbacks), void>::type trigger(Event event) {
            std::get<index>(_handle_event_callbacks)(event);
            trigger<index + 1>(std::forward<Event>(event));
        }

        /// trigger is a termination for the template loop.
        template <std::size_t index>
        typename std::enable_if<index == sizeof...(HandleEventCallbacks), void>::type trigger(Event) {}

        std::tuple<HandleEventCallbacks...> _handle_event_callbacks;
    };

    /// make_replicate creates a replicate from functors.
    template <typename Event, typename... HandleEventCallbacks>
    inline replicate<Event, HandleEventCallbacks...> make_replicate(HandleEventCallbacks... handle_event_callbacks) {
        return replicate<Event, HandleEventCallbacks...>(std::forward<HandleEventCallbacks>(handle_event_callbacks)...);
    }
}

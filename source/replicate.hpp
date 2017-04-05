#pragma once

#include <cstdint>
#include <utility>
#include <tuple>
#include <type_traits>

/// tarsier is a collection of event handlers.
namespace tarsier {

    /// Replicate triggers several handlers for each event.
    template <typename Event, typename ...HandleEventCallbacks>
    class Replicate {
        static_assert(std::is_copy_constructible<Event>::value, "Event must be copy-constructible");
        public:
            Replicate(HandleEventCallbacks... handleEventCallbacks) :
                _handleEventCallbacks(std::forward<HandleEventCallbacks>(handleEventCallbacks)...)
            {
            }
            Replicate(const Replicate&) = delete;
            Replicate(Replicate&&) = default;
            Replicate& operator=(const Replicate&) = delete;
            Replicate& operator=(Replicate&&) = default;
            virtual ~Replicate() {}

            /// operator() handles an event.
            virtual void operator()(Event event) {
                Replicate<Event, HandleEventCallbacks...>::trigger<0>(std::forward<Event>(event));
            }

        protected:

            /// trigger calls the n-th event callback.
            template<std::size_t Index>
            typename std::enable_if<Index < sizeof...(HandleEventCallbacks), void>::type
            trigger(Event event) {
                std::get<Index>(_handleEventCallbacks)(event);
                trigger<Index + 1>(std::forward<Event>(event));
            }

            /// trigger is a termination for the template loop.
            template<std::size_t Index>
            typename std::enable_if<Index == sizeof...(HandleEventCallbacks), void>::type
            trigger(Event) {}

            std::tuple<HandleEventCallbacks...> _handleEventCallbacks;
    };

    /// make_replicate creates a Replicate from functors.
    template <typename Event, typename ...HandleEventCallbacks>
    Replicate<Event, HandleEventCallbacks...> make_replicate(HandleEventCallbacks... handleEventCallbacks) {
        return Replicate<Event, HandleEventCallbacks...>(std::forward<HandleEventCallbacks>(handleEventCallbacks)...);
    }
}

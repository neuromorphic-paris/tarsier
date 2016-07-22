#pragma once

#include <cstdint>
#include <utility>
#include <tuple>

/// tarsier is a collection of event handlers.
namespace tarsier {

    /// Replicate triggers several handlers for each event.
    template <typename Event, typename ...HandleEventCallbacks>
    class Replicate {
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
                trigger(_handleEventCallbacks, event);
            }

        protected:

            /// trigger loops over the members of a tuple.
            template<std::size_t Index = 0, typename ...Tuple>
            static typename std::enable_if<Index == sizeof...(Tuple), void>::type
            trigger(std::tuple<Tuple...>&, Event& event) {}
            template<std::size_t Index = 0, typename ...Tuple>
            static typename std::enable_if<Index<sizeof...(Tuple), void>::type
            trigger(std::tuple<Tuple...>& tuple, Event& event) {
                std::get<Index>(tuple)(event);
                trigger<Index + 1, Tuple...>(tuple, event);
            }

            std::tuple<HandleEventCallbacks...> _handleEventCallbacks;
    };

    /// make_replicate creates a Replicate from functors.
    template <typename Event, typename ...HandleEventCallbacks>
    Replicate<Event, HandleEventCallbacks...> make_replicate(HandleEventCallbacks... handleEventCallbacks) {
        return Replicate<Event, HandleEventCallbacks...>(std::forward<HandleEventCallbacks>(handleEventCallbacks)...);
    }
}

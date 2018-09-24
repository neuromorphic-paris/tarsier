#pragma once

#include <array>
#include <atomic>
#include <cstdint>
#include <deque>
#include <limits>
#include <memory>
#include <utility>

/// tarsier is a collection of event handlers.
namespace tarsier {
    /// merge creates a unique event stream from sources running on different threads.
    template <std::size_t Sources, typename Event, typename HandleEvent>
    class merge {
        public:
        merge(HandleEvent handle_event) : _handle_event(std::forward<HandleEvent>(handle_event)), _dispatching(false) {
            _accessing_events.clear(std::memory_order_release);
        }
        merge(const merge&) = delete;
        merge(merge&&) = default;
        merge& operator=(const merge&) = delete;
        merge& operator=(merge&&) = default;
        virtual ~merge() {}

        /// push handles an event from a specified source.
        template <std::size_t Index>
        void push(Event event) {
            while (_accessing_events.test_and_set(std::memory_order_acquire)) {
            }
            const auto dispatch = std::get<Index>(_source_to_buffered_events).empty() && !_dispatching;
            std::get<Index>(_source_to_buffered_events).push_back(event);
            if (dispatch) {
                _dispatching = true;
                for (;;) {
                    auto minimum_t = std::numeric_limits<uint64_t>::max();
                    std::size_t minimum_index = 0;
                    for (std::size_t index = 0; index < Sources; ++index) {
                        if (_source_to_buffered_events[index].empty()) {
                            minimum_t = std::numeric_limits<uint64_t>::max();
                            break;
                        } else if (_source_to_buffered_events[index].front().t < minimum_t) {
                            minimum_t = _source_to_buffered_events[index].front().t;
                            minimum_index = index;
                        }
                    }
                    if (minimum_t == std::numeric_limits<uint64_t>::max()) {
                        _dispatching = false;
                        break;
                    } else {
                        auto minimum_event = _source_to_buffered_events[minimum_index].front();
                        _source_to_buffered_events[minimum_index].pop_front();
                        _accessing_events.clear(std::memory_order_release);
                        _handle_event(minimum_event);
                        while (_accessing_events.test_and_set(std::memory_order_acquire)) {
                        }
                    }
                }
            }
            _accessing_events.clear(std::memory_order_release);
        }

        protected:
        HandleEvent _handle_event;
        std::array<std::deque<Event>, Sources> _source_to_buffered_events;
        std::atomic_flag _accessing_events;
        bool _dispatching;
    };

    /// make_merge creates a merge from a functor.
    template <std::size_t Sources, typename Event, typename HandleEvent>
    std::unique_ptr<merge<Sources, Event, HandleEvent>> make_merge(HandleEvent handle_event) {
        return std::unique_ptr<merge<Sources, Event, HandleEvent>>(
            new merge<Sources, Event, HandleEvent>(std::forward<HandleEvent>(handle_event)));
    }
}

#pragma once

#include <array>
#include <atomic>
#include <chrono>
#include <limits>
#include <memory>
#include <stdexcept>
#include <thread>
#include <vector>

/// tarsier is a collection of event handlers.
namespace tarsier {
    /// merge creates a unique event stream from sources running on different
    /// threads.
    template <std::size_t sources, typename Event, typename HandleEvent>
    class merge {
        public:
        merge(
            std::size_t fifo_size,
            std::chrono::high_resolution_clock::duration sleep_duration,
            HandleEvent handle_event) :
            _fifo_size(fifo_size),
            _sleep_duration(sleep_duration),
            _handle_event(std::forward<HandleEvent>(handle_event)),
            _running(true) {
            for (auto& fifo : _fifos) {
                fifo.events.resize(_fifo_size);
                fifo.head.store(0, std::memory_order_release);
                fifo.tail.store(0, std::memory_order_release);
            }
            _loop = std::thread([this]() {
                while (_running.load(std::memory_order_acquire)) {
                    auto minimum_t = std::numeric_limits<uint64_t>::max();
                    std::size_t minimum_source = 0;
                    auto dispatch = true;
                    for (std::size_t source = 0; source < sources; ++source) {
                        if (_next_events_and_exists[source].second) {
                            if (dispatch && _next_events_and_exists[source].first.t < minimum_t) {
                                minimum_t = _next_events_and_exists[source].first.t;
                                minimum_source = source;
                            }
                        } else {
                            const auto current_head = _fifos[source].head.load(std::memory_order_relaxed);
                            if (current_head == _fifos[source].tail.load(std::memory_order_acquire)) {
                                dispatch = false;
                            } else {
                                _next_events_and_exists[source].first = _fifos[source].events[current_head];
                                _fifos[source].head.store((current_head + 1) % _fifo_size, std::memory_order_release);
                                _next_events_and_exists[source].second = true;
                                if (dispatch && _next_events_and_exists[source].first.t < minimum_t) {
                                    minimum_t = _next_events_and_exists[source].first.t;
                                    minimum_source = source;
                                }
                            }
                        }
                    }
                    if (dispatch) {
                        _handle_event(_next_events_and_exists[minimum_source].first);
                        _next_events_and_exists[minimum_source].second = false;
                    } else {
                        std::this_thread::sleep_for(_sleep_duration);
                    }
                }
            });
        }
        merge(const merge&) = delete;
        merge(merge&&) = default;
        merge& operator=(const merge&) = delete;
        merge& operator=(merge&&) = default;
        virtual ~merge() {
            _running.store(false, std::memory_order_release);
            _loop.join();
            std::array<bool, sources> has_events;
            has_events.fill(true);
            for (;;) {
                auto minimum_t = std::numeric_limits<uint64_t>::max();
                std::size_t minimum_source = 0;
                for (std::size_t source = 0; source < sources; ++source) {
                    if (has_events[source]) {
                        if (_next_events_and_exists[source].second) {
                            minimum_t = _next_events_and_exists[source].first.t;
                            minimum_source = source;
                        } else {
                            const auto current_head = _fifos[source].head.load(std::memory_order_relaxed);
                            if (current_head == _fifos[source].tail.load(std::memory_order_acquire)) {
                                has_events[source] = false;
                            } else {
                                _next_events_and_exists[source].first = _fifos[source].events[current_head];
                                _fifos[source].head.store((current_head + 1) % _fifo_size, std::memory_order_release);
                                _next_events_and_exists[source].second = true;
                                if (_next_events_and_exists[source].first.t < minimum_t) {
                                    minimum_t = _next_events_and_exists[source].first.t;
                                    minimum_source = source;
                                }
                            }
                        }
                    }
                }
                if (minimum_t == std::numeric_limits<uint64_t>::max()) {
                    break;
                } else {
                    _handle_event(_next_events_and_exists[minimum_source].first);
                    _next_events_and_exists[minimum_source].second = false;
                }
            }
        }

        /// push handles an event from a specified source.
        template <std::size_t source>
        bool push(Event event) {
            static_assert(source < sources, "source must be in the integer range [0, sources[");
            return push(source, event);
        }
        bool push(std::size_t source, Event event) {
            const auto current_tail = _fifos[source].tail.load(std::memory_order_relaxed);
            const auto next_tail = (current_tail + 1) % _fifo_size;
            if (next_tail == _fifos[source].head.load(std::memory_order_acquire)) {
                return false;
            }
            _fifos[source].events[current_tail] = event;
            _fifos[source].tail.store(next_tail, std::memory_order_release);
            return true;
        }

        protected:
        /// fifo stores the variables of a thread-safe fifo.
        struct fifo {
            std::vector<Event> events;
            std::atomic<std::size_t> head;
            std::atomic<std::size_t> tail;
        };

        const std::size_t _fifo_size;
        const std::chrono::high_resolution_clock::duration _sleep_duration;
        HandleEvent _handle_event;
        std::array<fifo, sources> _fifos;
        std::array<std::pair<Event, bool>, sources> _next_events_and_exists;
        std::thread _loop;
        std::atomic_bool _running;
    };

    /// make_merge creates a merge from a functor.
    template <std::size_t sources, typename Event, typename HandleEvent>
    inline std::unique_ptr<merge<sources, Event, HandleEvent>> make_merge(
        std::size_t fifo_size,
        std::chrono::high_resolution_clock::duration sleep_duration,
        HandleEvent handle_event) {
        return std::unique_ptr<merge<sources, Event, HandleEvent>>(
            new merge<sources, Event, HandleEvent>(fifo_size, sleep_duration, std::forward<HandleEvent>(handle_event)));
    }
}

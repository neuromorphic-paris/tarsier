#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace tarsier {

/// ComputeActivity evaluates the activity within a temporal neighbourhood
template <typename Event, typename ActivityEvent, uint64_t lifespan,
          typename ActivityEventFromEvent, typename HandleActivityEvent>
class ComputeActivity {
public:
  ComputeActivity(ActivityEventFromEvent activityEventFromEvent,
                  HandleActivityEvent handleActivityEvent)
      : _activityEventFromEvent(
            std::forward<ActivityEventFromEvent>(activityEventFromEvent)),
        _handleActivityEvent(
            std::forward<HandleActivityEvent>(handleActivityEvent)),
        _lifespan(lifespan), _activity(0), _lastTimeStamp(0), _activityON(0),
        _lastTimeStampON(0), _activityOFF(0), _lastTimeStampOFF(0) {}

  ComputeActivity(const ComputeActivity &) = delete;
  ComputeActivity(ComputeActivity &&) = default;
  ComputeActivity &operator=(const ComputeActivity &) = delete;
  ComputeActivity &operator=(ComputeActivity &&) = default;
  virtual ~ComputeActivity() {}

  virtual void operator()(Event event) {
    // exponential decay depending on the difference to the last timestamp
    _activity *=
        exp(-static_cast<double>(event.timestamp - _lastTimeStamp) / _lifespan);
    // every event generates an activity of 1
    _activity += 1;
    _lastTimeStamp = event.timestamp;

    if (event.polarity) {
      _activityON *= exp(
          -static_cast<double>(event.timestamp - _lastTimeStampON) / _lifespan);
      _activityON += 1;
      _lastTimeStampON = event.timestamp;
    }
    if (!event.polarity) {
      _activityOFF *=
          exp(-static_cast<double>(event.timestamp - _lastTimeStampOFF) /
              _lifespan);
      _activityOFF += 1;
      _lastTimeStampOFF = event.timestamp;
    }
    _handleActivityEvent(
        _activityEventFromEvent(event, _activity, _activityON, _activityOFF));
  }

protected:
  ActivityEventFromEvent _activityEventFromEvent;
  const uint64_t _lifespan;
  double _activity;
  uint64_t _lastTimeStamp;
  double _activityON;
  uint64_t _lastTimeStampON;
  double _activityOFF;
  uint64_t _lastTimeStampOFF;
  HandleActivityEvent _handleActivityEvent;
};

template <typename Event, typename ActivityEvent, uint64_t lifespan,
          typename ActivityEventFromEvent, typename HandleActivityEvent>
ComputeActivity<Event, ActivityEvent, lifespan, ActivityEventFromEvent,
                HandleActivityEvent>
make_computeActivity(ActivityEventFromEvent activityEventFromEvent,
                     HandleActivityEvent handleActivityEvent) {
  return ComputeActivity<Event, ActivityEvent, lifespan, ActivityEventFromEvent,
                         HandleActivityEvent>(
      std::forward<ActivityEventFromEvent>(activityEventFromEvent),
      std::forward<HandleActivityEvent>(handleActivityEvent));
}
}

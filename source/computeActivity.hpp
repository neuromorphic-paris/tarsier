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

  void activityIncrease(double &currentActivity, uint64_t &currentTimestamp,
                        uint64_t &lastTimeStamp) {
    // exponential decay depending on the difference to the last timestamp
    currentActivity *=
        exp(-static_cast<double>(currentTimestamp - lastTimeStamp) / _lifespan);
    // every event generates an activity of 1
    currentActivity += 1;
  }

  virtual void operator()(Event event) {
    _currentTimeStamp = event.timestamp;
    activityIncrease(_activity, _currentTimeStamp, _lastTimeStamp);
    _lastTimeStamp = _currentTimeStamp;

    if (event.polarity) {
      activityIncrease(_activityON, _currentTimeStamp, _lastTimeStampON);
      _lastTimeStampON = _currentTimeStamp;
    }
    if (!event.polarity) {
      activityIncrease(_activityOFF, _currentTimeStamp, _lastTimeStampOFF);
      _lastTimeStampOFF = _currentTimeStamp;
    }
    _handleActivityEvent(
        _activityEventFromEvent(event, _activity, _activityON, _activityOFF));
  }

protected:
  ActivityEventFromEvent _activityEventFromEvent;
  const uint64_t _lifespan;
  uint64_t _currentTimeStamp;
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

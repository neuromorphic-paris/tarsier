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
        _lifespan(lifespan), _activity(1), _lastTimeStamp(0) {}

  ComputeActivity(const ComputeActivity &) = delete;
  ComputeActivity(ComputeActivity &&) = default;
  ComputeActivity &operator=(const ComputeActivity &) = delete;
  ComputeActivity &operator=(ComputeActivity &&) = default;
  virtual ~ComputeActivity() {}

  virtual void operator()(Event event) {
    _activity =
        _activity *
            exp(-static_cast<double>(atisEvent.timestamp - _lastTimeStamp) /
                _lifespan) +
        1;
    _lastTimeStamp = event.timestamp;
    _handleActivityEvent(_activityEventFromEvent(event, _activity));
  }

protected:
  const double _lifespan;
  double _activity;
  uint64_t _lastTimeStamp;

}

template <typename Event, typename ActivityEvent, uint64_t lifespan,
          typename ActivityEventFromEvent, typename HandleActivityEvent>
ComputeActivity<Event, ActivityEvent, lifespan, ActivityEventFromEvent,
                HandleActivityEvent>
make_computeActivity(ActivityEventFromEvent activityEventFromEvent,
                     HandleActivityEvent handleActivityEvent) {
  return ComputeActivity<Event, ActivityEvent, lifespan, ActivityEventFromEvent,
                         HandleActivityEvent>(
      std::forward<ActivityEventFromEvent>(activityEventFromEvent),
      std::forward < HandleActivityEvent(handleActivityEvent));
}
}

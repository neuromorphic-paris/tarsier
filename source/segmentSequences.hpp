#pragma once

#include <vector>

/// tarsier is a collection of event handler
namespace tarsier{
  template<
    typename Event,// must be contain at least a field .sequenceTrigger
    typename HandlerSequences,
    typename HandlerSegmentSequences>
  class SegmentSequences{
  public:
    SegmentSequences(HandlerSequences handlerSequences,
                     HandlerSegmentSequences handlerSegmentSequences):
      _handlerSequences(std::forward<HandlerSequences>(handlerSequences)),
      _handlerSegmentSequences(std::forward<HandlerSegmentSequences>(handlerSegmentSequences)),
      _curentSequence(0),
      _stackEvent(false)
    {}

    virtual ~SegmentSequences(){}

    virtual void operator()(Event ev){
      if(ev.sequenceTrigger == 1){
        _curentSequence.clear();
      }else if(ev.sequenceTrigger == -1){
        _handlerSequences(_curentSequence);
      }

      if(_stackEvent){
        _curentSequence.push_back(ev);
      }

      _handlerSegmentSequences(ev);
    }

  protected:
    HandlerSequences _handlerSequences;
    HandlerSegmentSequences _handlerSegmentSequences;

    std::vector<Event> _curentSequence;
    bool _stackEvent;
  };

  /// make_segmentSequences
  template<
    typename Event,// must be contain at least a field .sequenceTrigger
    typename HandlerSequences,
    typename HandlerSegmentSequences>
  SegmentSequences<Event,
                   HandlerSequences,
                   HandlerSegmentSequences>
  make_segmentSequences(HandlerSequences handlerSequences,
                        HandlerSegmentSequences handlerSegmentSequences)
  {
    return SegmentSequences<Event,
                            HandlerSequences,
                            HandlerSegmentSequences>
      (std::forward<HandlerSequences>(handlerSequences),
       std::forward<HandlerSegmentSequences>(handlerSegmentSequences)
       );
  }
};

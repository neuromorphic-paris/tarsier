#pragma once

#include <iostream>
#include <algorithm>
#include <cstdint>
#include <cmath>
#include <utility>
#include <vector>
#include <array>
#include <numeric>

/// tarsier is a collection of event handlers.
namespace tarsier {

  /// Generic TimeSurfaceGenerator, pure virtual
  template<
    int64_t memorySize,
    int64_t contextSize,
    int64_t initMemory,
    typename Event,
    typename TimeSurfaceEvent,
    typename Kernel, // double f(Event ref, Event neighbor)
    typename TimeSurfaceEventFromEvent, // TimeSurfaceEvent f(Event, std::vector<double>)
    typename HandlerTimeSurfaceGenerator //  void f(TimeSurfaceEvent)
    >
  class TimeSurfaceGenerator{
  public:
    TimeSurfaceGenerator(Kernel kernel,
                         TimeSurfaceEventFromEvent timeSurfaceEventFromEvent,
                         HandlerTimeSurfaceGenerator handlerTimeSurfaceGenerator):
      _kernel(std::forward<Kernel>(kernel)),
      _timeSurfaceEventFromEvent(std::forward<TimeSurfaceEventFromEvent>(timeSurfaceEventFromEvent)),
      _handlerTimeSurfaceGenerator(std::forward<HandlerTimeSurfaceGenerator>(handlerTimeSurfaceGenerator))
    {
      for(auto&& it: _memory){
        it = initMemory;
      }
      for(auto&& it: _context){
        it = 0.;
      }
    }

    virtual ~TimeSurfaceGenerator(){}

    virtual void operator()(Event ev)=0;

  protected:
    Kernel _kernel;
    TimeSurfaceEventFromEvent _timeSurfaceEventFromEvent;
    HandlerTimeSurfaceGenerator _handlerTimeSurfaceGenerator;

    std::array<int64_t, memorySize> _memory;
    std::array<double, contextSize> _context;
  };

  /// 1D TimeSurfaceGenerator
  template<
    int64_t X,
    int64_t nP,
    int64_t radius,
    int64_t initMemory,
    typename Event, // require at least a field .t .x .p
    typename TimeSurfaceEvent,
    typename Kernel, // double f(Event ref, Event neighbor)
    typename TimeSurfaceEventFromEvent, // TimeSurfaceEvent f(Event, std::vector<double>)
    typename HandlerTimeSurfaceGenerator //  void f(TimeSurfaceEvent)
    >
  class TimeSurfaceGenerator1D: public TimeSurfaceGenerator<X*nP,
                                                            (2*radius+1)*nP,
                                                            initMemory,
                                                            Event,
                                                            TimeSurfaceEvent,
                                                            Kernel,
                                                            TimeSurfaceEventFromEvent,
                                                            HandlerTimeSurfaceGenerator
                                                            >{
  public:
    TimeSurfaceGenerator1D(Kernel kernel,
                           TimeSurfaceEventFromEvent timeSurfaceEventFromEvent,
                           HandlerTimeSurfaceGenerator handlerTimeSurfaceGenerator):
      TimeSurfaceGenerator<X*nP,
                           (2*radius+1)*nP,
                           initMemory,
                           Event,
                           TimeSurfaceEvent,
                           Kernel,
                           TimeSurfaceEventFromEvent,
                           HandlerTimeSurfaceGenerator
                           >(std::forward<Kernel>(kernel),
                             std::forward<TimeSurfaceEventFromEvent>(timeSurfaceEventFromEvent),
                             std::forward<HandlerTimeSurfaceGenerator>(handlerTimeSurfaceGenerator))
    {}

    virtual void operator()(Event ev) {
      this->_memory[ev.p*X+ev.x] = static_cast<int64_t>(ev.t);
      uint64_t cpt = 0;

      for(int64_t p = 0; p < nP; p++){
        for(int64_t x = static_cast<int64_t>(ev.x)-radius;
            x <= static_cast<int64_t>(ev.x)+radius;
            x++){
          if(x >= 0 && x < X){
            this->_context[cpt++] = this->_kernel(ev,Event{this->_memory[p*X+x], x, p});
          }else{
            this->_context[cpt++] = 0;
          }
        }
      }

      this->_handlerTimeSurfaceGenerator(this->_timeSurfaceEventFromEvent(ev,this->_context));
    }
  };

  /// 2D TimeSurfaceGenerator
  template<
    int64_t X,
    int64_t Y,
    int64_t nP,
    int64_t radius,
    int64_t initMemory,
    typename Event, // require at least a field .t .x .y .p
    typename TimeSurfaceEvent,
    typename Kernel, // double f(Event ref, Event neighbor)
    typename TimeSurfaceEventFromEvent, // TimeSurfaceEvent f(Event, std::vector<double>)
    typename HandlerTimeSurfaceGenerator //  void f(TimeSurfaceEvent)
    >
  class TimeSurfaceGenerator2D: public TimeSurfaceGenerator<X*Y*nP,
                                                            (2*radius+1)*(2*radius+1)*nP,
                                                            initMemory,
                                                            Event,
                                                            TimeSurfaceEvent,
                                                            Kernel,
                                                            TimeSurfaceEventFromEvent,
                                                            HandlerTimeSurfaceGenerator
                                                            >{
  public:
    TimeSurfaceGenerator2D(Kernel kernel,
                           TimeSurfaceEventFromEvent timeSurfaceEventFromEvent,
                           HandlerTimeSurfaceGenerator handlerTimeSurfaceGenerator):
      TimeSurfaceGenerator<X*Y*nP,
                           (2*radius+1)*(2*radius+1)*nP,
                           initMemory,
                           Event,
                           TimeSurfaceEvent,
                           Kernel,
                           TimeSurfaceEventFromEvent,
                           HandlerTimeSurfaceGenerator
                           >(std::forward<Kernel>(kernel),
                             std::forward<TimeSurfaceEventFromEvent>(timeSurfaceEventFromEvent),
                             std::forward<HandlerTimeSurfaceGenerator>(handlerTimeSurfaceGenerator))
    {}

    virtual void operator()(Event ev){
      this->_memory[ev.p*X*Y + ev.y*X + ev.x] = static_cast<int64_t>(ev.t);
      uint64_t cpt = 0;

      for(int64_t p = 0; p < nP; p++){
        for(int64_t x = static_cast<int64_t>(ev.x)-radius;
            x <= static_cast<int64_t>(ev.x)+radius;
            x++){
          for(int64_t y = static_cast<int64_t>(ev.y)-radius;
              y <= static_cast<int64_t>(ev.y)+radius;
              y++){
            if(x >= 0 && x < X & y >= 0 && y < Y){
              this->_context[cpt++] = this->_kernel(ev,Event{this->_memory[p*X*Y + y*X + x], x, y, p});
            }else{
              this->_context[cpt++] = 0;
            }
          }
        }
      }
      this->_handlerTimeSurfaceGenerator(this->_timeSurfaceEventFromEvent(ev,this->_context));
    }
  };

  //------------------------------------------------------------------------------------------\\
  /// make_timeSurfaceGenerator

  /// 1D version
  template<
    int64_t X,
    int64_t nP,
    int64_t radius,
    int64_t initMemory,
    typename Event, //Requires at least a field .t, .x, .p
    typename TimeSurfaceEvent,
    typename Kernel, // double f(Event ref, Event neighbor)
    typename TimeSurfaceEventFromEvent, // TimeSurfaceEvent f(Event, std::vector<double>)
    typename HandlerTimeSurfaceGenerator // void f(TimeSurfaceEvent)
    >
  TimeSurfaceGenerator1D<X,
                         nP,
                         radius,
                         initMemory,
                         Event,
                         TimeSurfaceEvent,
                         Kernel,
                         TimeSurfaceEventFromEvent,
                         HandlerTimeSurfaceGenerator>
  make_timeSurfaceGenerator(Kernel kernel,
                            TimeSurfaceEventFromEvent timeSurfaceEventFromEvent,
                            HandlerTimeSurfaceGenerator handlerTimeSurfaceGenerator)
  {
    return TimeSurfaceGenerator1D<X,
                                  nP,
                                  radius,
                                  initMemory,
                                  Event,
                                  TimeSurfaceEvent,
                                  Kernel,
                                  TimeSurfaceEventFromEvent,
                                  HandlerTimeSurfaceGenerator>
      (std::forward<Kernel>(kernel),
       std::forward<TimeSurfaceEventFromEvent>(timeSurfaceEventFromEvent),
       std::forward<HandlerTimeSurfaceGenerator>(handlerTimeSurfaceGenerator)
       );
  }

  /// 2D version
  template<
    int64_t X,
    int64_t Y,
    int64_t nP,
    int64_t radius,
    int64_t initMemory,
    typename Event, //Requires at least a field .t, .x, .y, .p
    typename TimeSurfaceEvent,
    typename Kernel, // double f(Event ref, Event neighbor)
    typename TimeSurfaceEventFromEvent, // TimeSurfaceEvent f(Event, std::vector<double>)
    typename HandlerTimeSurfaceGenerator // void f(TimeSurfaceEvent)
    >
  TimeSurfaceGenerator2D<X,
                         Y,
                         nP,
                         radius,
                         initMemory,
                         Event,
                         TimeSurfaceEvent,
                         Kernel,
                         TimeSurfaceEventFromEvent,
                         HandlerTimeSurfaceGenerator>
  make_timeSurfaceGenerator(Kernel kernel,
                            TimeSurfaceEventFromEvent timeSurfaceEventFromEvent,
                            HandlerTimeSurfaceGenerator handlerTimeSurfaceGenerator)
  {
    return TimeSurfaceGenerator2D<X,
                                  Y,
                                  nP,
                                  radius,
                                  initMemory,
                                  Event,
                                  TimeSurfaceEvent,
                                  Kernel,
                                  TimeSurfaceEventFromEvent,
                                  HandlerTimeSurfaceGenerator>
      (std::forward<Kernel>(kernel),
       std::forward<TimeSurfaceEventFromEvent>(timeSurfaceEventFromEvent),
       std::forward<HandlerTimeSurfaceGenerator>(handlerTimeSurfaceGenerator)
       );
  }
};

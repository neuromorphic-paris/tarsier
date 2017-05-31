#include "../source/timeSurfaceGenerator.hpp"

#include "catch.hpp"

struct Event1d{ // minimum field require to build 1D TS
  int64_t t;
  int64_t x;
  int64_t p;
};

struct TsEvent1d{
  int64_t t;
  int64_t x;
  int64_t p;
  std::vector<double> context;
};

struct Event2d{ // minimum field require to build 2D TS
  int64_t t;
  int64_t x;
  int64_t y;
  int64_t p;
};

struct TsEvent2d{
  int64_t t;
  int64_t x;
  int64_t y;
  int64_t p;
  std::vector<double> context;
};

TEST_CASE("Compute 1D timeSurfaces from the given events", "[TimeSurface]") {
  auto tsEvent1dFromEvent = [](Event1d ev, std::vector<double> context){
    return TsEvent1d{ev.t, ev.x, ev.p, context};
  };
  auto kernel1d = [](Event1d evRef, Event1d evNeighbor){
    auto diff = static_cast<double>(evRef.t)-static_cast<double>(evNeighbor.t);
    return (diff < 3*100.) ? exp(-(diff)/100.) : 0;
  };
  bool go = false;
  auto handlerTimeSurface1d = [&go](TsEvent1d ev){
    if(go){
      std::cout << "J'ai recu un event 1d" << std::endl;
      for(auto&& it: ev.context){
        std::cout << it << " ";
      }
      std::cout << std::endl;
    }
  };
  /*
  auto tsEvent2dFromEvent = [](Event2d ev, std::vector<double> context){
    return TsEvent2d{ev.t, ev.x, ev.y, ev.p, context};
  };
  auto kernel2d = [](Event2d evRef, Event2d evNeighbor){
    auto diff = static_cast<double>(evRef.t)-static_cast<double>(evNeighbor.t);
    return (diff < 3*100.) ? exp(-(diff)/100.) : 0;
  };

  auto handlerTimeSurface2d = [&go](TsEvent2d ev){
    if(go){
      std::cout << "J'ai recu un event 2d" << std::endl;
      for(auto&& it: ev.context){
        std::cout << it << " ";
      }
      std::cout << std::endl;
    }
    };*/

  auto myTs1d = tarsier::make_timeSurfaceGenerator<200, 2, 5,-10000, Event1d, TsEvent1d>(kernel1d, tsEvent1dFromEvent, handlerTimeSurface1d);
  // auto myTs2d = tarsier::make_timeSurfaceGenerator<200, 200, 2, 5,-10000, Event2d, TsEvent2d>(kernel2d, tsEvent2dFromEvent, handlerTimeSurface2d);

  myTs1d(Event1d{0, 10, 1});
  myTs1d(Event1d{10, 9, 1});
  myTs1d(Event1d{20, 8, 0});
  myTs1d(Event1d{30, 9, 0});
  myTs1d(Event1d{40, 10, 0});
  myTs1d(Event1d{50, 11, 0});
  myTs1d(Event1d{60, 12, 0});
  myTs1d(Event1d{70, 13, 0});
  myTs1d(Event1d{80, 12, 1});
  myTs1d(Event1d{90, 11, 1});
  go = true;
  myTs1d(Event1d{100, 10, 1});
  go = false;


  /*auto flowEventGenerated = false;
    auto computeFlow = tarsier::make_computeFlow<Event, FlowEvent, 304, 240, 2, 10, 1000000>(
        [](Event event, double vx, double vy) -> FlowEvent {
            return FlowEvent{event.x, event.y, event.timestamp, vx, vy};
        },
        [&flowEventGenerated](FlowEvent flowEvent) -> void {
            flowEventGenerated = true;
            REQUIRE(std::abs(flowEvent.vx - 0.000090472103004291901) < std::numeric_limits<double>::epsilon());
            REQUIRE(std::abs(flowEvent.vy - 0.00023201716738197433) < std::numeric_limits<double>::epsilon());
        }
    );
    computeFlow(Event{100 - 2, 100 - 2, 2000000});
    computeFlow(Event{100 - 1, 100 - 2, 2001000});
    computeFlow(Event{100 - 0, 100 - 2, 2002000});
    computeFlow(Event{100 - 2, 100 - 1, 2003000});
    computeFlow(Event{100 + 1, 100 - 2, 2004000});
    computeFlow(Event{100 - 1, 100 - 1, 2005000});
    computeFlow(Event{100 - 0, 100 - 1, 2006000});
    computeFlow(Event{100 - 2, 100 - 0, 2007000});
    computeFlow(Event{100 + 1, 100 - 1, 2008000});
    computeFlow(Event{100, 100, 2010000});
    REQUIRE(flowEventGenerated);*/
}

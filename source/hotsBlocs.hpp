#pragma once

#include <iostream>
#include <algorithm>
#include <cstdint>
#include <cmath>
#include <utility>
#include <vector>

/// tarsier is a collection of event handlers.
namespace tarsier {

  /// Generic TimeSurfaceGenerator, pure virtual
  template<
    int64_t memorySize,
    int64_t contextSize,
    int64_t initMemory,
    typename Event,
    typename TimeSurfaceEvent,
    typename Kernel, // double f(Event, double neighbor)
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
      _handlerTimeSurfaceGenerator(std::forward<HandlerTimeSurfaceGenerator>(handlerTimeSurfaceGenerator)),
      _memory(memorySize,initMemory),
      _context(contextSize,0.)
    {}

    virtual ~TimeSurfaceGenerator(){}

    virtual void operator()(Event ev)=0;

  protected:
    Kernel _kernel;
    TimeSurfaceEventFromEvent _timeSurfaceEventFromEvent;
    HandlerTimeSurfaceGenerator _handlerTimeSurfaceGenerator;

    std::vector<int64_t> _memory;
    std::vector<double> _context;
  };

  /// 1D TimeSurfaceGenerator
  template<
    int64_t X,
    int64_t nP,
    int64_t radius,
    int64_t initMemory,
    typename Event, // require at least a field .t .x .p
    typename TimeSurfaceEvent,
    typename Kernel, // double f(Event, double neighbor)
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
            this->_context[cpt++] = this->_kernel(ev,static_cast<double>(this->_memory[p*X+x]));
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
    typename Kernel, // double f(Event, double neighbor)
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
      this->_memory[ev.p*X(ev.x*Y+ev.y)] = static_cast<int64_t>(ev.t);
      uint64_t cpt = 0;

      for(int64_t p = 0; p < nP; p++){
        for(int64_t x = static_cast<int64_t>(ev.x)-radius;
            x <= static_cast<int64_t>(ev.x)+radius;
            x++){
          for(int64_t y = static_cast<int64_t>(ev.y)-radius;
              y <= static_cast<int64_t>(ev.y)+radius;
              y++){
            if(x >= 0 && x < X & y >= 0 && y < Y){
              this->_context[cpt++] = this->_kernel(ev,static_cast<double>(this->_memory[p*X*(x*Y+y)]));
            }else{
              this->_context[cpt++] = 0;
            }
          }
        }

        this->_handlerTimeSurfaceGenerator(this->_timeSurfaceEventFromEvent(ev,this->_context));
      }
    }
  };

  //------------------------------------------------------------------------------------------\\
  /// IiwkClustering

  template<
    uint64_t nCenters,
    std::size_t neighborhood,
    typename Event, // require at least a field .context
    typename IiwkClusteringEvent,
    typename IiwkClusteringMetric, // double f(std::vector<double> iterator center begin,std::vector<double> iterator center end,std::vector<double> iterator context begin)
    typename IiwkClusteringEventFromEvent, // IiwkClusteringEvent f(Event,out_polarity)
    typename HandlerIiwkClustering // void f(IiwkClusteringEvent)
    >
  class IiwkClustering{
  public:
    IiwkClustering(double ksi,
                   double npow,
                   IiwkClusteringMetric iiwkClusteringMetric,
                   IiwkClusteringEventFromEvent iiwkClusteringEventFromEvent,
                   HandlerIiwkClustering handlerIiwkClustering):
      _ksi(ksi),
      _npow(npow),
      _sumOfDistances(0.),
      _iiwkClusteringMetric(std::forward<IiwkClusteringMetric>(iiwkClusteringMetric)),
      _iiwkClusteringEventFromEvent(std::forward<IiwkClusteringEventFromEvent>(iiwkClusteringEventFromEvent)),
      _handlerIiwkClustering(std::forward<HandlerIiwkClustering>(handlerIiwkClustering)),
      _distances(nCenters,0.),
      _centers(nCenters,std::vector<double>(neighborhood,1./static_cast<double>(neighborhood)))
    {}

    virtual ~IiwkClustering(){}
    virtual double& getKsi(){
      return _ksi;
    }
    virtual double& getNpow(){
      return _npow;
    }

    virtual void operator()(Event ev){
      /// Clustering
      double minimum;
      int64_t out_p;
      _sumOfDistances = 0.;

      auto context = ev.context;
      auto sumOfContext = std::accumulate(context.begin(), context.end(), 0.);
      for(auto&& it: context){
        it/=sumOfContext;
      }

      for(uint64_t i = 0; i < nCenters; i++){
        _distances[i] = _iiwkClusteringMetric(_centers[i].begin(),
                                              _centers[i].end(),
                                              context.begin()
                                              );

        if(i == 0 || minimum > _distances[i]){
          minimum = _distances[i];
          out_p = i;
        }
        _sumOfDistances+=_distances[i];
      }

      /// Update
      double coeff = _ksi*(
                           (_npow+1)*std::pow(_distances[out_p],_npow-1)
                           +_npow*std::pow(_distances[out_p], _npow-2)*(_sumOfDistances-_distances[out_p])
                           );

      auto sumOfCenter = 0.;
      auto cpt = 0;
      for(auto&& it: _centers[out_p]){
        it+=coeff*(context[cpt++]-it);
        if(it < 0){
          it = 0;
        }else{
          sumOfCenter+=it;
        }
      }

      for(auto&& it: _centers[out_p]){
        it/=sumOfCenter;
      }

      /// Send
      _handlerIiwkClustering(_iiwkClusteringEventFromEvent(ev, out_p));
    }

  protected:
    double _ksi;
    double _npow;
    double _sumOfDistances;
    IiwkClusteringMetric _iiwkClusteringMetric;
    IiwkClusteringEventFromEvent _iiwkClusteringEventFromEvent;
    HandlerIiwkClustering _handlerIiwkClustering;

    std::vector<double> _distances;
    std::vector<std::vector<double> > _centers;
  };

  //------------------------------------------------------------------------------------------\\
  /// StandardClustering

  template<
    uint64_t nCenters,
    std::size_t neighborhood,
    typename Event, // require at least a field .context
    typename StandardClusteringEvent,
    typename StandardClusteringMetric, // double f(std::vector<double> iterator center begin,std::vector<double> iterator center end,std::vector<double> iterator context begin)
    typename StandardClusteringEventFromEvent, // IiwkClusteringEvent f(Event,out_polarity)
    typename HandlerStandardClustering // void f(IiwkClusteringEvent)
    >
  class StandardClustering{
  public:
    StandardClustering(double baseLearningRate,
                       double baseLearningActivity,
                       StandardClusteringMetric standardClusteringMetric,
                       StandardClusteringEventFromEvent standardClusteringEventFromEvent,
                       HandlerStandardClustering handlerStandardClustering):
      _baseLearningRate(baseLearningRate),
      _baseLearningActivity(baseLearningActivity),
      _standardClusteringMetric(std::forward<StandardClusteringMetric>(standardClusteringMetric)),
      _standardClusteringEventFromEvent(std::forward<StandardClusteringEventFromEvent>(standardClusteringEventFromEvent)),
      _handlerStandardClustering(std::forward<HandlerStandardClustering>(handlerStandardClustering)),
      _distances(nCenters,0.),
      _centers(nCenters,std::vector<double>(neighborhood,1./static_cast<double>(neighborhood))),
      _activity(nCenters,0)
    {}

    virtual ~StandardClustering() {}

    virtual void operator()(Event ev){
      /// Clustering
      double minimum;
      int64_t out_p;

      auto context = ev.context;
      auto sumOfContext = std::accumulate(context.begin(), context.end(), 0.);
      // auto power = sumOfContext/static_cast<double>(context.size());
      for(auto&& it: context){
        it/=sumOfContext;
      }

      for(uint64_t i = 0; i < nCenters; i++){
        _distances[i] = _standardClusteringMetric(_centers[i].begin(),
                                                  _centers[i].end(),
                                                  context.begin()
                                                  );

        if(i == 0 || minimum > _distances[i]){
          minimum = _distances[i];
          out_p = i;
        }
      }
      _activity[out_p]++;

      /// Update
      auto cpt = 0;
      auto scal_prod = 0., scal_context = 0., scal_center = 0.;
      for(auto&& it: _centers[out_p]){
        scal_prod+=(it*context[cpt]);
        scal_center+=(it*it);
        scal_context+=(context[cpt]*context[cpt]);
        cpt++;
      }
      auto beta = scal_prod / sqrt(scal_center*scal_context);
      auto alpha = _baseLearningRate * (1 - _activity[out_p]/_baseLearningActivity);
      alpha = (alpha > 0.) ? alpha : 0.;

      auto sumOfCenter = 0.;
      cpt = 0;
      for(auto&& it: _centers[out_p]){
        it+=alpha*beta*(context[cpt++]-it);
        if(it < 0){
          it = 0;
        }else{
          sumOfCenter+=it;
        }
      }

      for(auto&& it: _centers[out_p]){
        it/=sumOfCenter;
      }

      /// Send
      _handlerStandardClustering(_standardClusteringEventFromEvent(ev, out_p));
    }

  protected:
    double _baseLearningRate;
    double _baseLearningActivity;
    StandardClusteringMetric _standardClusteringMetric;
    StandardClusteringEventFromEvent _standardClusteringEventFromEvent;
    HandlerStandardClustering _handlerStandardClustering;

    std::vector<double> _distances;
    std::vector<std::vector<double> > _centers;
    std::vector<int64_t> _activity;
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
    typename Kernel, // double f(Event, double neighbor)
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
    typename Kernel, // double f(Event, double neighbor)
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

  //------------------------------------------------------------------------------------------\\
  /// make_iiwkClustering

  template<
    uint64_t nCenters,
    std::size_t neighborhood,
    typename Event,
    typename IiwkClusteringEvent,
    typename IiwkClusteringMetric,
    typename IiwkClusteringEventFromEvent,
    typename HandlerIiwkClustering
    >
  IiwkClustering<nCenters,
                 neighborhood,
                 Event,
                 IiwkClusteringEvent,
                 IiwkClusteringMetric,
                 IiwkClusteringEventFromEvent,
                 HandlerIiwkClustering>
  make_iiwkClustering(double ksi,
                      double npow,
                      IiwkClusteringMetric iiwkClusteringMetric,
                      IiwkClusteringEventFromEvent iiwkClusteringEventFromEvent,
                      HandlerIiwkClustering handlerIiwkClustering)
  {
    return IiwkClustering<nCenters,
                          neighborhood,
                          Event,
                          IiwkClusteringEvent,
                          IiwkClusteringMetric,
                          IiwkClusteringEventFromEvent,
                          HandlerIiwkClustering>
      (ksi,
       npow,
       std::forward<IiwkClusteringMetric>(iiwkClusteringMetric),
       std::forward<IiwkClusteringEventFromEvent>(iiwkClusteringEventFromEvent),
       std::forward<HandlerIiwkClustering>(handlerIiwkClustering)
       );
  }
  //------------------------------------------------------------------------------------------\\
  /// make_standardClustering

  template<
    uint64_t nCenters,
    std::size_t neighborhood,
    typename Event, // require at least a field .context
    typename StandardClusteringEvent,
    typename StandardClusteringMetric, // double f(std::vector<double> iterator center begin,std::vector<double> iterator center end,std::vector<double> iterator context begin)
    typename StandardClusteringEventFromEvent, // IiwkClusteringEvent f(Event,out_polarity)
    typename HandlerStandardClustering // void f(IiwkClusteringEvent)
    >
  StandardClustering<nCenters,
                     neighborhood,
                     Event,
                     StandardClusteringEvent,
                     StandardClusteringMetric,
                     StandardClusteringEventFromEvent,
                     HandlerStandardClustering>
  make_standardClustering(double baseLearningRate,
                          double baseLearningActivity,
                          StandardClusteringMetric standardClusteringMetric,
                          StandardClusteringEventFromEvent standardClusteringEventFromEvent,
                          HandlerStandardClustering handlerStandardClustering)
  {
    return StandardClustering<nCenters,
                              neighborhood,
                              Event,
                              StandardClusteringEvent,
                              StandardClusteringMetric,
                              StandardClusteringEventFromEvent,
                              HandlerStandardClustering>
      (baseLearningRate,
       baseLearningActivity,
       std::forward<StandardClusteringMetric>(standardClusteringMetric),
       std::forward<StandardClusteringEventFromEvent>(standardClusteringEventFromEvent),
       std::forward<HandlerStandardClustering>(handlerStandardClustering)
       );
  }

};

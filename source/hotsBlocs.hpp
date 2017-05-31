#pragma once

#include <iostream>
#include <algorithm>
#include <cstdint>
#include <cmath>
#include <utility>
#include <vector>

/// tarsier is a collection of event handlers.
namespace tarsier {

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

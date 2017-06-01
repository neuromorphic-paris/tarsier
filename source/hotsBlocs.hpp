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
    IiwkClustering(double ksi1,
                   double ksi2,
                   double npow,
                   IiwkClusteringMetric iiwkClusteringMetric,
                   IiwkClusteringEventFromEvent iiwkClusteringEventFromEvent,
                   HandlerIiwkClustering handlerIiwkClustering):
      _ksi1(ksi1),
      _ksi2(ksi2),
      _npow(npow),
      _sumOfDistances(0.),
      _iiwkClusteringMetric(std::forward<IiwkClusteringMetric>(iiwkClusteringMetric)),
      _iiwkClusteringEventFromEvent(std::forward<IiwkClusteringEventFromEvent>(iiwkClusteringEventFromEvent)),
      _handlerIiwkClustering(std::forward<HandlerIiwkClustering>(handlerIiwkClustering)),
      _distances(nCenters,0.),
      _centers(nCenters,std::vector<double>(neighborhood,0.5)),
      _sumOfCenters(nCenters,0.5*neighborhood)
    {}

    virtual ~IiwkClustering(){}
    virtual double& getKsi1(){
      return _ksi2;
    }
    virtual double& getKsi2(){
      return _ksi2;
    }
    virtual double& getNpow(){
      return _npow;
    }
    virtual std::vector<std::vector<double> > getCenters() const{
      return _centers;
    }
    virtual void setCenters(std::vector<std::vector<double> > newCenters){
      if(newCenters.size() != _centers.size()){
        std::cout << "Error: waiting for centers of size " << _centers.size() << " instead of " << newCenters.size() << std::endl;
      }else if(newCenters[0].size() != _centers[0].size()){
        std::cout << "Error: waiting for centers of neighborhood size " << _centers[0].size() << " instead of " << newCenters[0].size() << std::endl;
      }
      _centers = newCenters;
      auto cpt = 0;
      for(auto&& it: _centers){
        _sumOfCenters[cpt] = 0.;
        for(auto&& it2: it){
          _sumOfCenters[cpt]+=it2;
        }
        cpt++;
      }
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
        auto curCenter = _centers[i];
        for(auto&& it: curCenter){
          it/=_sumOfCenters[i];
        }
        _distances[i] = _iiwkClusteringMetric(curCenter.begin(),
                                              curCenter.end(),
                                              context.begin()
                                              );

        if(i == 0 || minimum > _distances[i]){
          minimum = _distances[i];
          out_p = i;
        }
        _sumOfDistances+=_distances[i];
      }

      /// Update
      if(minimum != 0){
        double coeff = _ksi1*(
                              (_npow+1)*std::pow(minimum,_npow-1)
                              +_npow*std::pow(minimum, _npow-2)*(_sumOfDistances-minimum)
                              );
        coeff = (coeff > 1) ? 1: coeff;
        double curCoeff;

        for(int64_t i = 0; i < nCenters; i++){
          if(i != out_p){
            curCoeff = _ksi2*std::pow(minimum,_npow)/_distances[i];
          }else{
            curCoeff = coeff;
          }
          auto sumOfCenter = 0.;
          auto cpt = 0;
          for(auto&& it: _centers[i]){
            it+=curCoeff*(ev.context[cpt]-it);
            if(it <= 0){
              it = 0;
            }else{
              if(it > 1){
                it = 1;
              }
            }
            sumOfCenter+=it;
            cpt++;
          }
          _sumOfCenters[i] = sumOfCenter;
        }
      }

      /// Send
      _handlerIiwkClustering(_iiwkClusteringEventFromEvent(ev, out_p));
    }

  protected:
    double _ksi1;
    double _ksi2;
    double _npow;
    double _sumOfDistances;
    IiwkClusteringMetric _iiwkClusteringMetric;
    IiwkClusteringEventFromEvent _iiwkClusteringEventFromEvent;
    HandlerIiwkClustering _handlerIiwkClustering;

    std::vector<double> _distances;
    std::vector<std::vector<double> > _centers;
    std::vector<double> _sumOfCenters;
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
      _centers(nCenters,std::vector<double>(neighborhood,0.5)),
      _activity(nCenters,0),
      _sumOfCenters(nCenters,0.5*neighborhood),
      _first(0)
    {}

    virtual ~StandardClustering() {}
    virtual double& getBaseLearningRate(){
      return _baseLearningRate;
    }
    virtual double& getBaseLearningActivity(){
      return _baseLearningActivity;
    }
    virtual std::vector<std::vector<double> > getCenters() const{
      return _centers;
    }
    virtual void setCenters(std::vector<std::vector<double> > newCenters){
      if(newCenters.size() != _centers.size()){
        std::cout << "Error: waiting for centers of size " << _centers.size() << " instead of " << newCenters.size() << std::endl;
      }else if(newCenters[0].size() != _centers[0].size()){
        std::cout << "Error: waiting for centers of neighborhood size " << _centers[0].size() << " instead of " << newCenters[0].size() << std::endl;
      }
      _centers = newCenters;
      auto cpt = 0;
      for(auto&& it: _centers){
        _sumOfCenters[cpt] = 0.;
        for(auto&& it2: it){
          _sumOfCenters[cpt]+=it2;
        }
        cpt++;
      }
    }

    virtual void operator()(Event ev){
      /// Clustering
      double minimum;
      int64_t out_p;

      if(_first >= nCenters){
        auto context = ev.context;
        auto sumOfContext = std::accumulate(context.begin(), context.end(), 0.);
        for(auto&& it: context){
          it/=sumOfContext;
        }
        for(uint64_t i = 0; i < nCenters; i++){
          auto curCenter = _centers[i];
          for(auto&& it: curCenter){
            it/=_sumOfCenters[i];
          }
          _distances[i] = _standardClusteringMetric(curCenter.begin(),
                                                    curCenter.end(),
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
          scal_prod+=(it*ev.context[cpt]);
          scal_center+=(it*it);
          scal_context+=(ev.context[cpt]*ev.context[cpt]);
          cpt++;
        }
        auto beta = scal_prod / sqrt(scal_center*scal_context);
        auto alpha = _baseLearningRate * (1 - _activity[out_p]/_baseLearningActivity);
        alpha = (alpha > 0.) ? alpha : 0.;

        auto sumOfCenter = 0.;
        cpt = 0;
        for(auto&& it: _centers[out_p]){
          it+=alpha*beta*(ev.context[cpt++]-it);
          if(it < 0){
            it = 0;
          }else{
            if(it > 1){
              it = 1;
            }
            sumOfCenter+=it;
          }
        }
        _sumOfCenters[out_p] = sumOfCenter;
      }else{
        out_p = _first++;
        auto cpt = 0;
        _sumOfCenters[out_p] = 0.;
        for(auto&& it: _centers[out_p]){
          it = ev.context[cpt++];
          _sumOfCenters[out_p]+=it;
        }
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
    std::vector<double> _sumOfCenters;
    int _first;
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
  make_iiwkClustering(double ksi1,
                      double ksi2,
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
      (ksi1,
       ksi2,
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

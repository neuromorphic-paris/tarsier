#pragma once

#include <iostream>
#include <algorithm>
#include <cstdint>
#include <cmath>
#include <utility>
#include <array>
#include <numeric>

/// tarsier is a collection of event handlers.
namespace tarsier {

  /// IiwkCluster
  template<
    uint64_t nCenters,
    std::size_t neighborhood,
    bool normalize,
    bool isLearning,
    typename Event, // require at least a field .context
    typename IiwkClusterEvent,
    typename IiwkClusterMetric, // double f(std::array<double,neighborhood> iterator center begin,std::array<double,neighborhood> iterator center end,std::array<double,neighborhood> iterator context begin)
    typename IiwkClusterEventFromEvent, // IiwkClusterEvent f(Event,out_polarity)
    typename HandlerIiwkCluster // void f(IiwkClusterEvent)
    >
  class IiwkCluster{
  public:
    IiwkCluster(double ksi1,
                double ksi2,
                double npow,
                IiwkClusterMetric iiwkClusterMetric,
                IiwkClusterEventFromEvent iiwkClusterEventFromEvent,
                HandlerIiwkCluster handlerIiwkCluster):
      _ksi1(ksi1),
      _ksi2(ksi2),
      _npow(npow),
      _sumOfDistances(0.),
      _iiwkClusterMetric(std::forward<IiwkClusterMetric>(iiwkClusterMetric)),
      _iiwkClusterEventFromEvent(std::forward<IiwkClusterEventFromEvent>(iiwkClusterEventFromEvent)),
      _handlerIiwkCluster(std::forward<HandlerIiwkCluster>(handlerIiwkCluster))
    {
      for(auto&& it: _distances){
        it = 0.;
      }
      for(auto&& it: _centers){
        for(auto&& it2: it){
          it2 = 0.5;
        }
      }
      for(auto&& it: _sumOfCenters){
        it = 0.5*neighborhood;
      }
    }

    IiwkCluster(double ksi1,
                double ksi2,
                double npow,
                std::array<std::array<double, neighborhood>, nCenters> centers,
                IiwkClusterMetric iiwkClusterMetric,
                IiwkClusterEventFromEvent iiwkClusterEventFromEvent,
                HandlerIiwkCluster handlerIiwkCluster):
      IiwkCluster<nCenters,
                  neighborhood,
                  normalize,
                  isLearning,
                  Event,
                  IiwkClusterEvent,
                  IiwkClusterMetric,
                  IiwkClusterEventFromEvent,
                  HandlerIiwkCluster>
    (ksi1,
     ksi2,
     npow,
     std::forward<IiwkClusterMetric>(iiwkClusterMetric),
     std::forward<IiwkClusterEventFromEvent>(iiwkClusterEventFromEvent),
     std::forward<HandlerIiwkCluster>(handlerIiwkCluster))
    {
      auto cpt = 0;
      for(auto&& it: _centers){
        for(auto&& it2: _centers){
          it2 = centers[cpt++];
        }
      }
    }

    virtual ~IiwkCluster(){}

    /*virtual void setCenters(std::vector<std::array<double,neighborhood> > newCenters){
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
      }*/

    virtual void operator()(Event ev){
      /// Cluster
      double minimum;
      int64_t out_p;
      _sumOfDistances = 0.;

      auto context = ev.context;
      if(normalize){
        auto sumOfContext = std::accumulate(context.begin(), context.end(), 0.);
        for(auto&& it: context){
          it/=sumOfContext;
        }
      }

      for(uint64_t i = 0; i < nCenters; i++){
        auto curCenter = _centers[i];
        if(normalize){
          for(auto&& it: curCenter){
            it/=_sumOfCenters[i];
          }
        }
        _distances[i] = _iiwkClusterMetric(curCenter.begin(),
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
      if(isLearning){
        if(minimum != 0){
          double coeff = _ksi1*(
                                (_npow+1)*std::pow(minimum,_npow-1)
                                +_npow*std::pow(minimum, _npow-2)*(_sumOfDistances-minimum)
                                );
          coeff = (coeff > 1) ? 1: coeff;
          double curCoeff;

          for(uint64_t i = 0; i < nCenters; i++){
            if(i != out_p){
              curCoeff = _ksi2*std::pow(minimum,_npow)/_distances[i];
            }else{
              curCoeff = coeff;
            }
            if(normalize){
              _sumOfCenters[i] = 0.;
            }
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
              if(normalize){
                _sumOfCenters[i]+=it;
              }
              cpt++;
            }
          }
        }
      }

      /// Send
      _handlerIiwkCluster(_iiwkClusterEventFromEvent(ev, out_p));
    }

  protected:
    double _ksi1;
    double _ksi2;
    double _npow;
    double _sumOfDistances;
    IiwkClusterMetric _iiwkClusterMetric;
    IiwkClusterEventFromEvent _iiwkClusterEventFromEvent;
    HandlerIiwkCluster _handlerIiwkCluster;

    std::array<std::array<double, neighborhood>, nCenters> _centers;
    std::array<double, nCenters> _distances;
    std::array<double, nCenters> _sumOfCenters;
  };

  //------------------------------------------------------------------------------------------\\
  /// StdCluster

  template<
    uint64_t nCenters,
    std::size_t neighborhood,
    bool normalize,
    bool isLearning,
    typename Event, // require at least a field .context
    typename StdClusterEvent,
    typename StdClusterMetric, // double f(std::array<double,neighborhood> iterator center begin,std::array<double,neighborhood> iterator center end,std::array<double,neighborhood> iterator context begin)
    typename StdClusterEventFromEvent, // IiwkClusterEvent f(Event,out_polarity)
    typename HandlerStdCluster // void f(IiwkClusterEvent)
    >
  class StdCluster{
  public:
    StdCluster(double baseLearningRate,
               double baseLearningActivity,
               StdClusterMetric stdClusterMetric,
               StdClusterEventFromEvent stdClusterEventFromEvent,
               HandlerStdCluster handlerStdCluster):
      _baseLearningRate(baseLearningRate),
      _baseLearningActivity(baseLearningActivity),
      _stdClusterMetric(std::forward<StdClusterMetric>(stdClusterMetric)),
      _stdClusterEventFromEvent(std::forward<StdClusterEventFromEvent>(stdClusterEventFromEvent)),
      _handlerStdCluster(std::forward<HandlerStdCluster>(handlerStdCluster)),
      _first(0)
    {
      for(auto&& it: _distances){
        it = 0.;
      }
      for(auto&& it: _centers){
        for(auto&& it2: it){
          it2 = 0.5;
        }
      }
      for(auto&& it: _activity){
        it = 0;
      }
      for(auto&& it: _sumOfCenters){
        it = 0.5*neighborhood;
      }
    }

    StdCluster(double baseLearningRate,
               double baseLearningActivity,
               std::array<std::array<double, neighborhood>, nCenters> centers,
               StdClusterMetric stdClusterMetric,
               StdClusterEventFromEvent stdClusterEventFromEvent,
               HandlerStdCluster handlerStdCluster):
      StdCluster<nCenters,
                 neighborhood,
                 normalize,
                 isLearning,
                 Event,
                 StdClusterEvent,
                 StdClusterMetric,
                 StdClusterEventFromEvent,
                 HandlerStdCluster>
    (baseLearningRate,
     baseLearningActivity,
     std::forward<StdClusterMetric>(stdClusterMetric),
     std::forward<StdClusterEventFromEvent>(stdClusterEventFromEvent),
     std::forward<HandlerStdCluster>(handlerStdCluster))
    {
      auto cpt = 0;
      for(auto&& it: _centers){
        for(auto&& it2: _centers){
          it2 = centers[cpt++];
        }
      }
    }

    virtual ~StdCluster() {}

    /*virtual void setCenters(std::vector<std::array<double,neighborhood> > newCenters){
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
      }*/

    virtual void operator()(Event ev){
      /// Cluster
      double minimum;
      int64_t out_p;

      if(isLearning == false || _first >= nCenters){
        auto context = ev.context;
        if(normalize){
          auto sumOfContext = std::accumulate(context.begin(), context.end(), 0.);
          for(auto&& it: context){
            it/=sumOfContext;
          }
        }
        for(uint64_t i = 0; i < nCenters; i++){
          auto curCenter = _centers[i];
          if(normalize){
            for(auto&& it: curCenter){
              it/=_sumOfCenters[i];
            }
          }
          _distances[i] = _stdClusterMetric(curCenter.begin(),
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
        if(isLearning){
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

          if(normalize){
            _sumOfCenters[out_p] = 0.;
          }
          cpt = 0;
          for(auto&& it: _centers[out_p]){
            it+=alpha*beta*(ev.context[cpt++]-it);
            if(it < 0){
              it = 0;
            }else{
              if(it > 1){
                it = 1;
              }
              if(normalize){
                _sumOfCenters[out_p]+=it;
              }
            }
          }
        }
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
      _handlerStdCluster(_stdClusterEventFromEvent(ev, out_p));
    }

  protected:
    double _baseLearningRate;
    double _baseLearningActivity;
    StdClusterMetric _stdClusterMetric;
    StdClusterEventFromEvent _stdClusterEventFromEvent;
    HandlerStdCluster _handlerStdCluster;

    std::array<std::array<double, neighborhood>, nCenters> _centers;
    std::array<double, nCenters> _distances;
    std::array<double, nCenters> _sumOfCenters;
    std::array<int64_t, nCenters> _activity;
    int _first;
  };

  //------------------------------------------------------------------------------------------\\
  /// make_iiwkCluster

  /// Contructor without centers
  template<
    uint64_t nCenters,
    std::size_t neighborhood,
    bool normalize,
    bool isLearning,
    typename Event,
    typename IiwkClusterEvent,
    typename IiwkClusterMetric,
    typename IiwkClusterEventFromEvent,
    typename HandlerIiwkCluster
    >
  IiwkCluster<nCenters,
              neighborhood,
              normalize,
              isLearning,
              Event,
              IiwkClusterEvent,
              IiwkClusterMetric,
              IiwkClusterEventFromEvent,
              HandlerIiwkCluster>
  make_iiwkCluster(double ksi1,
                   double ksi2,
                   double npow,
                   IiwkClusterMetric iiwkClusterMetric,
                   IiwkClusterEventFromEvent iiwkClusterEventFromEvent,
                   HandlerIiwkCluster handlerIiwkCluster)
  {
    return IiwkCluster<nCenters,
                       neighborhood,
                       normalize,
                       isLearning,
                       Event,
                       IiwkClusterEvent,
                       IiwkClusterMetric,
                       IiwkClusterEventFromEvent,
                       HandlerIiwkCluster>
      (ksi1,
       ksi2,
       npow,
       std::forward<IiwkClusterMetric>(iiwkClusterMetric),
       std::forward<IiwkClusterEventFromEvent>(iiwkClusterEventFromEvent),
       std::forward<HandlerIiwkCluster>(handlerIiwkCluster)
       );
  }

  /// Contructor with centers
  template<
    uint64_t nCenters,
    std::size_t neighborhood,
    bool normalize,
    bool isLearning,
    typename Event,
    typename IiwkClusterEvent,
    typename IiwkClusterMetric,
    typename IiwkClusterEventFromEvent,
    typename HandlerIiwkCluster
    >
  IiwkCluster<nCenters,
              neighborhood,
              normalize,
              isLearning,
              Event,
              IiwkClusterEvent,
              IiwkClusterMetric,
              IiwkClusterEventFromEvent,
              HandlerIiwkCluster>
  make_iiwkCluster(double ksi1,
                   double ksi2,
                   double npow,
                   std::array<std::array<double, neighborhood>, nCenters> centers,
                   IiwkClusterMetric iiwkClusterMetric,
                   IiwkClusterEventFromEvent iiwkClusterEventFromEvent,
                   HandlerIiwkCluster handlerIiwkCluster)
  {
    return IiwkCluster<nCenters,
                       neighborhood,
                       normalize,
                       isLearning,
                       Event,
                       IiwkClusterEvent,
                       IiwkClusterMetric,
                       IiwkClusterEventFromEvent,
                       HandlerIiwkCluster>
      (ksi1,
       ksi2,
       npow,
       centers,
       std::forward<IiwkClusterMetric>(iiwkClusterMetric),
       std::forward<IiwkClusterEventFromEvent>(iiwkClusterEventFromEvent),
       std::forward<HandlerIiwkCluster>(handlerIiwkCluster)
       );
  }
  //------------------------------------------------------------------------------------------\\
  /// make_stdCluster

  /// Contructor without centers
  template<
    uint64_t nCenters,
    std::size_t neighborhood,
    bool normalize,
    bool isLearning,
    typename Event, // require at least a field .context
    typename StdClusterEvent,
    typename StdClusterMetric, // double f(std::array<double,neighborhood> iterator center begin,std::array<double,neighborhood> iterator center end,std::array<double,neighborhood> iterator context begin)
    typename StdClusterEventFromEvent, // IiwkClusterEvent f(Event,out_polarity)
    typename HandlerStdCluster // void f(IiwkClusterEvent)
    >
  StdCluster<nCenters,
             neighborhood,
             normalize,
             isLearning,
             Event,
             StdClusterEvent,
             StdClusterMetric,
             StdClusterEventFromEvent,
             HandlerStdCluster>
  make_stdCluster(double baseLearningRate,
                  double baseLearningActivity,
                  StdClusterMetric stdClusterMetric,
                  StdClusterEventFromEvent stdClusterEventFromEvent,
                  HandlerStdCluster handlerStdCluster)
  {
    return StdCluster<nCenters,
                      neighborhood,
                      normalize,
                      isLearning,
                      Event,
                      StdClusterEvent,
                      StdClusterMetric,
                      StdClusterEventFromEvent,
                      HandlerStdCluster>
      (baseLearningRate,
       baseLearningActivity,
       std::forward<StdClusterMetric>(stdClusterMetric),
       std::forward<StdClusterEventFromEvent>(stdClusterEventFromEvent),
       std::forward<HandlerStdCluster>(handlerStdCluster)
       );
  }

  /// Contructor with centers
  template<
    uint64_t nCenters,
    std::size_t neighborhood,
    bool normalize,
    bool isLearning,
    typename Event, // require at least a field .context
    typename StdClusterEvent,
    typename StdClusterMetric, // double f(std::array<double,neighborhood> iterator center begin,std::array<double,neighborhood> iterator center end,std::array<double,neighborhood> iterator context begin)
    typename StdClusterEventFromEvent, // IiwkClusterEvent f(Event,out_polarity)
    typename HandlerStdCluster // void f(IiwkClusterEvent)
    >
  StdCluster<nCenters,
             neighborhood,
             normalize,
             isLearning,
             Event,
             StdClusterEvent,
             StdClusterMetric,
             StdClusterEventFromEvent,
             HandlerStdCluster>
  make_stdCluster(double baseLearningRate,
                  double baseLearningActivity,
                  std::array<std::array<double, neighborhood>, nCenters> centers,
                  StdClusterMetric stdClusterMetric,
                  StdClusterEventFromEvent stdClusterEventFromEvent,
                  HandlerStdCluster handlerStdCluster)
  {
    return StdCluster<nCenters,
                      neighborhood,
                      normalize,
                      isLearning,
                      Event,
                      StdClusterEvent,
                      StdClusterMetric,
                      StdClusterEventFromEvent,
                      HandlerStdCluster>
      (baseLearningRate,
       baseLearningActivity,
       centers,
       std::forward<StdClusterMetric>(stdClusterMetric),
       std::forward<StdClusterEventFromEvent>(stdClusterEventFromEvent),
       std::forward<HandlerStdCluster>(handlerStdCluster)
       );
  }
};

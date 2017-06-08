#pragma once

#include <vector>
#include <iostream>

namespace tarsier{
  template<typename Signature,
           typename ClusterSignaturesMetric,
           typename HandlerClusterSignatures>
  class ClusterSignatures{
  public:
    ClusterSignatures(ClusterSignaturesMetric clusterSignaturesMetric,
                     HandlerClusterSignatures handlerClusterSignatures):
      _clusterSignaturesMetric(std::forward<ClusterSignaturesMetric>(clusterSignaturesMetric)),
      _handlerClusterSignatures(std::forward<HandlerClusterSignatures>(handlerClusterSignatures)),
      _listOfSignatures(0),
      _distances(0),
      _groups(0)
    {}

    virtual ~ClusterSignatures(){}

    virtual void operator()(Signature inputSignature){
      _listOfSignatures.push_back(inputSignature);
      _groups.push_back(_listOfSignatures.size());
      _distances.push_back(std::vector<double>(_listOfSignatures.size(),0.));
      for(std::size_t i = 0; i < _listOfSignatures.size()-1; i++){
        double distanceResult = static_cast<double>(_clusterSignaturesMetric(inputSignature, _listOfSignatures[i]));
        _distances[_listOfSignatures.size()-1][i] = distanceResult;
        _distances[i].push_back(distanceResult);
      }

      _handlerClusterSignatures(_distances);
    }

  protected:
    HandlerClusterSignatures _handlerClusterSignatures;
    ClusterSignaturesMetric _clusterSignaturesMetric;

    std::vector<Signature> _listOfSignatures;
    std::vector<std::vector<double> > _distances;
    std::vector<int64_t> _groups;
  };

  template<typename Signature,
           typename ClusterSignaturesMetric,
           typename HandlerClusterSignatures>
  ClusterSignatures<Signature,
                   ClusterSignaturesMetric,
                   HandlerClusterSignatures>
  make_clusterSignatures(ClusterSignaturesMetric clusterSignaturesMetric,
                        HandlerClusterSignatures handlerClusterSignatures){
    return ClusterSignatures<Signature,
                            ClusterSignaturesMetric,
                            HandlerClusterSignatures>
      (std::forward<ClusterSignaturesMetric>(clusterSignaturesMetric),
       std::forward<HandlerClusterSignatures>(handlerClusterSignatures)
       );
  }
};

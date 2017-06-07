#pragma once

#include <vector>
#include <iostream>

namespace tarsier{
  template<typename Sequence,
           typename ClusterSequencesMetric,
           typename HandlerClusterSequences>
  class ClusterSequences{
  public:
    ClusterSequences(ClusterSequencesMetric clusterSequencesMetric,
                     HandlerClusterSequences handlerClusterSequences):
      _clusterSequencesMetric(std::forward<ClusterSequencesMetric>(clusterSequencesMetric)),
      _handlerClusterSequences(std::forward<HandlerClusterSequences>(handlerClusterSequences)),
      _listOfSequences(0),
      _distances(0)
    {}

    virtual ~ClusterSequences(){}

    virtual void operator()(Sequence inputSequence){
      _listOfSequences.push_back(inputSequence);
      _distances.push_back(std::vector<double>(_listOfSequences.size(),0.));
      for(std::size_t i = 0; i < _listOfSequences.size()-1; i++){
        double distanceResult = static_cast<double>(_clusterSequencesMetric(inputSequence, _listOfSequences[i]));
        _distances[_listOfSequences.size()-1][i] = distanceResult;
        _distances[i].push_back(distanceResult);
      }

      _handlerClusterSequences();
    }

  protected:
    HandlerClusterSequences _handlerClusterSequences;
    ClusterSequencesMetric _clusterSequencesMetric;

    std::vector<Sequence> _listOfSequences;
    std::vector<std::vector<double> > _distances;
  };

  template<typename Sequence,
           typename ClusterSequencesMetric,
           typename HandlerClusterSequences>
  ClusterSequences<Sequence,
                   ClusterSequencesMetric,
                   HandlerClusterSequences>
  make_clusterSequences(ClusterSequencesMetric clusterSequencesMetric,
                        HandlerClusterSequences handlerClusterSequences){
    return ClusterSequences<Sequence,
                            ClusterSequencesMetric,
                            HandlerClusterSequences>
      (std::forward<ClusterSequencesMetric>(clusterSequencesMetric),
       std::forward<HandlerClusterSequences>(handlerClusterSequences)
       );
  }
};

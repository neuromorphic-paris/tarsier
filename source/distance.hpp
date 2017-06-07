#pragma once

#include <vector>
#include <algorithm>

/// tarsier is a collection of event handler
namespace tarsier{
  /// computeLevenshteinDistance is able to estimate the edit distance between two sequences according the classic cost function
  template<typename FisrtContainer, // need at least operator[] that return a single value
           typename SecondContainer> // need at least operator[] that return a single value
  std::size_t computeLevenshteinDistance(FisrtContainer inputString,
                                         SecondContainer expectedString){
    std::vector<std::vector<std::size_t> > d(inputString.size()+1, std::vector<std::size_t>(expectedString.size()+1,0));
    std::size_t substituteCost = 0, addCost = 0, deleteCost = 0;
    for(std::size_t i = 0; i <= inputString.size(); i++){
      d[i][0] = i;
    }
    for(std::size_t i = 1; i <= expectedString.size(); i++){
      d[0][i] = i;
    }
    for(std::size_t i = 1; i <= inputString.size(); i++){
      for(std::size_t j = 1; j <= expectedString.size(); j++){
        substituteCost = d[i-1][j-1] + ((inputString[i-1] == expectedString[j-1]) ? 0 : 1);
        addCost = d[i][j-1] + 1;
        deleteCost = d[i-1][j] + 1;
        d[i][j] = (addCost < deleteCost) ? ((addCost < substituteCost) ? addCost : substituteCost) : ((deleteCost < substituteCost) ? deleteCost : substituteCost);
      }
    }
    return d[inputString.size()][expectedString.size()];
  }

  /// computeLevenshteinDistanceHeuristic is able to estimate the edit distance between two sequences according an heuristic that allow to be invariant to repetitions
  template<typename FisrtContainer, // need at least operator[] that return a single value
           typename SecondContainer> // need at least operator[] that return a single value
  std::size_t computeLevenshteinDistanceHeuristic(FisrtContainer inputString,
                                                    SecondContainer expectedString){
    std::vector<std::vector<std::size_t> > d(inputString.size(), std::vector<std::size_t>(expectedString.size(),0));
    d[0][0] = ((inputString[0] == expectedString[0]) ? 0 : 1);
    for(std::size_t i = 1; i < inputString.size(); i++){
      d[i][0] = d[i-1][0] + ((inputString[i] == expectedString[0]) ? 0 : 1);
    }
    for(std::size_t i = 1; i < expectedString.size(); i++){
      d[0][i] = d[0][i-1] + ((inputString[0] == expectedString[i]) ? 0 : 1);
    }
    for(std::size_t i = 1; i < inputString.size(); i++){
      for(std::size_t j = 1; j < expectedString.size(); j++){
        d[i][j]= ((inputString[i] == expectedString[j]) ? 0 : 1) + ((d[i][j-1] < d[i-1][j]) ? ((d[i][j-1] < d[i-1][j-1]) ? d[i][j-1] : d[i-1][j-1]) : ((d[i-1][j] < d[i-1][j-1]) ? d[i-1][j] : d[i-1][j-1]));
      }
    }
    return d[inputString.size()-1][expectedString.size()-1];
  }

  /// Implementation of euclidean distance better on vectors upper than 20 elems
  template<typename ContainerIterator,typename T>
  T computeEuclideanDistanceFast(ContainerIterator beg1,
                                 ContainerIterator end1,
                                 ContainerIterator beg2){
    return static_cast<T>(std::inner_product(beg1,
                                             end1,
                                             beg2,
                                             0.,
                                             [](T a, T b){
                                               return a+b;
                                             },
                                             [](T a, T b){
                                               return std::pow(a-b, 2);
                                             }));
  }

  /// Implementation of euclidean distance better on very small vectors
  template<typename ContainerIterator,typename T>
  T computeEuclideanDistance(ContainerIterator beg1,
                             ContainerIterator end1,
                             ContainerIterator beg2){
    T d = 0;
    for(ContainerIterator it1 = beg1, it2 = beg2;
        it1 != end1;
        ++it1, ++it2){
      d+=std::pow((*it1)-(*it2), 2);
    }
    return d;
  }

  /// Implementation of battacharya distance better on vectors upper than 15 elems
  template<typename ContainerIterator>
  class computeBattacharyaDistanceFast{
  public:
    computeBattacharyaDistanceFast(double limitOfInfinite,
                                   double limitOfZeros):
      _limitOfInfinite(limitOfInfinite),
      _limitOfZeros(limitOfZeros)
    {}

    virtual ~computeBattacharyaDistanceFast(){}

    virtual double operator()(ContainerIterator beg1,
                              ContainerIterator end1,
                              ContainerIterator beg2){
      double d = -log(std::inner_product(beg1,
                                         end1,
                                         beg2,
                                         0.,
                                         [](double a, double b){
                                           return a+b;
                                         },
                                         [](double a, double b){
                                           return sqrt(a*b);
                                         })
                      );
      return (std::isinf(d)) ? _limitOfInfinite : (d < _limitOfZeros) ? 0. : d;
    }

  protected:
    double _limitOfInfinite;
    double _limitOfZeros;
  };

  /// Implementation of battacharya distance better on very small vectors
  template<typename ContainerIterator>
  class computeBattacharyaDistance{
  public:
    computeBattacharyaDistance(double limitOfInfinite,
                               double limitOfZeros):
      _limitOfInfinite(limitOfInfinite),
      _limitOfZeros(limitOfZeros)
    {}

    virtual ~computeBattacharyaDistance(){}

    virtual double operator()(ContainerIterator beg1,
                              ContainerIterator end1,
                              ContainerIterator beg2){
      double d = 0.;
      for(ContainerIterator it1 = beg1, it2 = beg2;
          it1 != end1;
          ++it1, ++it2){
        d+=sqrt((*it1)*(*it2));
      }
      d = -log(d);
      return (std::isinf(d)) ? _limitOfInfinite : (d < _limitOfZeros) ? 0. : d;
    }

  protected:
    double _limitOfInfinite;
    double _limitOfZeros;
  };
};

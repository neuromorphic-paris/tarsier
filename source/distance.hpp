#pragma once

#include <vector>
#include <algorithm>
#include <iostream>

/// tarsier is a collection of event handler
namespace tarsier{
  /// computeLevenshteinDistance is able to estimate the edit distance between two sequences according the classic cost function
  template<typename FisrtContainer, // need at least operator[] that return a single value and .size()
           typename SecondContainer> // need at least operator[] that return a single value and .size()
  class computeLevenshteinDistance{
  public:
    computeLevenshteinDistance(double addCost=1,
                               double deleteCost=1,
                               double substituteCost=1):
      _addCost(addCost),
      _deleteCost(deleteCost),
      _substituteCost(substituteCost)
    {}

    virtual ~computeLevenshteinDistance(){}

    virtual double operator()(FisrtContainer inputString,
                              SecondContainer expectedString
                              ){
      std::vector<std::vector<double> > d(inputString.size()+1, std::vector<double>(expectedString.size()+1,0));
      double substituteElem = 0., addElem = 0., deleteElem = 0.;

      d[0][0] = 0;
      for(std::size_t i = 1; i <= inputString.size(); i++){
        d[i][0] = d[i-1][0]+_deleteCost;
      }
      for(std::size_t i = 1; i <= expectedString.size(); i++){
        d[0][i] = d[0][i-1]+_addCost;
      }
      for(std::size_t i = 1; i <= inputString.size(); i++){
        for(std::size_t j = 1; j <= expectedString.size(); j++){
          substituteElem = d[i-1][j-1] + ((inputString[i-1] == expectedString[j-1]) ? 0 : _substituteCost);
          addElem = d[i][j-1] + _addCost;
          deleteElem = d[i-1][j] + _deleteCost;
          d[i][j] = (addElem < deleteElem) ? ((addElem < substituteElem) ? addElem : substituteElem) : ((deleteElem < substituteElem) ? deleteElem : substituteElem);
        }
      }
      return d[inputString.size()][expectedString.size()];
    }

  protected:
    double _addCost;
    double _deleteCost;
    double _substituteCost;
  };

  /// computeLevenshteinDistanceHeuristic is able to estimate the edit distance between two sequences according an heuristic that allow to be invariant to repetitions
  template<typename FisrtContainer, // need at least operator[] that return a single value and .size()
           typename SecondContainer> // need at least operator[] that return a single value and .size()
  class computeLevenshteinDistanceHeuristic{
  public:
    computeLevenshteinDistanceHeuristic(double addCost=0,
                                        double deleteCost=0,
                                        double substituteCost=0):
      _addCost(addCost),
      _deleteCost(deleteCost),
      _substituteCost(substituteCost)
    {}

    virtual ~computeLevenshteinDistanceHeuristic(){}

    virtual double operator()(FisrtContainer inputString,
                              SecondContainer expectedString
                              ){
      std::vector<std::vector<double> > d(inputString.size(), std::vector<double>(expectedString.size(),0));
      double substituteElem = 0., addElem = 0., deleteElem = 0.;

      d[0][0] = ((inputString[0] == expectedString[0]) ? 0 : 1);
      for(std::size_t i = 1; i < inputString.size(); i++){
        d[i][0] = d[i-1][0] + ((inputString[i] == expectedString[0]) ? 0 : 1)+_deleteCost;
      }
      for(std::size_t i = 1; i < expectedString.size(); i++){
        d[0][i] = d[0][i-1] + ((inputString[0] == expectedString[i]) ? 0 : 1)+_addCost;
      }
      for(std::size_t i = 1; i < inputString.size(); i++){
        for(std::size_t j = 1; j < expectedString.size(); j++){
          substituteElem = d[i-1][j-1]+((inputString[i] == expectedString[j]) ? 0 : _substituteCost);
          addElem = d[i][j-1]+_addCost;
          deleteElem = d[i-1][j]+_deleteCost;
          d[i][j]= ((inputString[i] == expectedString[j]) ? 0 : 1) + ((addElem < deleteElem) ? ((addElem < substituteElem) ? addElem : substituteElem) : ((deleteElem < substituteElem) ? deleteElem : substituteElem));
        }
      }
      return d[inputString.size()-1][expectedString.size()-1];
    }

  protected:
    double _addCost;
    double _deleteCost;
    double _substituteCost;
  };

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
      /*if(d < 0){
        throw("Vectors are not normalized, negative distance");
      }*/
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

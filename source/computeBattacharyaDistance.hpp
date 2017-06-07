#pragma once

#include <algorithm>
#include <cstdlib>
#include <cmath>

namespace tarsier{
  template<typename Container,
           typename ContainerIterator>
  class computeBattacharyaDistance{
  public:
    computeBattacharyaDistance(double limitOfInfinite,
                               double limitOfZeros):
      _limitOfInfinite(limitOfInfinite),
      _limitOfZeros(limitOfZeros)
    {}

    virtual ~computeBattacharyaDistance(){}

    /// Better on vectors upper than 15 elems
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
      return (std::isinf(d)) ? _limitOfInfinite : (d < _limitOfZeros) ? 0 : d;
      }

    /// Better on very small vectors
    virtual double operator()(Container v1,
                              Container v2){
      double d = 0.;
      for(ContainerIterator it1 = v1.begin(), it2 = v2.begin();
          it1 != v1.end();
          ++it1, ++it2){
        d+=sqrt((*it1)*(*it2));
      }
      d = -log(d);
      return (std::isinf(d)) ? _limitOfInfinite : (d < _limitOfZeros) ? 0 : d;
    }

  protected:
    double _limitOfInfinite;
    double _limitOfZeros;
  };
};

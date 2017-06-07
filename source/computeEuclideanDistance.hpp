#pragma once
#include <algorithm>
#include <cstdlib>
#include <cmath>

namespace tarsier{
  template<typename Container,
           typename ContainerIterator>
  class computeEuclideanDistance{
  public:
    computeEuclideanDistance(){}

    virtual ~computeEuclideanDistance(){}

    /// Better on vectors upper than 20 elems
    virtual double operator()(ContainerIterator beg1,
                              ContainerIterator end1,
                              ContainerIterator beg2){
      return std::inner_product(beg1,
                                end1,
                                beg2,
                                0.,
                                [](double a, double b){
                                  return a+b;
                                },
                                [](double a, double b){
                                  return std::pow(a-b, 2);
                                });
    }

    /// Better on very small vectors
    virtual double operator()(Container v1,
                              Container v2){
      double d = 0.;
      for(ContainerIterator it1 = v1.begin(), it2 = v2.begin();
          it1 != v1.end();
          ++it1, ++it2){
        d+=std::pow((*it1)-(*it2), 2);
      }
      return d;
    }
  };
};

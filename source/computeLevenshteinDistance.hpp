#pragma once

#include <vector>
#include <iostream>

namespace tarsier{
  template<typename FisrtContainer,
           typename SecondContainer>
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

  template<typename FisrtContainer,
           typename SecondContainer>
  std::size_t computeLevenshteinDistanceHeuristique(FisrtContainer inputString,
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
};

#include "../source/distance.hpp"
#include <string>
#include <iostream>
#include <vector>

#include "catch.hpp"

TEST_CASE("Compute distances", "[Distance]") {
  tarsier::computeLevenshteinDistance<std::string, std::string> levenshtein(1,2,4);
  tarsier::computeLevenshteinDistanceHeuristic<std::string, std::string> levenshteinHeuristic(1,2,4);
  REQUIRE(levenshtein(std::string("abcde"), std::string("aabcdedee")) == 4);
  REQUIRE(levenshteinHeuristic(std::string("abcde"), std::string("aabcdedee")) == 5);

  std::vector<int> v1{2,3,4};
  std::vector<int> v2{5,2,1};
  REQUIRE((tarsier::computeEuclideanDistance<std::vector<int>::iterator, int>(v1.begin(), v1.end(), v2.begin())) == 19);
  REQUIRE((tarsier::computeEuclideanDistanceFast<std::vector<int>::iterator, int>(v1.begin(), v1.end(), v2.begin())) == 19);

  std::vector<double> v3{0.1,0.2,0.3,0.1,0.1,0.1,0.1};
  tarsier::computeBattacharyaDistance<std::vector<double>::iterator> bata(1000,0);
  tarsier::computeBattacharyaDistanceFast<std::vector<double>::iterator> bataFast(1000,0);
  REQUIRE((bata(v3.begin(), v3.end(), v3.begin())) == 0);
  REQUIRE((bataFast(v3.begin(), v3.end(), v3.begin())) == 0);
}

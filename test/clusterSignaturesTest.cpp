#include "../source/clusterSignatures.hpp"
#include <iostream>
#include <vector>

#include "catch.hpp"

typedef std::vector<int> VEC;

TEST_CASE("Cluster signatures", "[clusterSig]") {
  auto mydist = [](VEC v1, VEC v2){
    double d = 0.;
    for(auto i = 0; i < v1.size(); i++){
      d+=std::pow(v1[i]-v2[i],2);
    }
    return d;
  };

  bool go = false;
  auto clu = tarsier::make_clusterSignatures<VEC>(mydist, [&go](std::vector<std::vector<double> > d){
      if(go){
        std::vector<VEC> dref{{0,6,0,13,38},{6,0,6,9,26},{0,6,0,13,38},{13,9,13,0,11},{38,26,38,11,0}};
        double sum = 0;
        for(auto i = 0; i < d.size(); i++){
          for(auto j = 0; j < d[i].size(); j++){
            sum+=(d[i][j]-dref[i][j]);
          }
        }
        REQUIRE(sum == 0);
      }
    });

  std::vector<VEC> v{{1,5,3},{2,3,4},{1,5,3},{3,5,6}};

  for(auto&& it: v){
    clu(it);
  }
  go = true;
  clu(VEC{2,4,9});
}

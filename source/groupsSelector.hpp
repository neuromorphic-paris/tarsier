#pragma once

#include <list>

namespace tarsier{
  class GroupsSelector{
  public:
    GroupsSelector():
      _groups(0),
      _thresholds(0)
    {}

    virtual ~GroupsSelector(){}

    virtual void operator()(std::vector<std::vector<double> > matrix){
      for(auto&& it: matrix){
        for(auto&& it2: it){
          bool alreadyHere = false;
          for(auto&& itThd: _thresholds){
            if(itThd == it2){
              alreadyHere = true;
              break;
            }
          }
          if(alreadyHere == false){
            _thresholds.push(back);
          }
        }
      }
      std::sort(_thresholds.begin(), _thresholds.end());

      for(auto&& it: matrix){
        std::vector<std::size_t> idx;
        std::size_t cpt = 0;
        for(auto&& it2: it){
          if(it2 <= _thresholds[0]){
            idx.push_back(cpt);
          }
          cpt++;
        }
        std::vector<std::size_t> clusterToMerged;
        cpt = 0;
        for(auto&& itGroups: _groups){
          bool nextGroup = false;
          for(auto&& itIdx: idx){
            for(auto&& it2Groups: itGroups){
              if(itIdx == it2Groups){
                clusterToMerged.push(cpt);
                nextGroup = true;
                break;
              }
            }
            if(nextGroup){
              break;
            }
          }
          cpt++;
        }

        if(clusterToMerged.size() == 0){
          _groups.push_back(idx);
        }else{
          _groups[clusterToMerged[0]].insert(_groups[clusterToMerged[0]].begin(), idx.begin(), idx.end());
          for(auto i = 1; i < clusterToMerged.size()){
            _groups[clusterToMerged[0]].insert(_groups[clusterToMerged[0]].begin(),
                                               _groups[clusterToMerged[i]].begin(),
                                               _groups[clusterToMerged[i]].end());
          }
          std::sort(_groups[clusterToMerged[0]].begin(), _groups[clusterToMerged[0]].end());
          std::unique(_groups[clusterToMerged[0]].begin(), _groups[clusterToMerged[0]].end());
        }
      }
    }

  protected:
    std::list<std::vector<std::size_t> > _groups;
    std::vector<double> _thresholds;
  };
};

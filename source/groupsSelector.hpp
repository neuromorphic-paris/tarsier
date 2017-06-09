#pragma once

#include <list>
#include <vector>
#include <algorithm>

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
            _thresholds.push_back(it2);
          }
        }
      }
      std::sort(_thresholds.begin(), _thresholds.end());
      /*for(auto&& it: _thresholds){
        std::cout << it << "\t";
        }
        std::cout << std::endl;
        std::cout << std::endl;*/


      for(auto&& itThd: _thresholds){
        _groups = this->reduce(matrix, itThd);
        std::vector<double> interClass;
        std::vector<double> idxCenters;
        auto groupsCpt = 0;
        std::cout << "--------------------------------------------------\n";
        std::cout << "THD: " << itThd << std::endl;
        std::cout << "--------------------------------------------------\n";
        for(auto&& it: _groups){
          auto errMean = 0.;
          auto minimum = -1.;
          auto cpt = 0;
          idxCenters.push_back(0);
          for(auto&& it2: it){
            auto tempErr = 0.;
            for(auto&& it3: it){
              tempErr+=matrix[it2][it3];
            }
            if(minimum < 0){
              minimum = tempErr;
              idxCenters.back() = it2;
            }else if(minimum > tempErr){
              minimum = tempErr;
              idxCenters.back() = it2;
            }
            errMean+= tempErr;
            cpt++;
          }
          // idxCenters.push_back(tempErr);
          errMean/=(it.size()*it.size());
          std::cout << "(intra " << groupsCpt++ << ": " << errMean << ")\t";
          for(auto&& it2: it){
            std::cout << it2 << "\t";
          }
          std::cout << std::endl;
        }
        std::cout << std::endl;
        auto cpt = 0;
        std::cout << "\t";
        for(auto&& it: idxCenters){
          std::cout << cpt++ << "\t";
        }
        std::cout << std::endl;
        cpt = 0;
        for(auto&& it: idxCenters){
          std::cout << cpt++ << ")\t";
          for(auto&& it2: idxCenters){
            std::cout << matrix[it][it2] << "\t";
          }
          std::cout << std::endl;
        }
        std::cout << std::endl;
        std::cout << std::endl;
        if(_groups.size() <= 5){//matrix.size()/10){
          std::cout << "Ok it is fine now!" << std::endl;
          break;
        }
      }
    }

  protected:
    std::list<std::vector<std::size_t> > reduce(std::vector<std::vector<double> > matrix, double thd){
      std::list<std::vector<std::size_t> > groups;

      for(auto&& it: matrix){
        std::vector<std::size_t> idx;
        std::size_t cpt = 0;
        for(auto&& it2: it){
          if(it2 <= thd){
            idx.push_back(cpt);
          }
          cpt++;
        }
        std::vector<std::size_t> clusterToMerged;
        cpt = 0;
        for(auto&& itGroups: groups){
          bool nextGroup = false;
          for(auto&& itIdx: idx){
            for(auto&& it2Groups: itGroups){
              if(itIdx == it2Groups){
                clusterToMerged.push_back(cpt);
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
          groups.push_back(idx);
        }else{
          auto selectedGroup = std::next(groups.begin(),clusterToMerged[0]);
          selectedGroup->insert(selectedGroup->begin(), idx.begin(), idx.end());
          for(auto i = clusterToMerged.size()-1; i > 0; i--){
            auto temp = std::next(groups.begin(),clusterToMerged[i]);
            selectedGroup->insert(selectedGroup->begin(),temp->begin(),temp->end());
            groups.erase(temp);
          }

          std::sort(selectedGroup->begin(), selectedGroup->end());
          auto temp = std::unique(selectedGroup->begin(), selectedGroup->end());
          selectedGroup->resize(std::distance(selectedGroup->begin(), temp));
        }
      }
      return groups;
    }

    std::list<std::vector<std::size_t> > _groups;
    std::vector<double> _thresholds;
  };
};

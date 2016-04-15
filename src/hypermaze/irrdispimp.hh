/**
 * @file irrdispimp.hh
 * @brief Extra classes used to display and manage the hypermaze
 */
#include "irrdisp.hh"

#ifndef IRRDISP_IMP_HH_INC
#define IRRDISP_IMP_HH_INC

class VisibleCounter{
  int visible;
  public:
  irr::scene::IMeshSceneNode* node;
    VisibleCounter(irr::scene::IMeshSceneNode* node):node(node),visible(0){};
    bool setVisible(bool isVisible){
      if(isVisible)
        visible++;
      else
        visible--;
      if(visible<0&&node->isVisible()){
        node->setVisible(false);
        return true;
      }else if(visible>=0&&!node->isVisible()){
        node->setVisible(true);
        return true;
      }
      return false;
    }
};

class MazeDisplay{
  std::map<Dirn,std::pair<std::pair<int,int>,std::pair<Dirn,bool> > > limits;
  std::map<Dirn,std::vector<std::vector<VisibleCounter*>*>*> nodes;
  std::set<Dirn> dirns;
  public:
    void init(Maze& m,NodeGen* ng,irr::core::vector3df center=irr::core::vector3df(0,0,0));

    void clear();

    MazeDisplay(Maze& m,NodeGen* ng,irr::core::vector3df center=irr::core::vector3df(0,0,0)){
      dirns.insert(UP);
      dirns.insert(LEFT);
      dirns.insert(FORWARD);
      init(m,ng,center);
    }

    ~MazeDisplay(){clear();};

    bool hideSide(Dirn side,bool out);
};

class StringDisplay{
  SP<String> s;
  std::list<irr::scene::IMeshSceneNode*> nodes;
  irr::scene::IMeshSceneNode* startEnd;
  irr::scene::IMeshSceneNode* endEnd;
  int activeNodes;
  NodeGen* ng;
  irr::core::vector3df center;
  static const int STRING_ID=1536;//(3<<9)
  public:

    void update();
    void updateActive();

    std::pair<StringPointer,bool> getStringPointer(irr::scene::ISceneNode* node);

    StringDisplay(SP<String> s,NodeGen* ng,irr::core::vector3df center=irr::core::vector3df(0,0,0)):s(s),center(center),ng(ng),activeNodes(0),startEnd(ng->makeStringEnd()),endEnd(ng->makeStringEnd()){
      endEnd->setRotation(irr::core::vector3df(0,180,0));
      update();
    };
    void setString(SP<String> _s){
      s=_s;
      update();
    }
};

#endif

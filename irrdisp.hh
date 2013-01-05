#include "irrlicht.h"
#include "maze.hh"
#include "string.hh"
#include <map>
#include <vector>
#include <list>
#include "dirns.hh"

#ifndef IRRDISP_HH_INC
#define IRRDISP_HH_INC

namespace irr{
  using namespace core;
  using namespace scene;
  using namespace io;
  using namespace video;
};

using namespace std;

inline irr::vector3df con(Vector v){
  return irr::vector3df(-v.X,v.Y,v.Z);
}

inline irr::vector3df remSgn(irr::vector3df v){
  return irr::vector3df(abs(v.X),abs(v.Y),abs(v.Z));
}

class NodeGen{
  public:
    virtual irr::IMeshSceneNode* makeUnitWall(bool isNode)=0;
    virtual irr::IMeshSceneNode* makeUnitString(bool isNode)=0;
    virtual void makeStringActive(irr::IMeshSceneNode* node,bool active)=0;
    virtual irr::IMeshSceneNode* makeUnitHandle()=0;
};

class VisibleCounter{
  int visible;
  public:
  irr::IMeshSceneNode* node;
    VisibleCounter(irr::IMeshSceneNode* node):node(node),visible(0){};
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
  map<Dirn,pair<pair<int,int>,pair<Dirn,bool> > > limits;
  map<Dirn,vector<vector<VisibleCounter*>*>*> nodes;
  set<Dirn> dirns;
  public:
    static const double wall=5;
    static const double gap=20;

    void init(Maze& m,NodeGen* ng,irr::vector3df center=irr::vector3df(0,0,0));

    void clear();

    MazeDisplay(Maze& m,NodeGen* ng,irr::vector3df center=irr::vector3df(0,0,0)){
      dirns.insert(UP);
      dirns.insert(LEFT);
      dirns.insert(FORWARD);
      init(m,ng,center);
    }

    bool hideSide(Dirn side,bool out);
};

class StringDisplay{
  StringSlice& s;
  list<irr::IMeshSceneNode*> nodes;
  int activeNodes;
  NodeGen* ng;
  irr::vector3df center;
  static const int STRING_ID=1536;//(3<<9)
  public:

    void update();
    void updateActive();
    
    pair<StringPointer,bool> getStringPointer(irr::ISceneNode* node);
      
    StringDisplay(StringSlice& s,NodeGen* ng,irr::vector3df center=irr::vector3df(0,0,0)):s(s),center(center),ng(ng),activeNodes(0){
      update();
    };
    void setString(StringSlice _s){
      s=_s;
      update();
    }
};

class Controller;

class PuzzleDisplay{
  public:
    Maze m;
    String s;
    StringSlice ss;
    Controller* c;
  private:
    NodeGen* ng;
    MazeDisplay md;
    StringDisplay sd;
    map<irr::ISceneNode*,Dirn> slicers;
    bool won;
   public:
    PuzzleDisplay(NodeGen* ng):m(Vector(5,5,5)),s(m),ss(s),ng(ng),md(m,ng),sd(ss,ng),won(false){
      for(Dirn* d=allDirns;d!=allDirns+6;++d){
        irr::IMeshSceneNode* node = ng->makeUnitHandle();
        node->setScale(irr::vector3df(1,1,1)*(md.wall+md.gap)/2);
        node->setPosition(-con(to_vector(*d))*(abs(to_vector(*d).dotProduct(m.size))/2+2)*(md.wall+md.gap));
        slicers[node]=*d;
      }
    };

    const map<irr::ISceneNode*,Dirn>& getSlicers();
    
    pair<StringPointer,bool> getStringPointer(irr::ISceneNode* node);

    void stringUpdated();
    
    void stringSelectionUpdated();
    
    void mazeUpdated();
    
    bool hideSide(Dirn side,bool out);
};

#endif

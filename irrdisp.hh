#include "irrlicht.h"
#include "maze.hh"
#include "string.hh"
#include <map>
#include <vector>
#include <set>
#include <list>
#include "dirns.hh"
#include "script.hh"
#include "sound.hh"
#include "fonts.hh"

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
    virtual irr::IMeshSceneNode* makeStringEnd()=0;
    virtual irr::IMeshSceneNode* makeUnitString(bool isNode)=0;
    virtual void makeStringActive(irr::IMeshSceneNode* node,bool active)=0;
    virtual irr::IMeshSceneNode* makeUnitHandle(int isForward)=0;
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
    
    ~MazeDisplay(){clear();};

    bool hideSide(Dirn side,bool out);
};

class StringDisplay{
  String& s;
  list<irr::IMeshSceneNode*> nodes;
  irr::IMeshSceneNode* startEnd;
  irr::IMeshSceneNode* endEnd;
  int activeNodes;
  NodeGen* ng;
  irr::vector3df center;
  static const int STRING_ID=1536;//(3<<9)
  public:

    void update();
    void updateActive();
    
    pair<StringPointer,bool> getStringPointer(irr::ISceneNode* node);
      
    StringDisplay(String& s,NodeGen* ng,irr::vector3df center=irr::vector3df(0,0,0)):s(s),center(center),ng(ng),activeNodes(0),startEnd(ng->makeStringEnd()),endEnd(ng->makeStringEnd()){
      endEnd->setRotation(irr::vector3df(0,180,0));
      update();
    };
    void setString(String _s){
      s=_s;
      update();
    }
};

class Controller;

class PuzzleDisplay{
  public:
    Maze m;
    Script sc;
    String s;
    StringPlay sp;
  private:
    NodeGen* ng;
    MazeDisplay md;
    StringDisplay sd;
    map<irr::ISceneNode*,Dirn> slicers;
    bool won;
    irr::IrrlichtDevice *device;
    FontManager* fm;
    SoundManager *sm;
   public:
    PuzzleDisplay(NodeGen* ng,irr::IrrlichtDevice* device,FontManager* fm,SoundManager* sm):m(Vector(5,5,5)),sc(),s(m),sp(s),ng(ng),md(m,ng),sd(s,ng),won(false),device(device),fm(fm),sm(sm){
      for(Dirn* d=allDirns;d!=allDirns+6;++d){
        irr::IMeshSceneNode* node = ng->makeUnitHandle(to_vector(*d).dotProduct(to_vector(s.targetDir)));
        node->setScale(irr::vector3df(1,1,1)*(md.wall+md.gap)/2);
        node->setPosition(-con(to_vector(*d))*(abs(to_vector(*d).dotProduct(m.size))/2+2)*(md.wall+md.gap));
        slicers[node]=*d;
      }
    };

    SP<Dirn> getSlicerDirn(irr::ISceneNode* slicer);
    
    pair<StringPointer,bool> getStringPointer(irr::ISceneNode* node);

    void win();

    void stringUpdated();
    
    void stringSelectionUpdated();
    
    void mazeUpdated();
    
    bool hideSide(Dirn side,bool out);
};

#endif

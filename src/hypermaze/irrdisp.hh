#include "irrlicht.h"
#include "../core/maze.hh"
#include "../core/string.hh"
#include <map>
#include <vector>
#include <set>
#include <list>
#include "../core/dirns.hh"
#include "../core/script.hh"
#include "../shared/sound.hh"
#include "../irrshared/fonts.hh"

#ifndef IRRDISP_HH_INC
#define IRRDISP_HH_INC

inline irr::core::vector3df con(Vector v){
  return irr::core::vector3df(-v.X,v.Y,v.Z);
}

inline irr::core::vector3df remSgn(irr::core::vector3df v){
  return irr::core::vector3df(abs(v.X),abs(v.Y),abs(v.Z));
}

class NodeGen{
  public:
    virtual irr::scene::IMeshSceneNode* makeUnitWall(bool isNode)=0;
    virtual irr::scene::IMeshSceneNode* makeStringEnd()=0;
    virtual irr::scene::IMeshSceneNode* makeUnitString(bool isNode)=0;
    virtual void makeStringActive(irr::scene::IMeshSceneNode* node,bool active)=0;
    virtual irr::scene::IMeshSceneNode* makeUnitHandle(int isForward)=0;
};

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
    static const double wall;
    static const double gap;

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

class Controller;

class PuzzleDisplay{
  public:
    Maze m;
    Script sc;
    SP<String> s;
    StringPlay sp;
  private:
    NodeGen* ng;
    MazeDisplay md;
    StringDisplay sd;
    std::map<irr::scene::ISceneNode*,Dirn> slicers;
    bool won;
    irr::IrrlichtDevice *device;
    FontManager* fm;
    SoundManager *sm;
   public:
    PuzzleDisplay(NodeGen* ng,irr::IrrlichtDevice* device,FontManager* fm,SoundManager* sm);

    SP<Dirn> getSlicerDirn(irr::scene::ISceneNode* slicer);

    std::pair<StringPointer,bool> getStringPointer(irr::scene::ISceneNode* node);

    void win();

    void stringUpdated();

    void stringSelectionUpdated();

    void mazeUpdated();

    bool hideSide(Dirn side,bool out);
};

#endif

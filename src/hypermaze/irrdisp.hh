/**
 * @file irrdisp.hh
 * @brief The classes to display and manage the hypermaze
 */
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
  return irr::core::vector3df(-(irr::f32)v.X, (irr::f32)v.Y, (irr::f32)v.Z);
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

class MazeDisplay; // defined in irrdispimp.hh
class StringDisplay; // defined in irrdispimp.hh
class MultiInterfaceController; // defined in controller.hh

extern const double WALL_SIZE;
extern const double GAP_SIZE;


class PuzzleDisplay{
  public:
    Maze m;
    Script sc;
    SP<String> s;
    StringPlay sp;
  private:
    NodeGen* ng;
    MazeDisplay* md;
    StringDisplay* sd;
    std::map<irr::scene::ISceneNode*,Dirn> slicers;
    bool won;
    irr::IrrlichtDevice *device;
    FontManager* fm;
    SoundManager *sm;
   public:
    PuzzleDisplay(NodeGen* ng,irr::IrrlichtDevice* device,FontManager* fm,SoundManager* sm);

    SP<Dirn> getSlicerDirn(irr::scene::ISceneNode* slicer);

    std::pair<StringPointer,bool> getStringPointer(irr::scene::ISceneNode* node);

    void win(MultiInterfaceController* c);

    void stringUpdated(MultiInterfaceController* c);

    void stringSelectionUpdated(MultiInterfaceController* c);

    void mazeUpdated(MultiInterfaceController* c);

    bool hideSide(Dirn side,bool out);

    ~PuzzleDisplay();
};

#endif

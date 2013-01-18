#include "irrlicht.h"
#include "string.hh"
#include "keymap.hh"
#include "irrdisp.hh"

#ifndef CONTROLLER_HH_INC
#define CONTROLLER_HH_INC

namespace irr{
  using namespace core;
  using namespace scene;
  using namespace io;
  using namespace video;
};

using namespace std;

class PuzzleDisplay;

class Controller: public irr::IEventReceiver{
  protected:
    PuzzleDisplay& pd;
  public:
    virtual void run(irr::u32 now)=0;
    virtual bool OnEvent(const irr::SEvent& event)=0;
    virtual bool onWin(){};
    Controller(PuzzleDisplay& pd):pd(pd){};
};

class KeyboardController:public Controller{
  bool actionTriggered[KeyMap::A_COUNT];
  irr::u32 actionTime[KeyMap::A_COUNT];
  static const irr::u32 DELAY=500;
  irr::IrrlichtDevice* device;

  public:
    KeyMap map;

    virtual bool OnEvent(const irr::SEvent& event);
    virtual bool isTriggered(KeyMap::Action a);
    
    virtual void run(irr::u32 now);
    
    virtual bool onWin();

    KeyboardController(PuzzleDisplay& pd,irr::IrrlichtDevice *device):Controller(pd),device(device){
      for (irr::u32 i=0; i<KeyMap::A_COUNT; ++i){
        actionTriggered[i] = false;
        actionTime[i] = 0;
      }
    };
};


class MouseSlicerController: public Controller{
  irr::ISceneCollisionManager* collMan;
  irr::ISceneNode* slice;
  int sliced;
  irr::vector3df sliceStart;
  irr::position2d<irr::s32> mousePos;


  public:
   virtual void run(irr::u32 now);

    virtual bool OnEvent(const irr::SEvent& event);
    
    MouseSlicerController(PuzzleDisplay& pd,irr::IrrlichtDevice *device):Controller(pd),collMan(device->getSceneManager()->getSceneCollisionManager()),slice(0),mousePos(0,0){};
};


class MouseStringDraggerController: public Controller{
  irr::ISceneCollisionManager* collMan;
  irr::ISceneNode* string;
  irr::vector3df startPoint;
  irr::position2d<irr::s32> mousePos;
  int dist;
  Dirn currdir;


  public:
    virtual void run(irr::u32 now);

    virtual bool OnEvent(const irr::SEvent& event);

    MouseStringDraggerController(PuzzleDisplay& pd,irr::IrrlichtDevice *device):Controller(pd),collMan(device->getSceneManager()->getSceneCollisionManager()),string(0),mousePos(0,0){};
};


class MouseStringSelectorController: public Controller{
  irr::ISceneCollisionManager* collMan;
  irr::position2d<irr::s32> mousePos;
  irr::vector3df startPoint;
  irr::ISceneNode* string;
  pair<StringPointer,bool> sp;
  bool selected;
  int moved;
  
  public:
  
    virtual void run(irr::u32 now);
    
    virtual bool OnEvent(const irr::SEvent& event);
  
    MouseStringSelectorController(PuzzleDisplay& pd,irr::IrrlichtDevice *device):Controller(pd),collMan(device->getSceneManager()->getSceneCollisionManager()),sp(pd.s.end(),false){};
};
  

class MultiInterfaceController:public Controller{
  public:
    KeyboardController kc;
    MouseSlicerController msc;
    MouseStringDraggerController mdc;
    MouseStringSelectorController mssc;

    MultiInterfaceController(PuzzleDisplay& pd,irr::IrrlichtDevice *device):Controller(pd),kc(pd,device),msc(pd,device),mdc(pd,device),mssc(pd,device){};

    virtual bool OnEvent(const irr::SEvent& event){
      return kc.OnEvent(event)||msc.OnEvent(event)||mdc.OnEvent(event)||mssc.OnEvent(event);
    }

    virtual void run(irr::u32 now){
      kc.run(now);
      msc.run(now);
      mdc.run(now);
      mssc.run(now);
    }
    virtual bool onWin(){
      return kc.onWin()||msc.onWin()||mdc.onWin()||mssc.onWin();
    }

};


#endif
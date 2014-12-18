#include "irrlicht.h"
#include "string.hh"
#include "keymap.hh"
#include "irrdisp.hh"
#include "sound.hh"
#include "fonts.hh"

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
    Controller(PuzzleDisplay& pd):pd(pd){};
    virtual ~Controller(){};
};

class KeyboardController:public Controller{
  bool actionTriggered[KeyMap::A_COUNT];
  irr::u32 actionTime[KeyMap::A_COUNT];
  static const irr::u32 DELAY=500;
  irr::IrrlichtDevice* device;
  FontManager* fm;
  SoundManager* sm;

  public:
    KeyMap map;

    virtual bool OnEvent(const irr::SEvent& event);
    virtual bool isTriggered(KeyMap::Action a);
    
    virtual void run(irr::u32 now);
    
    KeyboardController(PuzzleDisplay& pd,irr::IrrlichtDevice *device,FontManager* fm,SoundManager* sm):Controller(pd),device(device),fm(fm),sm(sm){
      for (irr::u32 i=0; i<KeyMap::A_COUNT; ++i){
        actionTriggered[i] = false;
        actionTime[i] = 0;
      }
      map.addMapping(irr::KEY_F1,KeyMap::A_CONF);
      irr::IReadFile* in=device->getFileSystem()->createAndOpenFile("hypermaze.keymap.conf");
      map.load(in);
      in->drop();
    };
};


class MouseSlicerController: public Controller{
  irr::ISceneCollisionManager* collMan;
  SP<Dirn> slice;
  int sliced;
  irr::vector3df sliceStart;
  irr::position2d<irr::s32> mousePos;


  public:
   virtual void run(irr::u32 now);

    virtual bool OnEvent(const irr::SEvent& event);
    
    MouseSlicerController(PuzzleDisplay& pd,irr::IrrlichtDevice *device,FontManager*,SoundManager* sm):
        Controller(pd),collMan(device->getSceneManager()->getSceneCollisionManager()),slice(),mousePos(0,0){};
};


class MouseStringDraggerController: public Controller{
  irr::ISceneCollisionManager* collMan;
  irr::ISceneNode* string;
  irr::vector3df startPoint;
  irr::position2d<irr::s32> mousePos;
  int dist;
  Dirn currdir;
  SoundManager* sm;


  public:
    virtual void run(irr::u32 now);

    virtual bool OnEvent(const irr::SEvent& event);

    MouseStringDraggerController(PuzzleDisplay& pd,irr::IrrlichtDevice *device,FontManager*,SoundManager* sm):Controller(pd),collMan(device->getSceneManager()->getSceneCollisionManager()),string(0),mousePos(0,0),sm(sm){};
};

class MouseStringDraggerController2: public Controller{
  irr::ISceneCollisionManager* collMan;
  irr::ISceneNode* string;
  irr::vector3df startPoint;
  irr::position2d<irr::s32> mousePos;
  int alloweddirns;
  int dist;
  Dirn currdir;
  SoundManager* sm;


  public:
    virtual void run(irr::u32 now);

    virtual bool OnEvent(const irr::SEvent& event);

    MouseStringDraggerController2(PuzzleDisplay& pd,irr::IrrlichtDevice *device,FontManager*,SoundManager* sm):Controller(pd),collMan(device->getSceneManager()->getSceneCollisionManager()),string(0),mousePos(0,0),sm(sm){};
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
  
    MouseStringSelectorController(PuzzleDisplay& pd,irr::IrrlichtDevice *device,FontManager*,SoundManager* sm):
        Controller(pd),collMan(device->getSceneManager()->getSceneCollisionManager()),string(0),sp(pd.s.end(),false){};
};

class MultiInterfaceController:public Controller{
  public:
    Controller* kc;
    Controller* msc;
    Controller* mdc;
    Controller* mssc;

    MultiInterfaceController(PuzzleDisplay& pd,irr::IrrlichtDevice *device,FontManager* fm,SoundManager* sm):
        Controller(pd),kc(new KeyboardController(pd,device,fm,sm)),msc(new MouseSlicerController(pd,device,fm,sm)),
        mdc(new MouseStringDraggerController2(pd,device,fm,sm)),mssc(new MouseStringSelectorController(pd,device,fm,sm)){};

    virtual bool OnEvent(const irr::SEvent& event){
      return kc->OnEvent(event)||msc->OnEvent(event)||mdc->OnEvent(event)||mssc->OnEvent(event);
    }

    virtual void run(irr::u32 now){
      kc->run(now);
      msc->run(now);
      mdc->run(now);
      mssc->run(now);
    }
};


#endif

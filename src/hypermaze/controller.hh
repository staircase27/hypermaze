#include "irrlicht.h"
#include "../core/string.hh"
#include "keymap.hh"
#include "irrdisp.hh"
#include "../shared/sound.hh"
#include "../irrshared/fonts.hh"

#ifndef CONTROLLER_HH_INC
#define CONTROLLER_HH_INC

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
  static const irr::u32 DELAY=400;
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
      map.load(device->getFileSystem());
      KeySpec help=map.getKeySpec(KeyMap::A_CONF);
      if(help.chr==0&&help.key==irr::KEY_KEY_CODES_COUNT)
        map.addMapping(KeySpec(irr::KEY_F1),KeyMap::A_CONF);
    };
};


class MouseSlicerController: public Controller{
  irr::scene::ISceneCollisionManager* collMan;
  SP<Dirn> slice;
  int sliced;
  irr::core::vector3df sliceStart;
  irr::core::position2d<irr::s32> mousePos;


  public:
   virtual void run(irr::u32 now);

    virtual bool OnEvent(const irr::SEvent& event);

    MouseSlicerController(PuzzleDisplay& pd,irr::IrrlichtDevice *device,FontManager*,SoundManager* sm):
        Controller(pd),collMan(device->getSceneManager()->getSceneCollisionManager()),slice(),mousePos(0,0){};
};

class MouseStringDraggerController: public Controller{
  irr::scene::ISceneCollisionManager* collMan;
  irr::scene::ISceneNode* string;
  irr::core::vector3df startPoint;
  irr::core::position2d<irr::s32> mousePos;
  int alloweddirns;
  int dist;
  Dirn currdir;
  SoundManager* sm;


  public:
    virtual void run(irr::u32 now);

    virtual bool OnEvent(const irr::SEvent& event);

    MouseStringDraggerController(PuzzleDisplay& pd,irr::IrrlichtDevice *device,FontManager*,SoundManager* sm):
                Controller(pd),collMan(device->getSceneManager()->getSceneCollisionManager()),string(0),
                mousePos(0,0),dist(0),currdir(FORWARD),sm(sm){};
};

class MouseStringSelectorController: public Controller{
  irr::scene::ISceneCollisionManager* collMan;
  irr::core::position2d<irr::s32> mousePos;
  irr::core::vector3df startPoint;
  irr::scene::ISceneNode* string;
  std::pair<StringPointer,bool> sp;
  bool selected;
  int moved;

  public:

    virtual void run(irr::u32 now);

    virtual bool OnEvent(const irr::SEvent& event);

    MouseStringSelectorController(PuzzleDisplay& pd,irr::IrrlichtDevice *device,FontManager*,SoundManager* sm):
        Controller(pd),collMan(device->getSceneManager()->getSceneCollisionManager()),string(0),sp(pd.s->end(),false){};
};

class MultiInterfaceController:public Controller{
  public:
    Controller* kc;
    Controller* msc;
    Controller* mdc;
    Controller* mssc;

    MultiInterfaceController(PuzzleDisplay& pd,irr::IrrlichtDevice *device,FontManager* fm,SoundManager* sm):
        Controller(pd),kc(new KeyboardController(pd,device,fm,sm)),msc(new MouseSlicerController(pd,device,fm,sm)),
        mdc(new MouseStringDraggerController(pd,device,fm,sm)),mssc(new MouseStringSelectorController(pd,device,fm,sm)){};

    virtual bool OnEvent(const irr::SEvent& event){
      return kc->OnEvent(event)||msc->OnEvent(event)||mdc->OnEvent(event)||mssc->OnEvent(event);
    }

    virtual void run(irr::u32 now){
      kc->run(now);
      msc->run(now);
      mdc->run(now);
      mssc->run(now);
    }
    
    virtual ~MultiInterfaceController(){
      delete kc;
      delete msc;
      delete mdc;
      delete mssc;
    }
};


#endif


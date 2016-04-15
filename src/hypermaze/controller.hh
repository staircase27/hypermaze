/**
 * @file controller.hh
 * @brief The classes to control and interact with the hypermaze
 */
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

class MultiInterfaceController: public Controller{
  public:
    MultiInterfaceController(PuzzleDisplay& pd):Controller(pd){};
    virtual void showGUI(bool show)=0;
    virtual KeyMap& getKeyMap()=0;
};

MultiInterfaceController* createController(PuzzleDisplay& pd,irr::IrrlichtDevice *device,FontManager* fm,SoundManager* sm);
#endif


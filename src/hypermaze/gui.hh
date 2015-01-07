#include "irrlicht.h"
#include "../core/script.hh"
#include "../irrshared/fonts.hh"

#ifndef GUI_HH_INC
#define GUI_HH_INC
class Maze;
class PuzzleDisplay;


using namespace std;

namespace irr{
  using namespace core;
  using namespace scene;
  using namespace io;
  using namespace video;
  using namespace gui;
};

static const int MGUIET_EMPTY = irr::EGUIET_COUNT+10;

class CGUIEmptyElement : public irr::IGUIElement
{
  public:
    CGUIEmptyElement(irr::IGUIEnvironment* environment, irr::IGUIElement* parent)
        :irr::IGUIElement((irr::EGUI_ELEMENT_TYPE)MGUIET_EMPTY, environment, parent, -1, irr::rect<irr::s32>(0,0,100000,10000)) {}

    irr::IGUIEnvironment* GetEnvironment() { return Environment; }

    virtual const irr::c8* getTypeName() const   { return "empty"; }

    virtual bool isPointInside(const irr::position2d<irr::s32>& point) const { return false; }

    virtual bool bringToFront(IGUIElement* element)
    {
      bool result = IGUIElement::bringToFront(element);
      if ( Parent )
      {
        Parent->bringToFront(this);
      }
      return result;
    }
};

class BaseGui : irr::IEventReceiver{
  protected:
    irr::IrrlichtDevice* device;
    FontManager* fm;
    irr::IEventReceiver* oldReceiver;
    CGUIEmptyElement* el;

    BaseGui():device(0),fm(0){};

    bool OnEvent(const irr::SEvent &event);
    virtual bool OnEventImpl(const irr::SEvent &event)=0;

    void apply(irr::IrrlichtDevice* _device);
    void unapply();
    
    virtual void createGUI()=0;
    
    virtual bool run()=0;
    
    void main(irr::IrrlichtDevice* _device,FontManager* _fm);
};

class GenerateGui: BaseGui{

  bool okClicked;
  bool cancelClicked;

  PuzzleDisplay* pd;
  
  irr::gui::IGUISpinBox *xSize, *ySize, *zSize;

  enum
  {
    GUI_ID_OK_BUTTON=201,
    GUI_ID_CANCEL_BUTTON
  };

  protected:
    virtual bool OnEventImpl(const irr::SEvent &event);
    virtual void createGUI();
    virtual bool run();
  public:
    bool generate(irr::IrrlichtDevice* _device,FontManager* _fm,PuzzleDisplay& pd);
};

class SaveGui: BaseGui{

  bool okClicked;
  bool cancelClicked;

  PuzzleDisplay* pd;

  irr::IGUIEditBox * fileField;

  enum
  {
    GUI_ID_OK_BUTTON=201,
    GUI_ID_CANCEL_BUTTON
  };

  protected:
    virtual bool OnEventImpl(const irr::SEvent &event);
    virtual void createGUI();
    bool run();
  public:
    bool save(irr::IrrlichtDevice* _device,FontManager* _fm,PuzzleDisplay& pd);
};

class OpenGui: BaseGui{

  bool okClicked;
  bool cancelClicked;

  PuzzleDisplay* pd;
  
  irr::IGUIEditBox * fileField;
  
  enum
  {
    GUI_ID_OK_BUTTON=201,
    GUI_ID_CANCEL_BUTTON
  };

  protected:
    virtual bool OnEventImpl(const irr::SEvent &event);
    void createGUI();
    bool run();
  public:
    bool open(irr::IrrlichtDevice* _device,FontManager* _fm,PuzzleDisplay& pd);
};

class WinGui: BaseGui{

  bool okClicked,nextClicked,generateClicked,loadClicked, saveClicked;

  PuzzleDisplay* pd;
  
  Message m;
  Pair<SPA<const char> > nextLevel;

  irr::u32 keyblock;

  enum
  {
    GUI_ID_OK_BUTTON=201,
    GUI_ID_GENERATE_BUTTON,
    GUI_ID_LOAD_BUTTON,
    GUI_ID_SAVE_BUTTON,
    GUI_ID_NEXT_BUTTON
  };

  protected:
    virtual bool OnEventImpl(const irr::SEvent &event);
    void createGUI();
    bool run();
  public:
    bool won(irr::IrrlichtDevice* _device,FontManager* _fm,PuzzleDisplay& pd,const Message& m, Pair<SPA<const char> > nextLevel);
    
    WinGui():pd(0),m(),nextLevel(){}
};

class MessageGui: BaseGui{

    Message m;
    
  bool okClicked;

  enum
  {
    GUI_ID_OK_BUTTON=201,
  };

  protected:
    virtual bool OnEventImpl(const irr::SEvent &event);
    void createGUI();
    bool run();
  public:
    bool message(irr::IrrlichtDevice* _device,FontManager* _fm,const Message& m);

    MessageGui():m(){}
};

#endif

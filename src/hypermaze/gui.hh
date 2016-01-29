/**
 * @file gui.hh
 * @brief The GUIs for the hypermaze
 * This header defines a few different GUIElements and classes used for
 * the hypermaze guis and the hypermaze gui classes for each type of gui.
 */
#include "irrlicht.h"
#include "../core/script.hh"
#include "../irrshared/fonts.hh"

#ifndef GUI_HH_INC
#define GUI_HH_INC
class Maze;
class PuzzleDisplay;

/// The id for empty GUI element used to contain other elements
static const int MGUIET_EMPTY = irr::gui::EGUIET_COUNT+10;

/// An empty GUI element used to contains other elements
class CGUIEmptyElement : public irr::gui::IGUIElement
{
  public:
    /// Construct a new empty GUI element
    /**
     * @param environment the GUI environment that the element is attached to
     * @param parent the parent of the new element
     */
    CGUIEmptyElement(irr::gui::IGUIEnvironment* environment, irr::gui::IGUIElement* parent)
        :irr::gui::IGUIElement((irr::gui::EGUI_ELEMENT_TYPE)MGUIET_EMPTY, environment, parent,
                                -1, irr::core::rect<irr::s32>(0,0,100000,10000)) {}

    /// Get the name of this element
    /**
     * @return the const string "empty"
     */
    virtual const irr::c8* getTypeName() const   { return "empty"; }

    /// Check if a point is inside this element.
    /**
     * @param point the point to check
     * @return always returns false. Nothing should count as inside this element
     */
    virtual bool isPointInside(const irr::core::position2d<irr::s32>& point) const { return false; }

    /// Bring this element to the font
    /**
     * Overridden to also bring this to front of parent
     * @param element the element to bring to the front
     * @return True if successful, false if not.
     */
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

    virtual ~BaseGui(){};
};

class ErrorGui: BaseGui{

  Message detail;
  irr::core::stringw msg;

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
    bool error(irr::IrrlichtDevice* _device,FontManager* _fm,const irr::core::stringw msg,irr::core::stringc detail);
    bool error(irr::IrrlichtDevice* _device,FontManager* _fm,const irr::core::stringw msg,const Message& detail);

    ErrorGui():msg(),detail(){}
};

class ConfirmGui: BaseGui{

  Message detail;
  irr::core::stringw msg;

  bool okClicked;
  bool cancelClicked;

  enum
  {
    GUI_ID_OK_BUTTON=201,
    GUI_ID_CANCEL_BUTTON=202,
  };

  protected:
    virtual bool OnEventImpl(const irr::SEvent &event);
    void createGUI();
    bool run();
  public:
    bool confirm(irr::IrrlichtDevice* _device,FontManager* _fm,const irr::core::stringw msg,irr::core::stringc detail);
    bool confirm(irr::IrrlichtDevice* _device,FontManager* _fm,const irr::core::stringw msg,const Message& detail);

    ConfirmGui():msg(),detail(){}
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

  irr::gui::IGUIEditBox * fileField;

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

  irr::gui::IGUIEditBox * fileField;

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



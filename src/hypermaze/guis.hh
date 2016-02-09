/**
 * @file guis.hh
 * @brief The GUIs for the hypermaze
 * This header defines a few different GUIElements and classes used for
 * the hypermaze guis and the hypermaze gui classes for each type of gui.
 */
#include "../core/script.hh"
#include "../irrshared/gui.hh"

#ifndef GUIS_HH_INC
#define GUIS_HH_INC
class Maze;
class PuzzleDisplay;


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



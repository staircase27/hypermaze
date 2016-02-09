#include "irrlicht.h"
#include "../irrshared/gui.hh"

class KeyMap;

class KeyMapGui: BaseGui{

  KeyMap* kmtmp;
  KeyMap* kmp;

  bool okClicked;
  bool cancelClicked;

  irr::gui::IGUITable *table;
  int editing;

  enum
  {
    GUI_ID_OK_BUTTON=201,
    GUI_ID_CANCEL_BUTTON
  };

  protected:
    virtual bool OnEventImpl(const irr::SEvent &event);

  public:
    KeyMapGui():kmtmp(0){};

    bool edit(irr::IrrlichtDevice* _device,FontManager* _fm,KeyMap& _km);
    void createGUI();
    bool run();

    virtual ~KeyMapGui();
};


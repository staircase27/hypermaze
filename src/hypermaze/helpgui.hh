#include "irrlicht.h"
#include "gui.hh"
#include "../irrshared/GUIFormattedText.hh"

class KeyMap;

class HelpGui: BaseGui{

  bool okClicked,keyMapClicked;

  KeyMap* km;

  irr::u32 keyblock;

  enum
  {
    GUI_ID_OK_BUTTON=201,
    GUI_ID_KEY_MAP_BUTTON
  };

  protected:
    virtual bool OnEventImpl(const irr::SEvent &event);


  public:
    bool help(irr::IrrlichtDevice* _device,FontManager* _fm,KeyMap& km);

    void createGUI();

    bool run();
};



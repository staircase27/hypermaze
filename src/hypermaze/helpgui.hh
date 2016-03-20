/**
 * @file helpgui.hh
 * @brief The help gui for the hypermaze
 */
#include "irrlicht.h"
#include "../irrshared/gui.hh"

class KeyMap;

/// A gui to show some help and give access to the keymap
class HelpGui: private BaseGui{

  bool okClicked, ///< has the ok button been clicked yet
       keyMapClicked; ///< has the keymap button been clicked yet

  KeyMap* km; ///< The current keymap so we can edit it

  irr::u32 keyblock; ///< block keys being pressed till this time to stop repeat clicks

  /// An enum for button IDs
  enum
  {
    GUI_ID_OK_BUTTON=201, ///< The ID for the OK button
    GUI_ID_KEY_MAP_BUTTON ///< The ID for the keymap button
  };

  private:
    /// @copydoc BaseGui::OnEventImpl
    virtual bool OnEventImpl(const irr::SEvent &event);
    /// @copydoc BaseGui::createGUI
    void createGUI();
    /// @copydoc BaseGui::run
    bool run();
  public:
    /// Show the help gui
    /**
     * @param _device the irrlicht device
     * @param _fm the font manager to use for fonts
     * @param km the current keymap for potentially editing
     * @return true of ok was clicked
     */
    bool help(irr::IrrlichtDevice* _device,FontManager* _fm,KeyMap& km);

};



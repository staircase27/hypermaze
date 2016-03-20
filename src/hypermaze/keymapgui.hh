/**
 * @file keymapgui.hh
 * @brief The KeyMap editor gui for the hypermaze
 */
#include "irrlicht.h"
#include "../irrshared/gui.hh"

class KeyMap;

/// A gui to edit the keymap
class KeyMapGui: private BaseGui{

  KeyMap* kmtmp; ///< A temporary keymap to store any edits in
  KeyMap* kmp; ///< They keymap we are editing (only changed if ok is clicked)

  bool okClicked; ///< Has ok been clicked yet?
  bool cancelClicked; ///< Has cancel been clicked yet?

  irr::gui::IGUITable *table; ///< The table that show the keymapping
  int editing; ///< Are we currently editing a keymapping

  /// An enum for button IDs
  enum
  {
    GUI_ID_OK_BUTTON=201, ///< ID for the ok button
    GUI_ID_CANCEL_BUTTON, ///< ID for the cancel button
  };

  private:
    /// @copydoc BaseGui::OnEventImpl
    virtual bool OnEventImpl(const irr::SEvent &event);
    /// @copydoc BaseGui::createGUI
    void createGUI();
    /// @copydoc BaseGui::run
    bool run();

  public:
    /// Show the edit keymap gui
    /**
     * If the user accepts their changes then this will also save the changes to file
     * @param _device the irrlicht device
     * @param _fm the font manager to use for fonts
     * @param _km the keymap to edit. Will only be changed if the user accepts their changes
     * @return true  if the keymap was edited (no changes can count)
     */
    bool edit(irr::IrrlichtDevice* _device,FontManager* _fm,KeyMap& _km);

    /// Construct a KeyMapGui
    KeyMapGui():kmtmp(0){};

    /// Destroy a KeyMapGui
    virtual ~KeyMapGui();
};


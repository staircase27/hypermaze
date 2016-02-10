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

/// A gui to display an error message
class ErrorGui: BaseGui{

  Message detail; ///< The error message details
  irr::core::stringw msg; ///< The summary error message to show

  bool okClicked; ///< Has ok been clicked yet?

  /// An enum for button IDs
  enum
  {
    GUI_ID_OK_BUTTON=201, ///< The ID for the OK button
  };

  protected:
    /// @copydoc BaseGui::OnEventImpl
    virtual bool OnEventImpl(const irr::SEvent &event);
    /// @copydoc BaseGui::createGUI
    void createGUI();
    /// @copydoc BaseGui::run
    bool run();
  public:
    /// Show the error gui
    /**
     * @param _device the irrlicht device
     * @param _fm the font manager to use for fonts
     * @param msg the summary error message
     * @param detail a detailed error message
     * @return true if the gui was exited normally
     */
    bool error(irr::IrrlichtDevice* _device,FontManager* _fm,const irr::core::stringw msg,irr::core::stringc detail);
    /// Show the error gui
    /**
     * @param _device the irrlicht device
     * @param _fm the font manager to use for fonts
     * @param msg the summary error message
     * @param detail a detailed error message
     * @return true if the gui was exited normally
     */
    bool error(irr::IrrlichtDevice* _device,FontManager* _fm,const irr::core::stringw msg,const Message& detail);

    /// Construct an ErrorGui object
    ErrorGui():msg(),detail(){}
};

/// A gui to confirm an action with the user
class ConfirmGui: BaseGui{

  Message detail; ///< The error message details
  irr::core::stringw msg; ///< The summary error message to show

  bool okClicked; ///< Has ok been clicked yet?
  bool cancelClicked; ///< Has cancel been clicked yet?

  /// An enum for button IDs
  enum
  {
    GUI_ID_OK_BUTTON=201, ///< ID for the ok button
    GUI_ID_CANCEL_BUTTON, ///< ID for the cancel button
  };

  protected:
    /// @copydoc BaseGui::OnEventImpl
    virtual bool OnEventImpl(const irr::SEvent &event);
    /// @copydoc BaseGui::createGUI
    void createGUI();
    /// @copydoc BaseGui::run
    bool run();
  public:
    /// show the confirm gui
    /**
     * @param _device the irrlicht device
     * @param _fm the font manager to use for fonts
     * @param msg the summary error message
     * @param detail a detailed error message
     * @return true if the ok button was clicked
     */
    bool confirm(irr::IrrlichtDevice* _device,FontManager* _fm,const irr::core::stringw msg,irr::core::stringc detail);
    /// show the confirm gui
    /**
     * @param _device the irrlicht device
     * @param _fm the font manager to use for fonts
     * @param msg the summary error message
     * @param detail a detailed error message
     * @return true if the ok button was clicked
     */
    bool confirm(irr::IrrlichtDevice* _device,FontManager* _fm,const irr::core::stringw msg,const Message& detail);

    /// Construct a ConfirmGui object
    ConfirmGui():msg(),detail(){}
};

/// Gui to generate a new maze
class GenerateGui: BaseGui{

  bool okClicked; ///< Has ok been clicked yet?
  bool cancelClicked; ///< Has cancel been clicked yet?

  /// The puzzle display object to give the generated maze to
  PuzzleDisplay* pd;

  irr::gui::IGUISpinBox *xSize, ///< GUI element to choose the x size of the maze to generate
                        *ySize, ///< GUI element to choose the x size of the maze to generate
                        *zSize; ///< GUI element to choose the x size of the maze to generate

  /// An enum for button IDs
  enum
  {
    GUI_ID_OK_BUTTON=201, ///< ID for the ok button
    GUI_ID_CANCEL_BUTTON, ///< ID for the cancel button
  };

  protected:
    /// @copydoc BaseGui::OnEventImpl
    virtual bool OnEventImpl(const irr::SEvent &event);
    /// @copydoc BaseGui::createGUI
    void createGUI();
    /// @copydoc BaseGui::run
    bool run();
  public:
    /// show the generate gui
    /**
     * @param _device the irrlicht device
     * @param _fm the font manager to use for fonts
     * @param pd the puzzle display to give the generated maze to
     * @return true if a new maze was generated
     */
    bool generate(irr::IrrlichtDevice* _device,FontManager* _fm,PuzzleDisplay& pd);
};

/// Gui to save a maze
class SaveGui: BaseGui{

  bool okClicked; ///< Has ok been clicked yet?
  bool cancelClicked; ///< Has cancel been clicked yet?

  /// The puzzle display object to get the maze and script from
  PuzzleDisplay* pd;

  /// GUI element to input the file name to save the maze as
  irr::gui::IGUIEditBox * fileField;

  /// An enum for button IDs
  enum
  {
    GUI_ID_OK_BUTTON=201, ///< ID for the ok button
    GUI_ID_CANCEL_BUTTON, ///< ID for the cancel button
  };

  protected:
    /// @copydoc BaseGui::OnEventImpl
    virtual bool OnEventImpl(const irr::SEvent &event);
    /// @copydoc BaseGui::createGUI
    void createGUI();
    /// @copydoc BaseGui::run
    bool run();
  public:
    /// Show the save gui
    /**
     * @param _device the irrlicht device
     * @param _fm the font manager to use for fonts
     * @param pd the puzzle display to get the maze and script from
     * @return true if the maze was saved successfully
     */
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



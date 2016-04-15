/**
 * @file guis.hh
 * @brief The GUIs for the hypermaze
 * This header defines a few different GUIElements and classes used for
 * the hypermaze guis and the hypermaze gui classes for each type of gui.
 */
#include "../core/script.hh"
#include "../irrshared/gui.hh"

#include "../irrshared/opensavegui.hh"
#define USEOPENSAVE

#ifndef GUIS_HH_INC
#define GUIS_HH_INC
class Maze;
class PuzzleDisplay;

/// A gui to display a message
class MessageGui: private BaseGui{

  Message m; ///< The message to display

  bool okClicked; ///< Has ok been clicked yet?

  /// An enum for button IDs
  enum
  {
    GUI_ID_OK_BUTTON=201, ///< The ID for the OK button
  };

  private:
    /// @copydoc BaseGui::OnEventImpl
    virtual bool OnEventImpl(const irr::SEvent &event);
    /// @copydoc BaseGui::createGUI
    void createGUI();
    /// @copydoc BaseGui::run
    bool run();
  public:
    /// Show the message gui
    /**
     * @param _device the irrlicht device
     * @param _fm the font manager to use for fonts
     * @param m the message to show
     * @return true of the ok button was clicked
     */
    bool message(irr::IrrlichtDevice* _device,FontManager* _fm,const Message& m);

    /// Construct a MessageGui object
    MessageGui():m(){}
};

/// A gui to display an error message
class ErrorGui: private BaseGui{

  Message detail; ///< The error message details
  irr::core::stringw msg; ///< The summary error message to show

  bool okClicked; ///< Has ok been clicked yet?

  /// An enum for button IDs
  enum
  {
    GUI_ID_OK_BUTTON=201, ///< The ID for the OK button
  };

  private:
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
class ConfirmGui: private BaseGui{

  Message detail; ///< The error message details
  irr::core::stringw msg; ///< The summary error message to show

  bool okClicked; ///< Has ok been clicked yet?
  bool cancelClicked; ///< Has cancel been clicked yet?
  irr::u32 keyblock; ///< A timer to block input so a key can't repeat trigger the same action

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
class GenerateGui: private BaseGui{

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

  private:
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

#ifndef USEOPENSAVE
/// Gui to save a maze
class SaveGui: private BaseGui{

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

  private:
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

/// Gui to load a maze from file
class OpenGui: private BaseGui{

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

  private:
    /// @copydoc BaseGui::OnEventImpl
    virtual bool OnEventImpl(const irr::SEvent &event);
    /// @copydoc BaseGui::createGUI
    void createGUI();
    /// @copydoc BaseGui::run
    bool run();
  public:
    /// show the open gui
    /**
     * @param _device the irrlicht device
     * @param _fm the font manager to use for fonts
     * @param pd the puzzle display to store the loaded maze and script into
     * @return true if a new maze was loaded
     */
    bool open(irr::IrrlichtDevice* _device,FontManager* _fm,PuzzleDisplay& pd);
};
#else
/// Gui to save a maze
class SaveGui: private OpenSaveGui{

  /// The puzzle display object to get the maze and script from
  PuzzleDisplay* pd;

  private:
    /// @copydoc OpenSaveGui::getButtonName
    const wchar_t* getButtonName(){
      return L"Save";
    }
    /// @copydoc OpenSaveGui::getWindowTitle
    const wchar_t* getWindowTitle(){
      return L"HyperMaze: Save";
    }
    /// @copydoc OpenSaveGui::select
    /**
     * This changes into directories, confirms if the user wants to replace an existing file
     * accepts absent files
     */
    E_SELECTION_TYPE select(const irr::fschar_t* file,E_PATH_TYPE type);
    /// @copydoc OpenSaveGui::process
    /**
     * This saves the maze to the file specified and shows an error message if anything failed
     */
    bool process(const irr::fschar_t* file);
    /// @copydoc OpenSaveGui::filtercount
    /**
    * this has 1 filter
    */
    int filtercount() { return 1; }
    /// @copydoc OpenSaveGui::filtername
    const wchar_t* filtername(int i) { return L"Hypermaze Levels (*.hml)"; }
    /// @copydoc OpenSaveGui::filterfiles
    bool filterfiles(int i, const irr::fschar_t* file, bool folder);
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

/// Gui to load a maze from file
class OpenGui: private OpenSaveGui{
  /// The puzzle display object to get the maze and script from
  PuzzleDisplay* pd;

  private:
    /// @copydoc OpenSaveGui::select
    /**
     * This changes into directories, opens filesand rejects absent paths
     */
    E_SELECTION_TYPE select(const irr::fschar_t* file,E_PATH_TYPE type);
    /// @copydoc OpenSaveGui::process
    /**
     * This loads the maze from the file specified and shows an error message if anything failed
     */
    bool process(const irr::fschar_t* file);
    /// @copydoc OpenSaveGui::selectURL
    /**
     * This always returns true to process urls
     */
    bool selectURL(const wchar_t* url);
    /// @copydoc OpenSaveGui::processURL
    /**
     * This loads the maze from the url specified and shows an error message if anything failed
     */
    bool processURL(const wchar_t* file);
    /// @copydoc OpenSaveGui::filtercount
    /**
     * this has 1 filter
     */
    int filtercount() { return 1; }
    /// @copydoc OpenSaveGui::filtername
    const wchar_t* filtername(int i) { return L"Hypermaze Levels (*.hml)"; }
    /// @copydoc OpenSaveGui::filterfiles
    bool filterfiles(int i, const irr::fschar_t* file, bool folder);
  public:
    /// show the open gui
    /**
     * @param _device the irrlicht device
     * @param _fm the font manager to use for fonts
     * @param pd the puzzle display to store the loaded maze and script into
     * @return true if a new maze was loaded
     */
    bool open(irr::IrrlichtDevice* _device,FontManager* _fm,PuzzleDisplay& pd);
};
#endif // USEOPENSAVE

/// Gui to show you have won and options for what to do next
class WinGui: private BaseGui{

  bool okClicked, ///< Has ok been clicked yet?
       nextClicked, ///< Has next been clicked yet?
       generateClicked, ///< Has generate been clicked yet?
       loadClicked, ///< Has load been clicked yet?
       saveClicked; ///< Has save been clicked yet?

  /// The puzzle display object so a new maze can be loaded or generated if requested
  PuzzleDisplay* pd;

  Message m; ///< A message to show on the win screen. Usually set by the current maze's script
  Pair<SPA<const char> > nextLevel; ///< The name and path for the next level. Usually set by the current maze's script

  irr::u32 keyblock; ///< A timer to block input so a key can't repeat trigger the same action

  /// An enum for button IDs
  enum
  {
    GUI_ID_OK_BUTTON=201, ///< ID for the ok button
    GUI_ID_GENERATE_BUTTON, ///< ID for the generate button
    GUI_ID_LOAD_BUTTON, ///< ID for the load button
    GUI_ID_SAVE_BUTTON, ///< ID for the save button
    GUI_ID_NEXT_BUTTON ///< ID for the next button
  };

  private:
    /// @copydoc BaseGui::OnEventImpl
    virtual bool OnEventImpl(const irr::SEvent &event);
    /// @copydoc BaseGui::createGUI
    void createGUI();
    /// @copydoc BaseGui::run
    bool run();
  public:
    /// Show the win gui
    /**
     * @param _device the irrlicht device
     * @param _fm the font manager to use for fonts
     * @param pd the puzzle display to store the loaded maze and script into
     * @param m a message to show on the win gui. Usually set by the current level's script
     * @param nextLevel the name and path for the next level. Usually set by the current level's script
     * @return true if a new maze was loaded or generated
     */
    bool won(irr::IrrlichtDevice* _device,FontManager* _fm,PuzzleDisplay& pd,const Message& m, Pair<SPA<const char> > nextLevel);

    WinGui():m(),nextLevel(){}
};

#endif



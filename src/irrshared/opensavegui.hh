/**
 * @file opensavegui.hh
 * @brief The gui class for loading and saving
 */
#ifndef OPENSAVEGUI_H
#define OPENSAVEGUI_H

#include "gui.hh"
#include "irrlicht.h"

/// A gui to select a file to load from/save to
class OpenSaveGui : private BaseGui
{
  bool okClicked; ///< Has ok been clicked yet?
  bool cancelClicked; ///< Has cancel been clicked yet?

  /// GUI element to input the file name to save/open the maze as/from
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
  protected:
    /// Access the irrlicht device. Needed for access to irrlicht system
    /**
     * @return the current irrlicht device in use
     */
    inline irr::IrrlichtDevice* getDevice(){
      return BaseGui::getDevice();
    }
    /// Access the FontManager which provides access to the fonts used by the gui
    /**
     * @return the current FontManager in use
     */
    inline FontManager* getFontManager(){
      return BaseGui::getFontManager();
    }
    /// Type for how to handle a selection
    enum E_SELECTION_TYPE{
      INVALID=0,///< The selection is invalid (and the user has been told) so do nothing further
      PROCESS=1, ///< The selection should be processed with the process function
      CHANGEDIR=2 ///< The selection is a directory that should be changed into.
    };
    /// what type of thing is the path for
    enum E_PATH_TYPE{
      ABSENT=0, ///< The path is absent
      URL=1, ///< The path is a url
      FILE=2, ///< The path is a file
      FOLDER=3 ///< The path is a folder
    };
    /// Get the name for the open/save button
    /**
     * @return the button name
     */
    virtual const wchar_t* getButtonName(){
      return L"Open";
    }
    /// Get the title to give the window
    /**
     * @return the title for the window
     */
    virtual const wchar_t* getWindowTitle(){
      return L"HyperMaze: Open";
    }
    /// Process the selected file
    /**
     * @param file the file to select.
     * @return true if the file is valid and has been processed successfully
     * @note if false is returned this function should show an error message first to explain the problem
     */
    virtual bool process(const wchar_t* file){return false;};

    /// Can the specified file be selected and if so what should happen
    /**
     * @param file the file to select.
     * @param type if the selection is file, folder or not yet created
     * @return true if the file is valid and has been processed successfully
     * @note if false is returned this function should show an error message first to explain the problem
     */
    virtual E_SELECTION_TYPE select(const wchar_t* file,E_PATH_TYPE type){
      if(type==FOLDER)
        return CHANGEDIR;
      else if(type==FILE || type==URL)
        return PROCESS;
      else
        return INVALID;
    };

    /// Filter the files and folders to show in the dialog
    /**
     * @note the user will be presented an option show all files
     * @param file the file to filter
     * @param folder is the file a folder
     * @return true if the file should be shown
     */
	virtual bool filterfiles(const wchar_t* file, bool folder) { return true; };

    /// Show the open/save gui
    /**
     * @param _device the irrlicht device
     * @param _fm the font manager to use for fonts
     * @return true of a file was processed
     */
    bool display(irr::IrrlichtDevice* _device,FontManager* _fm);
  public:
    /// Default destructor
    virtual ~OpenSaveGui(){};
};

#endif // OPENSAVEGUI_H

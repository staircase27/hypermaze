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
  bool pathSelected; ///< Has an entry been selected in the path combi box
  bool itemSelected; ///< Has an entry been selected in the contents list
  bool filterChanged; ///< Has an entry been selected in the contents list
  int currentFilter; ///< The index of the currently selected filter

  /// GUI element to input the file name to save/open the maze as/from

  irr::gui::IGUIEditBox * fileField; ///< The field for the user to enter a filename in
  irr::gui::IGUIComboBox* pathfield; ///< The current path and options to select other paths
  irr::gui::IGUIListBox * contentslist; ///< The contents of the current folder
  irr::gui::IGUIComboBox* filterfield; ///< The filters available

  irr::io::IFileSystem* fs; ///< The irrlicht filesystem object
  irr::io::path path; ///< The current path
  irr::io::IFileList* rawfiles; ///< The raw list of files in the current directory
  irr::io::IFileList* filteredfiles; ///< The filtered list of files in the current directory

  int drivecount; ///< The number of drives found
  irr::io::path* drives; ///< an array of the drives found
  irr::core::stringw* drivenames; ///< an array of the names of the drives found

  /// An enum for button IDs
  enum
  {
    GUI_ID_OK_BUTTON=201, ///< ID for the ok button
    GUI_ID_CANCEL_BUTTON, ///< ID for the cancel button
    GUI_ID_PATH_COMBO, ///< ID for the path combo box
    GUI_ID_FILTER_COMBO, ///< ID for the filter combo box (if used)
    GUI_ID_CONTENTS_LIST ///< ID for the contents list box
  };
  private:
    /// @copydoc BaseGui::OnEventImpl
    virtual bool OnEventImpl(const irr::SEvent &event);
    /// @copydoc BaseGui::createGUI
    void createGUI();
    /// @copydoc BaseGui::run
    bool run();
    /// Populate the path combo box and contents list box with contents
    void populateWithFolderContents();

    /// Populate the contents list box with the filtered contents
    void populateWithFilteredContents();
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
    virtual bool process(const irr::fschar_t* file){return false;};

    /// Process the selected file
    /**
     * @param url the url to select.
     * @return true if the file is valid and has been processed successfully
     * @note if false is returned this function should show an error message first to explain the problem
     */
    virtual bool processURL(const wchar_t* url){return false;};

    /// Can the specified file be selected and if so what should happen
    /**
     * @param file the file to select.
     * @param type if the selection is file, folder or not yet created
     * @return what should be done with the path
     */
    virtual E_SELECTION_TYPE select(const irr::fschar_t* file,E_PATH_TYPE type){
      if(type==FOLDER)
        return CHANGEDIR;
      else if(type==FILE)
        return PROCESS;
      else
        return INVALID;
    };
    /// Can the specified url be selected and if so what should happen
    /**
     * @param url the url to select.
     * @return true if the url should be processed (options are equivalent to PROCESS and INVALID)
     */
    virtual bool selectURL(const wchar_t* url){
      return INVALID;
    };

    /// How many filters this gui has
    /**
     * return 0 to disable filters (the default implementation)
     * @note If this value returned by this changes while a gui is displayed the result is undefined
     * @return the filter count
     */
    virtual int filtercount() { return 0; }

    /// get a filters name
    /**
     * filter 0 will be the default
     * @note only used if filtercount returns >0.
     * @note a don't filter option will be added
     * @param i the index of the filter
     * @return a name for the filters
     */
    virtual const wchar_t* filtername(int i) { return L"Unknown filter"; }

    /// Filter the files and folders to show in the dialog
    /**
     * @note only used if filtercount returns >0.
     * @note the user will be presented an option show all files
     * @param i the index of the currently selected filter
     * @param file the file to filter
     * @param folder is the file a folder
     * @return true if the file should be shown
     */
	virtual bool filterfiles(int i, const irr::fschar_t* file, bool folder) { return true; };

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

#ifndef OPENSAVEGUI_H
#define OPENSAVEGUI_H

#include "gui.h"

class OpenSaveGui : private BaseGui
{
  private:
    /// @copydoc BaseGui::OnEventImpl
    virtual bool OnEventImpl(const irr::SEvent &event);
    /// @copydoc BaseGui::createGUI
    void createGUI();
    /// @copydoc BaseGui::run
    bool run();
  protected:
    /// Process the selected file
    /**
     * @param file the file to select.
     * @param folder if the folder is a selection
     * @param exists if the file or folder exists
     * @return true if the file is valid and has been processed
     */
    bool process(file_t file,bool folder,bool exists);

    /// Filter the files and folders to show in the dialog
    /**
     * @param file the file to filter
     * @param folder is the file a folder
     * @return true if the file should be shown
     */
    bool filterfiles(file_t file,bool folder);

  public:
    /// Show the open/save gui
    /**
     * @param _device the irrlicht device
     * @param _fm the font manager to use for fonts
     * @return true of a file was processed
     */
    bool display(irr::IrrlichtDevice* _device,FontManager* _fm);
    /// Default destructor
    virtual ~OpenSaveGui();
};

#endif // OPENSAVEGUI_H

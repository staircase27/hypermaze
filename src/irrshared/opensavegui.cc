/**
 * @file opensavegui.cc
 * @brief The implementation of opensavegui.hh
 */
#include "opensavegui.hh"
#include "irrcurl.hh"
#include "platformcompat.hh"

#include <iostream>

namespace irr{
  using namespace core;
  using namespace scene;
  using namespace io;
  using namespace video;
  using namespace gui;
};



bool OpenSaveGui::display(irr::IrrlichtDevice* _device,FontManager* _fm){
  fs=_device->getFileSystem();
  rawfiles=0;
  filteredfiles=0;
  fs->grab();
  path=fs->getWorkingDirectory();
  main(_device,_fm);
  fs->drop();
  if(rawfiles)
    rawfiles->drop();
  if(filteredfiles)
    filteredfiles->drop();
  fs=0;
  delete[] drives;
  delete[] drivenames;
  return true;
};

bool OpenSaveGui::OnEventImpl(const irr::SEvent &event){
  if(event.EventType == irr::EET_GUI_EVENT){
    if(event.GUIEvent.EventType==irr::gui::EGET_BUTTON_CLICKED){
      if(event.GUIEvent.Caller->getID()==GUI_ID_OK_BUTTON){
        okClicked=true;
        return true;
      }else{
        cancelClicked=true;
        return true;
      }
    }
    if(event.GUIEvent.EventType == irr::gui::EGET_EDITBOX_ENTER){
      okClicked=true;
      return true;
    }
    if (event.GUIEvent.EventType == irr::gui::EGET_COMBO_BOX_CHANGED) {
      if (event.GUIEvent.Caller->getID() == GUI_ID_PATH_COMBO)
        pathSelected = true;
      else
        filterChanged = true;
    }
    if(event.GUIEvent.EventType == irr::gui::EGET_LISTBOX_SELECTED_AGAIN){
      itemSelected=true;
    }
  }else if(event.EventType == irr::EET_KEY_INPUT_EVENT){
    if(event.KeyInput.Key==irr::KEY_ESCAPE){
      cancelClicked=true;
      return true;
    }else if(event.KeyInput.Key == irr::KEY_UP || event.KeyInput.Key == irr::KEY_DOWN){
      if(contentslist->OnEvent(event))
        return true;
    }else if(event.KeyInput.PressedDown && event.KeyInput.Key == irr::KEY_KEY_H && event.KeyInput.Control){
      showhidden=!showhidden;
      filterChanged=true;
      std::cout<<"Toggling hidden"<<std::endl;
      return true;
    }
  }else if(event.EventType == irr::EET_MOUSE_INPUT_EVENT && event.MouseInput.Event == irr::EMIE_MOUSE_WHEEL)
    if(contentslist->OnEvent(event))
      return true;
    
  return false;
};

void OpenSaveGui::createGUI(){
  okClicked=cancelClicked=pathSelected=itemSelected=filterChanged=showhidden=false;

  irr::IVideoDriver* driver = getDevice()->getVideoDriver();
  irr::IGUIEnvironment *guienv = getDevice()->getGUIEnvironment();

  irr::rect<irr::s32> rect=driver->getViewPort();
  irr::position2d<irr::s32> center=rect.getCenter();
  irr::dimension2d<irr::s32> size=rect.getSize();
  size.Width=min(600,size.Width-10);
  size.Height=min(600,size.Height-10);

  irr::core::stringw pathw(path);
  pathfield = guienv->addComboBox(irr::rect<irr::s32>(center.X-size.Width/2,center.Y-size.Height/2,center.X+size.Width/2,center.Y-size.Height/2+32),getTopElement(),GUI_ID_PATH_COMBO);

  irr::fschar_t* drive;
  drivecount = getDriveList(drive);
  drives = new irr::path[drivecount];
  drivenames = new irr::stringw[drivecount];
  for (int i = 0; i< drivecount; ++i) {
    drives[i] = drive;
    drive += strlen(drive) + 1;
    drivenames[i] = drive;
    drive += strlen(drive) + 1;
  }

  contentslist = guienv->addListBox(irr::rect<irr::s32>(center.X-size.Width/2,center.Y-size.Height/2+32+10,center.X+size.Width/2,center.Y+size.Height/2-32-10-32-10),getTopElement(),GUI_ID_CONTENTS_LIST,true);

  currentFilter = -1;

  populateWithFolderContents();

  fileField = guienv->addEditBox(0,irr::rect<irr::s32>(center.X-size.Width/2,center.Y+size.Height/2-32-10-32,center.X+size.Width/2,center.Y+size.Height/2-32-10),true,getTopElement());

  int filtercount = this->filtercount();
  if (filtercount > 0) {
    filterfield = guienv->addComboBox(irr::rect<irr::s32>(center.X - size.Width / 2, center.Y + size.Height / 2 - 32, center.X + size.Width / 2 - 220, center.Y + size.Height / 2), getTopElement(), GUI_ID_FILTER_COMBO);
    for (int i = 0; i < filtercount; ++i)
      filterfield->addItem(this->filtername(i), i + 1);
    filterfield->addItem(L"All (*.*)", 0);
    currentFilter = 0;
  }

  guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-210,center.Y+size.Height/2-32,center.X+size.Width/2-100,center.Y+size.Height/2),getTopElement(),GUI_ID_CANCEL_BUTTON,L"Cancel");
  guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-100,center.Y+size.Height/2-32,center.X+size.Width/2,center.Y+size.Height/2),getTopElement(),GUI_ID_OK_BUTTON,getButtonName());

  guienv->setFocus(fileField);

  getDevice()->setWindowCaption(getWindowTitle());
}

void OpenSaveGui::populateWithFolderContents(){
  pathfield->clear();
  pathfield->addItem(irr::stringw(path).c_str(),0);
  for (int i = 0; i < drivecount; ++i)
    pathfield->addItem(drivenames[i].c_str(), i + 1);
  pathfield->setSelected(0);
  if(rawfiles)
    rawfiles->drop();
  rawfiles=fs->createFileList();
  populateWithFilteredContents();
}
void OpenSaveGui::populateWithFilteredContents() {
  if (filteredfiles)
    filteredfiles->drop();
  contentslist->clear();
  filteredfiles = fs->createEmptyFileList(path, false, false);
  for (int i = 0; i < rawfiles->getFileCount(); ++i) {
    if (rawfiles->getFileName(i) == ".") 
      continue;
    if (rawfiles->getFileName(i) == ".." || ((currentFilter==-1 || filterfiles(currentFilter,rawfiles->getFileName(i).c_str(), rawfiles->isDirectory(i)))&&
        (showhidden || ! ishidden(path,rawfiles->getFileName(i))))) {
      irr::core::stringw wname(rawfiles->getFileName(i));
      filteredfiles->addItem(rawfiles->getFullFileName(i), rawfiles->getFileOffset(i),
          rawfiles->getFileSize(i), rawfiles->isDirectory(i), i);
      contentslist->addItem(wname.c_str());
    }
  }
}


bool OpenSaveGui::run(){
  if(cancelClicked)
    return false;
  if(okClicked && fileField->getText()[0]==L'\0'){
    okClicked=false;
    itemSelected=true;
  }
  if(okClicked){
    okClicked=false;
    getTopElement()->setVisible(false);
    const wchar_t* file=fileField->getText();
    if(isurl(file)){
      if(this->selectURL(file)){
        if(this->processURL(file)){
          okClicked=true;
          return false;
        }
      }
      getTopElement()->setVisible(true);
      getDevice()->getGUIEnvironment()->setFocus(fileField);
    }else{
      irr::path filepath=fs->getAbsolutePath(file);
      if(filepath.lastChar()=='/'){
        filepath.erase(filepath.size() - 1);
      }
      E_PATH_TYPE type=((irr::stringw(file)==L".")||(irr::stringw(file)==L"..") || (irr::stringw(file) == L"/") || (irr::stringw(file) == L"\\"))?FOLDER:ABSENT;
      if (type == ABSENT)
        for (int i = 0; i < drivecount; ++i)
          if (samePath(drives[i], filepath)) {
            type = FOLDER;
            break;
          }
      if(type==ABSENT){
        irr::path dir=fs->getFileDir(filepath);
        if(samePath(dir,path)){
          if(rawfiles->findFile(filepath,true)!=-1)
            type=FOLDER;
          if(type==ABSENT && rawfiles->findFile(filepath,false)!=-1)
            type=FILE;
        }else{
          dir.append("/");
          if(fs->changeWorkingDirectoryTo(dir)){
            irr::IFileList* fl=fs->createFileList();
            if(fl->findFile(filepath,true)!=-1)
              type=FOLDER;
            if(type==ABSENT && fl->findFile(filepath,false)!=-1)
              type=FILE;
            fl->drop();
          }
          fs->changeWorkingDirectoryTo(path);
        }
      }
      switch(this->select(filepath.c_str(),type)){
        case PROCESS:
          if(this->process(filepath.c_str())){
            okClicked=true;
            return false;
          }
          break;
        case CHANGEDIR:
          filepath.append("/");
          fs->changeWorkingDirectoryTo(filepath);
          path = fs->getWorkingDirectory();
          populateWithFolderContents();
          fileField->setText(L"");
          break;
      }
      getTopElement()->setVisible(true);
      getDevice()->getGUIEnvironment()->setFocus(fileField);
    }
  }
  if(itemSelected){
    itemSelected=false;
    int index=contentslist->getSelected();
    if(index>=0){
      getTopElement()->setVisible(false);
      E_PATH_TYPE type=filteredfiles->isDirectory(index)?FOLDER:FILE;
      switch(this->select(filteredfiles->getFullFileName(index).c_str(),type)){
        case PROCESS:
          if(this->process(filteredfiles->getFullFileName(index).c_str())){
            okClicked=true;
            return false;
          }
          break;
        case CHANGEDIR:
          fs->changeWorkingDirectoryTo(fs->getAbsolutePath(filteredfiles->getFileName(index)));
          path = fs->getWorkingDirectory();
          populateWithFolderContents();
          fileField->setText(L"");
          break;
      }
      getTopElement()->setVisible(true);
      getDevice()->getGUIEnvironment()->setFocus(fileField);
    }
  }
  if(pathSelected){
    pathSelected=false;
    int i = pathfield->getSelected();
    if(i>0){ // do nothing for nothing selected or current path selected
      irr::u32 data = pathfield->getItemData(i);
      if (data > 0) {
        fs->changeWorkingDirectoryTo(drives[i - 1]);
        path = fs->getWorkingDirectory();
        populateWithFolderContents();
        fileField->setText(L"");
      }
    }
    getDevice()->getGUIEnvironment()->setFocus(fileField);
  }
  if (filterChanged) {
    filterChanged = false;
    if (filterfield->getSelected() < 0)
      filterfield->setSelected(0);
    currentFilter = filterfield->getItemData(filterfield->getSelected())-1;
    populateWithFilteredContents();
  }
  return true;
}

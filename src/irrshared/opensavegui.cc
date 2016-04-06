/**
 * @file opensavegui.cc
 * @brief The implementation of opensavegui.hh
 */
#include "opensavegui.hh"
#include "irrcurl.hh"

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
  fs=0;
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
    if(event.GUIEvent.EventType == irr::gui::EGET_COMBO_BOX_CHANGED){
      pathSelected=true;
    }
    if(event.GUIEvent.EventType == irr::gui::EGET_LISTBOX_SELECTED_AGAIN){
      itemSelected=true;
    }
  }
  if(event.EventType == irr::EET_KEY_INPUT_EVENT){
    if(event.KeyInput.Key==irr::KEY_ESCAPE){
      cancelClicked=true;
      return true;
    }else if(event.KeyInput.Key == irr::KEY_UP || event.KeyInput.Key == irr::KEY_DOWN)
      if(contentslist->OnEvent(event))
        return true;
  }
  return false;
};

void OpenSaveGui::createGUI(){
  okClicked=cancelClicked=pathSelected=itemSelected=false;

  irr::IVideoDriver* driver = getDevice()->getVideoDriver();
  irr::IGUIEnvironment *guienv = getDevice()->getGUIEnvironment();

  irr::rect<irr::s32> rect=driver->getViewPort();
  irr::position2d<irr::s32> center=rect.getCenter();
  irr::dimension2d<irr::s32> size=rect.getSize();
  size.Width=min(600,size.Width-10);
  size.Height=min(600,size.Height-10);

  irr::core::stringw pathw(path);
  pathfield = guienv->addComboBox(irr::rect<irr::s32>(center.X-size.Width/2,center.Y-size.Height/2,center.X+size.Width/2,center.Y-size.Height/2+32),getTopElement(),0);

  contentslist = guienv->addListBox(irr::rect<irr::s32>(center.X-size.Width/2,center.Y-size.Height/2+32+10,center.X+size.Width/2,center.Y+size.Height/2-32-10-32-10),getTopElement(),0,true);

  populateWithFolderContents();

  fileField = guienv->addEditBox(0,irr::rect<irr::s32>(center.X-size.Width/2,center.Y+size.Height/2-32-10-32,center.X+size.Width/2,center.Y+size.Height/2-32-10),true,getTopElement());

  guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-210,center.Y+size.Height/2-32,center.X+size.Width/2-100,center.Y+size.Height/2),getTopElement(),GUI_ID_CANCEL_BUTTON,L"Cancel");
  guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-100,center.Y+size.Height/2-32,center.X+size.Width/2,center.Y+size.Height/2),getTopElement(),GUI_ID_OK_BUTTON,getButtonName());

  guienv->setFocus(fileField);

  getDevice()->setWindowCaption(getWindowTitle());
}

void OpenSaveGui::populateWithFolderContents(){
  pathfield->clear();
  pathfield->addItem(irr::stringw(path).c_str(),-1);
  pathfield->setSelected(0);
  contentslist->clear();
  if(rawfiles)
    rawfiles->drop();
  if(filteredfiles)
    filteredfiles->drop();
  rawfiles=fs->createFileList();
  filteredfiles=fs->createEmptyFileList(path,false,false);
  for(int i=0;i<rawfiles->getFileCount();++i){
    irr::core::stringw wname(rawfiles->getFileName(i));
    if(filterfiles(wname.c_str(),rawfiles->isDirectory(i))){
      filteredfiles->addItem(rawfiles->getFullFileName(i),rawfiles->getFileOffset(i),
          rawfiles->getFileSize(i),rawfiles->isDirectory(i),i);
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
      E_PATH_TYPE type=((irr::stringw(file)==L".")||(irr::stringw(file)==L"..")||(irr::stringw(file)==L"/"))?FOLDER:ABSENT;
      if(type==ABSENT){
        irr::path dir=fs->getFileDir(filepath);
        if(dir=="")
          dir="/";
        if(dir==path){
          if(rawfiles->findFile(filepath,true)!=-1)
            type=FOLDER;
          if(type==ABSENT && rawfiles->findFile(filepath,false)!=-1)
            type=FILE;
        }else{
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
          path=filepath;
          fs->changeWorkingDirectoryTo(path);
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
      switch(this->select(filteredfiles->getFileName(index).c_str(),type)){
        case PROCESS:
          if(this->process(filteredfiles->getFileName(index).c_str())){
            okClicked=true;
            return false;
          }
          break;
        case CHANGEDIR:
          path=fs->getAbsolutePath(filteredfiles->getFileName(index));
          fs->changeWorkingDirectoryTo(path);
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
    if(pathfield->getSelected()!=0){
      // do something
    }
    getDevice()->getGUIEnvironment()->setFocus(fileField);
  }
  return true;
}

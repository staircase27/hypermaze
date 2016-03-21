/**
 * @file opensavegui.cc
 * @brief The implementation of opensavegui.hh
 */
#include "opensavegui.hh"
#include "irrcurl.hh"

namespace irr{
  using namespace core;
  using namespace scene;
  using namespace io;
  using namespace video;
  using namespace gui;
};

bool OpenSaveGui::display(irr::IrrlichtDevice* _device,FontManager* _fm){
  main(_device,_fm);
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
  }
  if(event.EventType == irr::EET_KEY_INPUT_EVENT && event.KeyInput.Key==irr::KEY_ESCAPE){
    cancelClicked=true;
    return true;
  }
  return false;
};

void OpenSaveGui::createGUI(){
  okClicked=cancelClicked=false;

  irr::IVideoDriver* driver = getDevice()->getVideoDriver();
  irr::IGUIEnvironment *guienv = getDevice()->getGUIEnvironment();

  irr::rect<irr::s32> rect=driver->getViewPort();
  irr::position2d<irr::s32> center=rect.getCenter();
  irr::dimension2d<irr::s32> size=rect.getSize();
  size.Width=min(400,size.Width-10);

  fileField = guienv->addEditBox(0,irr::rect<irr::s32>(center.X-size.Width/2,center.Y-5-32,center.X+size.Width/2,center.Y-5),true,getTopElement());

  guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-210,center.Y+5,center.X+size.Width/2-100,center.Y+5+32),getTopElement(),GUI_ID_CANCEL_BUTTON,L"Cancel");
  guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-100,center.Y+5,center.X+size.Width/2,center.Y+5+32),getTopElement(),GUI_ID_OK_BUTTON,getButtonName());

  guienv->setFocus(fileField);

  getDevice()->setWindowCaption(getWindowTitle());
}

bool OpenSaveGui::run(){
  if(cancelClicked)
    return false;
  if(okClicked){
    getTopElement()->setVisible(false);
    const wchar_t* file=fileField->getText();
    E_PATH_TYPE type=isurl(file)?URL:getDevice()->getFileSystem()->existFile(file)?FILE:ABSENT;
    switch(this->select(file,type)){
      case PROCESS:
        if(this->process(file))
          return false;
        break;
      case CHANGEDIR:
        break;
    }
    getTopElement()->setVisible(true);
    getDevice()->getGUIEnvironment()->setFocus(fileField);
    okClicked=false;
  }
  return true;
}

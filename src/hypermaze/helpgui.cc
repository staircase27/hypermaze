/**
 * @file helpgui.cc
 * @brief The implementation of helpgui.hh
 */
#include "helpgui.hh"
#include "keymapgui.hh"
#include "../irrshared/GUIFormattedText.hh"

bool HelpGui::OnEventImpl(const irr::SEvent &event){
  if(event.EventType == irr::EET_GUI_EVENT){
    if(event.GUIEvent.EventType==irr::gui::EGET_BUTTON_CLICKED){
      if(event.GUIEvent.Caller->getID()==GUI_ID_OK_BUTTON){
        okClicked=true;
        return true;
      }else{
        keyMapClicked=true;
        return true;
      }
    }
    if(event.GUIEvent.EventType == irr::gui::EGET_EDITBOX_ENTER){
      okClicked=true;
      return true;
    }
  }
  if(event.EventType == irr::EET_KEY_INPUT_EVENT && event.KeyInput.PressedDown &&
      (keyblock==0||getDevice()->getTimer()->getTime()>keyblock)){
    if(event.KeyInput.Key == irr::KEY_ESCAPE || event.KeyInput.Key == irr::KEY_RETURN){
      okClicked=true;
      keyblock=0;
      return true;
    }else if(event.KeyInput.Key == irr::KEY_KEY_E){
      keyMapClicked=true;
      keyblock=0;
      return true;

    }
  }
  return false;
};


bool HelpGui::help(irr::IrrlichtDevice* _device,FontManager* _fm,KeyMap& km){
  this->km=&km;
  main(_device,_fm);
  return okClicked;
}

void HelpGui::createGUI(){
  okClicked=keyMapClicked=false;
  keyblock=0;

  irr::video::IVideoDriver* driver = getDevice()->getVideoDriver();
  irr::gui::IGUIEnvironment *guienv = getDevice()->getGUIEnvironment();

  irr::core::rect<irr::s32> rect=driver->getViewPort();
  irr::core::position2d<irr::s32> center=rect.getCenter();
  irr::core::dimension2d<irr::s32> size=rect.getSize();
  size.Width=min(600,size.Width-10);
  size.Height=min(600,size.Height-10);

  GUIFormattedText* text=new GUIFormattedText(L"Hyper Maze Puzzle Game by Staircase",
      guienv,getTopElement(),0,irr::core::rect<irr::s32>(center.X-size.Width/2,center.Y-size.Height/2,
          center.X+size.Width/2,center.Y+size.Height/2-10-32-10-32),true,true);
  text->setOverrideFont(0,getFontManager()->getFont(24,true));
  text->setAllTextAlignment(irr::gui::EGUIA_CENTER,irr::gui::EGUIA_CENTER);
  text->addText(L"\nBend, Stretch and Move the String through the maze to get it to the far"
                L" side of the hyper maze.\n Either click and drag on the controls or use the"
                L" keyboard controls set below.\n\n\nGame written by Staircase in association"
                L" with Dark Field Games. http://www.darkfieldgames.com/\nThis game is released"
                L" open source under the ***** licence. Feel free to edit and change as you like."
                L" If you make any improvements or new ideas please send them me them and I may"
                L" include them in the next release (and thank you below)");
  text->drop();

  guienv->addButton(irr::core::rect<irr::s32>(center.X-75,center.Y+size.Height/2-32-32-10,
          center.X+75,center.Y+size.Height/2-32-10),getTopElement(),GUI_ID_KEY_MAP_BUTTON,L"Edit Key Map (e)");
  irr::gui::IGUIButton* ok=guienv->addButton(
      irr::core::rect<irr::s32>(center.X+size.Width/2-100,center.Y+size.Height/2-32,
          center.X+size.Width/2,center.Y+size.Height/2),getTopElement(),GUI_ID_OK_BUTTON,L"OK");

  guienv->setFocus(ok);

  getDevice()->setWindowCaption(L"Hyper Maze: Help");
}

bool HelpGui::run(){
  if(okClicked){
    return false;
  }
  if(keyMapClicked){
    getTopElement()->setVisible(false);
    KeyMapGui kmg;
    kmg.edit(getDevice(),getFontManager(),*km);
    getTopElement()->setVisible(true);
    keyMapClicked=false;
    keyblock=getDevice()->getTimer()->getTime()+500;
  }
  return true;
}

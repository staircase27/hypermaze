#include "irrlicht.h"
#include "gui.hh"
#include "keymapgui.hh"
#include "../irrshared/GUIFormattedText.hh"

class HelpGui: BaseGui{

  bool okClicked,keyMapClicked;

  KeyMap* km;

  irr::u32 keyblock;

  enum
  {
    GUI_ID_OK_BUTTON=201,
    GUI_ID_KEY_MAP_BUTTON
  };

  protected:
    virtual bool OnEventImpl(const irr::SEvent &event){
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
      if(event.EventType == irr::EET_KEY_INPUT_EVENT && event.KeyInput.Key==irr::KEY_ESCAPE &&
          (keyblock==0||device->getTimer()->getTime()>keyblock)){
        okClicked=true;
        keyblock=0;
        return true;
      }
      return false;
    };


  public:
    bool help(irr::IrrlichtDevice* _device,FontManager* _fm,KeyMap& km){
      this->km=&km;
      main(_device,_fm);
      return okClicked;
    }

    void createGUI(){
      okClicked=keyMapClicked=false;

      irr::video::IVideoDriver* driver = device->getVideoDriver();
      irr::gui::IGUIEnvironment *guienv = device->getGUIEnvironment();

      irr::core::rect<irr::s32> rect=driver->getViewPort();
      irr::core::position2d<irr::s32> center=rect.getCenter();
      irr::core::dimension2d<irr::s32> size=rect.getSize();
      size.Width=std::min(600,size.Width-10);
      size.Height=std::min(600,size.Height-10);

      GUIFormattedText* text=new GUIFormattedText(L"Hyper Maze Puzzle Game by Staircase",guienv,el,0,irr::core::rect<irr::s32>(center.X-size.Width/2,center.Y-size.Height/2,center.X+size.Width/2,center.Y+size.Height/2-10-32-10-32),true,true);
      text->setOverrideFont(0,fm->getFont(24,true));
      text->setAllTextAlignment(irr::gui::EGUIA_CENTER,irr::gui::EGUIA_CENTER);
      text->addText(L"\nBend, Stretch and Move the String through the maze to get it to the far side of the hyper maze.\n Either click and drag on the controls or use the keyboard controls set below.\n\n\nGame written by Staircase in association with Dark Field Games. http://www.darkfieldgames.com/\nThis game is released open source under the ***** licence. Feel free to edit and change as you like. If you make any improvements or new ideas please send them me them and I may include them in the next release (and thank you below)");
      guienv->addButton(irr::core::rect<irr::s32>(center.X-75,center.Y+size.Height/2-32-32-10,center.X+75,center.Y+size.Height/2-32-10),el,GUI_ID_KEY_MAP_BUTTON,L"Edit Key Map");
      irr::gui::IGUIButton* ok=guienv->addButton(irr::core::rect<irr::s32>(center.X+size.Width/2-100,center.Y+size.Height/2-32,center.X+size.Width/2,center.Y+size.Height/2),el,GUI_ID_OK_BUTTON,L"OK");

      guienv->setFocus(ok);

      device->setWindowCaption(L"Hyper Maze Help");
    }

    bool run(){
      if(okClicked){
        return false;
      }
      if(keyMapClicked){
        el->setVisible(false);
        KeyMapGui kmg;
        kmg.edit(device,fm,*km);
        el->setVisible(true);
        keyMapClicked=false;
        keyblock=device->getTimer()->getTime()+500;
      }
      return true;
    }
};



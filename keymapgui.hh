#include "gui.hh"
#include "keymap.hh"
#include "irrlicht.h"
#include <sstream>

using namespace std;

namespace irr{
  using namespace core;
  using namespace scene;
  using namespace io;
  using namespace video;
  using namespace gui;
};

class KeyMapGui: BaseGui{

  bool okClicked;
  bool cancelClicked;
  
  irr::IGUIEditBox *statusField; 
  irr::IGUITable *table; 
  enum
  {
    GUI_ID_OK_BUTTON=201,
    GUI_ID_CANCEL_BUTTON
  };

  protected:
    virtual bool OnEventImpl(const irr::SEvent &event){
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
      }
      if(event.EventType == irr::EET_KEY_INPUT_EVENT && event.KeyInput.Key==irr::KEY_ESCAPE){
        cancelClicked=true;
        return true;
      }
      if(event.GUIEvent.EventType == irr::gui::EGET_EDITBOX_ENTER){
        okClicked=true;
        return true;
      }
      if(event.EventType == irr::EET_KEY_INPUT_EVENT&&event.KeyInput.PressedDown){
        cout<<event.KeyInput.Char<<L" or "<<event.KeyInput.Key<<" ("<<event.KeyInput.Shift<<","<<event.KeyInput.Control<<")";
        wstringstream ss;
        ss<<(int)event.KeyInput.Char<<L" or "<<event.KeyInput.Key<<" ("<<event.KeyInput.Shift<<","<<event.KeyInput.Control<<")";
        statusField->setText(ss.str().c_str());
      }
      return false;
    };


  public:
    bool edit(irr::IrrlichtDevice* _device,Maze& m){
      apply(_device);
      okClicked=cancelClicked=false;
      
      irr::IVideoDriver* driver = device->getVideoDriver();
      irr::ISceneManager* smgr = device->getSceneManager();
      irr::IGUIEnvironment *guienv = device->getGUIEnvironment();
      
      irr::rect<irr::s32> rect=driver->getViewPort();
      irr::position2d<irr::s32> center=rect.getCenter();
      irr::dimension2d<irr::s32> size=rect.getSize();
      size.Width=min(400,size.Width-10);
      
      table = guienv->addTable(irr::rect<irr::s32>(center.X-size.Width/2,10,center.X+size.Width/2,center.Y-5-32-10),0,-1,true);
      
      statusField = guienv->addEditBox(0,irr::rect<irr::s32>(center.X-size.Width/2,center.Y-5-32,center.X+size.Width/2,center.Y-5));
      
      guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-210,center.Y+5,center.X+size.Width/2-100,center.Y+5+32),0,GUI_ID_CANCEL_BUTTON,L"Cancel");
      guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-100,center.Y+5,center.X+size.Width/2,center.Y+5+32),0,GUI_ID_OK_BUTTON,L"Open");
      
      guienv->setFocus(statusField);
        
      device->setWindowCaption(L"Open Hyper Maze");
      
      while(true)
      {
        if(!device->run())
          break;
        
        driver->beginScene(true, true, irr::SColor(255,113,113,133));

        smgr->drawAll();
        
        guienv->drawAll();

        driver->endScene();

        
        if(cancelClicked)
          break;
        if(okClicked){
          break;
        }
      }
      guienv->clear();
      unapply();
      return okClicked;
    }
};


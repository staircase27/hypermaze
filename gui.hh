#include "irrlicht.h"
#include "irrdisp.hh"
#include "maze.hh"
#include "string.hh"
#include <fstream>
using namespace std;

namespace irr{
  using namespace core;
  using namespace scene;
  using namespace io;
  using namespace video;
  using namespace gui;
};

class BaseGui : irr::IEventReceiver{
  protected:
    irr::IrrlichtDevice* device;
    irr::IEventReceiver* oldReceiver;
  
    BaseGui():device(0){};
      
    bool OnEvent(const irr::SEvent &event){
      if(oldReceiver && event.EventType == irr::EET_KEY_INPUT_EVENT && !event.KeyInput.PressedDown)
        oldReceiver->OnEvent(event);
      return OnEventImpl(event);
    };
    virtual bool OnEventImpl(const irr::SEvent &event)=0;
  
    void apply(irr::IrrlichtDevice* _device){
      if(device)
        unapply();
      device=_device;
      oldReceiver=device->getEventReceiver();
      device->setEventReceiver(this);
    }
    void unapply(){
      if(device)
        device->setEventReceiver(oldReceiver);
      device=0;
    };
};


class GenerateGui: BaseGui{

  bool okClicked;
  bool cancelClicked;
  
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
         if(event.EventType == irr::EET_KEY_INPUT_EVENT && event.KeyInput.Key==irr::KEY_ESCAPE){
           cancelClicked=true;
           return true;
         }
         if(event.GUIEvent.EventType == irr::gui::EGET_EDITBOX_ENTER){
           okClicked=true;
           return true;
         }
      }
      return false;
    };


  public:
    bool generate(irr::IrrlichtDevice* _device,Maze& m){
      apply(_device);
      okClicked=cancelClicked=false;
      
      irr::IVideoDriver* driver = device->getVideoDriver();
      irr::ISceneManager* smgr = device->getSceneManager();
      irr::IGUIEnvironment *guienv = device->getGUIEnvironment();
      
      irr::rect<irr::s32> rect=driver->getViewPort();
      irr::position2d<irr::s32> center=rect.getCenter();
      irr::dimension2d<irr::s32> size=rect.getSize();
      size.Width=min(400,size.Width-10);
      
      irr::gui::IGUISpinBox* xSize=guienv->addSpinBox(L"5",irr::rect<irr::s32>(center.X-size.Width/2,center.Y-5-32-10-32,
          center.X+size.Width/2,center.Y-5-32-10));
      xSize->setDecimalPlaces(0);
      xSize->setRange(3,100);
      xSize->setValue(m.size.X);
      irr::gui::IGUISpinBox* ySize=guienv->addSpinBox(L"5",irr::rect<irr::s32>(center.X-size.Width/2,center.Y-5-32,
          center.X+size.Width/2,center.Y-5));
      ySize->setDecimalPlaces(0);
      ySize->setRange(3,100);
      ySize->setValue(m.size.Y);
      irr::gui::IGUISpinBox* zSize=guienv->addSpinBox(L"5",irr::rect<irr::s32>(center.X-size.Width/2,center.Y+5,
          center.X+size.Width/2,center.Y+5+32));
      zSize->setDecimalPlaces(0);
      zSize->setRange(3,100);
      zSize->setValue(m.size.Z);
      
      guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-210,center.Y+5+32+10,center.X+size.Width/2-100,center.Y+5+32+10+32),0,GUI_ID_CANCEL_BUTTON,L"Cancel");
      guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-100,center.Y+5+32+10,center.X+size.Width/2,center.Y+5+32+10+32),0,GUI_ID_OK_BUTTON,L"Generate");
      
      guienv->setFocus(xSize);
      
      device->setWindowCaption(L"Generate New Hyper Maze");
        
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
          m=::generate(Vector(xSize->getValue(),ySize->getValue(),zSize->getValue()));
          break;
        }
      }

      
      guienv->clear();
      unapply();
      return okClicked;
    }
};

class SaveGui: BaseGui{

  bool okClicked;
  bool cancelClicked;
  
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
         if(event.EventType == irr::EET_KEY_INPUT_EVENT && event.KeyInput.Key==irr::KEY_ESCAPE){
           cancelClicked=true;
           return true;
         }
         if(event.GUIEvent.EventType == irr::gui::EGET_EDITBOX_ENTER){
           okClicked=true;
           return true;
         }
      }
      return false;
    };


  public:
    bool save(irr::IrrlichtDevice* _device,Maze& m){
      apply(_device);
      okClicked=cancelClicked=false;
      
      irr::IVideoDriver* driver = device->getVideoDriver();
      irr::ISceneManager* smgr = device->getSceneManager();
      irr::IGUIEnvironment *guienv = device->getGUIEnvironment();
      
      irr::rect<irr::s32> rect=driver->getViewPort();
      irr::position2d<irr::s32> center=rect.getCenter();
      irr::dimension2d<irr::s32> size=rect.getSize();
      size.Width=min(400,size.Width-10);
      
      irr::IGUIEditBox * fileField = guienv->addEditBox(0,irr::rect<irr::s32>(center.X-size.Width/2,center.Y-5-32,center.X+size.Width/2,center.Y-5));
      
      guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-210,center.Y+5,center.X+size.Width/2-100,center.Y+5+32),0,GUI_ID_CANCEL_BUTTON,L"Cancel");
      guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-100,center.Y+5,center.X+size.Width/2,center.Y+5+32),0,GUI_ID_OK_BUTTON,L"Save");
      
      guienv->setFocus(fileField);
        
      device->setWindowCaption(L"Save Hyper Maze");
      
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
          irr::stringc fname(fileField->getText());
          ofstream ofs(fname.c_str());
          ofs<<m;
          ofs.close();
          break;
        }
      }
      guienv->clear();
      unapply();
      return okClicked;
    }
};


class OpenGui: BaseGui{

  bool okClicked;
  bool cancelClicked;
  
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
         if(event.EventType == irr::EET_KEY_INPUT_EVENT && event.KeyInput.Key==irr::KEY_ESCAPE){
           cancelClicked=true;
           return true;
         }
         if(event.GUIEvent.EventType == irr::gui::EGET_EDITBOX_ENTER){
           okClicked=true;
           return true;
         }
      }
      return false;
    };


  public:
    bool open(irr::IrrlichtDevice* _device,Maze& m){
      apply(_device);
      okClicked=cancelClicked=false;
      
      irr::IVideoDriver* driver = device->getVideoDriver();
      irr::ISceneManager* smgr = device->getSceneManager();
      irr::IGUIEnvironment *guienv = device->getGUIEnvironment();
      
      irr::rect<irr::s32> rect=driver->getViewPort();
      irr::position2d<irr::s32> center=rect.getCenter();
      irr::dimension2d<irr::s32> size=rect.getSize();
      size.Width=min(400,size.Width-10);
      
      irr::IGUIEditBox * fileField = guienv->addEditBox(0,irr::rect<irr::s32>(center.X-size.Width/2,center.Y-5-32,center.X+size.Width/2,center.Y-5));
      
      guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-210,center.Y+5,center.X+size.Width/2-100,center.Y+5+32),0,GUI_ID_CANCEL_BUTTON,L"Cancel");
      guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-100,center.Y+5,center.X+size.Width/2,center.Y+5+32),0,GUI_ID_OK_BUTTON,L"Open");
      
      guienv->setFocus(fileField);
        
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
          irr::stringc fname(fileField->getText());
          ifstream ifs(fname.c_str());
          ifs>>m;
          ifs.close();
          break;
        }
      }
      guienv->clear();
      unapply();
      return okClicked;
    }
};



#include "irrlicht.h"
#include "irrdisp.hh"
#include "maze.hh"
#include "string.hh"
#include "mazegen.hh"
#include <fstream>

#ifndef GUI_HH_INC
#define GUI_HH_INC
using namespace std;

namespace irr{
  using namespace core;
  using namespace scene;
  using namespace io;
  using namespace video;
  using namespace gui;
};

static const int MGUIET_EMPTY = irr::EGUIET_COUNT+1;

class CGUIEmptyElement : public irr::IGUIElement
{
  public:
    CGUIEmptyElement(irr::IGUIEnvironment* environment, irr::IGUIElement* parent)
        :irr::IGUIElement((irr::EGUI_ELEMENT_TYPE)MGUIET_EMPTY, environment, parent, -1, irr::rect<irr::s32>(0,0,100000,10000)) {}

    irr::IGUIEnvironment* GetEnvironment() { return Environment; }

    virtual const irr::c8* getTypeName() const   { return "empty"; }

    virtual bool isPointInside(const irr::position2d<irr::s32>& point) const { return false; }
    
    virtual bool bringToFront(IGUIElement* element)
    {
      bool result = IGUIElement::bringToFront(element);
      if ( Parent )
      {
        Parent->bringToFront(this);
      }
      return result;
    }
};

class BaseGui : irr::IEventReceiver{
  protected:
    irr::IrrlichtDevice* device;
    irr::IEventReceiver* oldReceiver;
    CGUIEmptyElement* el;

    BaseGui():device(0){};

    bool OnEvent(const irr::SEvent &event){
      if(oldReceiver && event.EventType == irr::EET_KEY_INPUT_EVENT && !event.KeyInput.PressedDown)
        oldReceiver->OnEvent(event);
      if(oldReceiver && event.EventType == irr::EET_MOUSE_INPUT_EVENT && (event.MouseInput.Event==irr::EMIE_LMOUSE_LEFT_UP || event.MouseInput.Event==irr::EMIE_RMOUSE_LEFT_UP || event.MouseInput.Event==irr::EMIE_MMOUSE_LEFT_UP))
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
    
    virtual void createGUI(){};
    
    virtual bool run(){};
    
    void main(irr::IrrlichtDevice* _device){
      apply(_device);

      irr::IVideoDriver* driver = device->getVideoDriver();
      irr::ISceneManager* smgr = device->getSceneManager();
      irr::IGUIEnvironment *guienv = device->getGUIEnvironment();
      
      el=new CGUIEmptyElement(guienv,guienv->getRootGUIElement());
      createGUI();

      while(device->run()){
        
        if(!run())
          break;
        
        driver->beginScene(true, true, irr::SColor(255,113,113,133));
        smgr->drawAll();
        guienv->drawAll();
        driver->endScene();
      }
      
      unapply();
      guienv->removeFocus(guienv->getFocus());
      el->remove();
    }
};


class GenerateGui: BaseGui{

  bool okClicked;
  bool cancelClicked;

  Maze* m;
  irr::gui::IGUISpinBox *xSize, *ySize, *zSize;

  enum
  {
    GUI_ID_OK_BUTTON=201,
    GUI_ID_CANCEL_BUTTON
  };

  protected:
    virtual bool OnEventImpl(const irr::SEvent &event){
      if(event.EventType == irr::EET_GUI_EVENT){
        if(event.GUIEvent.EventType==irr::EGET_BUTTON_CLICKED){
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


  public:
    bool generate(irr::IrrlichtDevice* _device,Maze& m){
      this->m=&m;
      main(_device);
      return okClicked;
    }
    
    virtual void createGUI(){
      okClicked=cancelClicked=false;

      irr::IVideoDriver* driver = device->getVideoDriver();
      irr::IGUIEnvironment *guienv = device->getGUIEnvironment();

      irr::rect<irr::s32> rect=driver->getViewPort();
      irr::position2d<irr::s32> center=rect.getCenter();
      irr::dimension2d<irr::s32> size=rect.getSize();
      size.Width=min(400,size.Width-10);

      xSize=guienv->addSpinBox(L"5",irr::rect<irr::s32>(center.X-size.Width/2,center.Y-5-32-10-32,
          center.X+size.Width/2,center.Y-5-32-10),true,el);
      xSize->setDecimalPlaces(0);
      xSize->setRange(3,100);
      xSize->setValue(m->size.X);
      
      ySize=guienv->addSpinBox(L"5",irr::rect<irr::s32>(center.X-size.Width/2,center.Y-5-32,
          center.X+size.Width/2,center.Y-5),true,el);
      ySize->setDecimalPlaces(0);
      ySize->setRange(3,100);
      ySize->setValue(m->size.Y);
      
      zSize=guienv->addSpinBox(L"5",irr::rect<irr::s32>(center.X-size.Width/2,center.Y+5,
          center.X+size.Width/2,center.Y+5+32),true,el);
      zSize->setDecimalPlaces(0);
      zSize->setRange(3,100);
      zSize->setValue(m->size.Z);

      guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-210,center.Y+5+32+10,center.X+size.Width/2-100,center.Y+5+32+10+32),el,GUI_ID_CANCEL_BUTTON,L"Cancel");
      guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-100,center.Y+5+32+10,center.X+size.Width/2,center.Y+5+32+10+32),el,GUI_ID_OK_BUTTON,L"Generate");

      guienv->setFocus(xSize->getEditBox());

      device->setWindowCaption(L"Generate New Hyper Maze");

    }
    
    
    virtual bool run(){
      if(cancelClicked)
        return false;
      if(okClicked){
        (*m)=::generate<RandLimitMazeGenHalf<Hunter<RandOrderWalker<DiagonalWalker> > > >(Vector(xSize->getValue(),ySize->getValue(),zSize->getValue()));
        return false;
      }
      return true;
    }

};

class SaveGui: BaseGui{

  bool okClicked;
  bool cancelClicked;

  Maze* m;
  irr::IGUIEditBox * fileField;

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
      return false;
    };


  public:
    bool save(irr::IrrlichtDevice* _device,Maze& m){
      this->m=&m;
      main(_device);
      return okClicked;
   }
   
    virtual void createGUI(){
      okClicked=cancelClicked=false;

      irr::IVideoDriver* driver = device->getVideoDriver();
      irr::IGUIEnvironment *guienv = device->getGUIEnvironment();

      irr::rect<irr::s32> rect=driver->getViewPort();
      irr::position2d<irr::s32> center=rect.getCenter();
      irr::dimension2d<irr::s32> size=rect.getSize();
      size.Width=min(400,size.Width-10);

      fileField = guienv->addEditBox(0,irr::rect<irr::s32>(center.X-size.Width/2,center.Y-5-32,center.X+size.Width/2,center.Y-5),true,el);

      guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-210,center.Y+5,center.X+size.Width/2-100,center.Y+5+32),el,GUI_ID_CANCEL_BUTTON,L"Cancel");
      guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-100,center.Y+5,center.X+size.Width/2,center.Y+5+32),el,GUI_ID_OK_BUTTON,L"Save");

      guienv->setFocus(fileField);

      device->setWindowCaption(L"Save Hyper Maze");

    }
    
    bool run(){
      if(cancelClicked)
        return false;
      if(okClicked){
        irr::stringc fname(fileField->getText());
        ofstream ofs(fname.c_str());
        ofs<<m;
        ofs.close();
        return false;
      } 
      return true;
    }
};


class OpenGui: BaseGui{

  bool okClicked;
  bool cancelClicked;

  Maze* m;
  irr::IGUIEditBox * fileField;
  
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
      return false;
    };


  public:
    bool open(irr::IrrlichtDevice* _device,Maze& m){
      this->m=&m;
      main(_device);
      return okClicked;
    }
    
    void createGUI(){
      okClicked=cancelClicked=false;

      irr::IVideoDriver* driver = device->getVideoDriver();
      irr::IGUIEnvironment *guienv = device->getGUIEnvironment();

      irr::rect<irr::s32> rect=driver->getViewPort();
      irr::position2d<irr::s32> center=rect.getCenter();
      irr::dimension2d<irr::s32> size=rect.getSize();
      size.Width=min(400,size.Width-10);

      fileField = guienv->addEditBox(0,irr::rect<irr::s32>(center.X-size.Width/2,center.Y-5-32,center.X+size.Width/2,center.Y-5),true,el);

      guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-210,center.Y+5,center.X+size.Width/2-100,center.Y+5+32),el,GUI_ID_CANCEL_BUTTON,L"Cancel");
      guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-100,center.Y+5,center.X+size.Width/2,center.Y+5+32),el,GUI_ID_OK_BUTTON,L"Open");

      guienv->setFocus(fileField);

      device->setWindowCaption(L"Open Hyper Maze");
    }
    
    bool run(){
      if(cancelClicked)
        return false;
      if(okClicked){
        irr::stringc fname(fileField->getText());
        ifstream ifs(fname.c_str());
        ifs>>*m;
        ifs.close();
        return false;
      }
      return true;
    }
};

#endif

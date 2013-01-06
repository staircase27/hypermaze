//    irr::IMesh* mesh=smgr->getMesh("irrlicht/dfg.obj");
//    irr::IMeshSceneNode* node = smgr->addMeshSceneNode(mesh);
//    node->setMaterialTexture( 0, driver->getTexture("irrlicht/dfg.png"));

#include "irrlicht.h"
#include "gui.hh"
#include "keymapgui.hh"

using namespace std;

namespace irr{
  using namespace core;
  using namespace scene;
  using namespace io;
  using namespace video;
  using namespace gui;
};

class HelpGui: BaseGui{

  bool okClicked,keyMapClicked;
  
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
      }
      if(event.EventType == irr::EET_KEY_INPUT_EVENT && event.KeyInput.Key==irr::KEY_ESCAPE){
        okClicked=true;
        return true;
      }
      if(event.GUIEvent.EventType == irr::gui::EGET_EDITBOX_ENTER){
        okClicked=true;
        return true;
      }
      return false;
    };


  public:
    bool help(irr::IrrlichtDevice* _device,KeyMap& km){
      okClicked=keyMapClicked=false;
      apply(_device);
      
      irr::IVideoDriver* driver = device->getVideoDriver();
      irr::ISceneManager* smgr = device->getSceneManager();
      irr::IGUIEnvironment *guienv = device->getGUIEnvironment();
      
      irr::rect<irr::s32> rect=driver->getViewPort();
      irr::position2d<irr::s32> center=rect.getCenter();
      irr::dimension2d<irr::s32> size=rect.getSize();
      size.Width=min(600,size.Width-10);
      size.Height=min(600,size.Height-10);
      
      guienv->addStaticText(L"Hyper Maze Puzzle Game by Staircase\n\nBend, Stretch and Move the String through the maze to get it to the far side of the hyper maze.\n Either click and drag on the controls or use the keyboard controls set below.\n\n\nGame written by Staircase in association with Dark Field Games. http://www.darkfieldgames.com/\nThis game is released open source under the ***** licence. Feel free to edit and change as you like. If you make any improvements or new ideas please send them me them and I may include them in the next release (and thank you below)",irr::rect<irr::s32>(center.X-size.Width/2,center.Y-size.Height/2,center.X+size.Width/2,center.Y+size.Height/2-10-32-10-32),true,true,0,-1,true)->setTextAlignment(irr::EGUIA_CENTER,irr::EGUIA_CENTER);
      guienv->addButton(irr::rect<irr::s32>(center.X-75,center.Y+size.Height/2-32-32-10,center.X+75,center.Y+size.Height/2-32-10),0,GUI_ID_KEY_MAP_BUTTON,L"Edit Key Map");
      guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-100,center.Y+size.Height/2-32,center.X+size.Width/2,center.Y+size.Height/2),0,GUI_ID_OK_BUTTON,L"OK");
      
//      guienv->setFocus();
        
      device->setWindowCaption(L"Hyper Maze Help");
      
      while(true)
      {
        if(!device->run())
          break;
        
        driver->beginScene(true, true, irr::SColor(255,113,113,133));

        smgr->drawAll();
        
        guienv->drawAll();

        driver->endScene();

        
        if(okClicked){
          break;
        }
        if(keyMapClicked){
          guienv->clear();
          unapply();
          KeyMapGui kmg;
          return kmg.edit(_device,km);
        }
      }
      guienv->clear();
      unapply();
      return okClicked;
    }
};

class WinGui: BaseGui{

  bool okClicked,generateClicked,loadClicked, saveClicked;
  
  enum
  {
    GUI_ID_OK_BUTTON=201,
    GUI_ID_GENERATE_BUTTON,
    GUI_ID_LOAD_BUTTON,
    GUI_ID_SAVE_BUTTON
  };

  protected:
    virtual bool OnEventImpl(const irr::SEvent &event){
      if(event.EventType == irr::EET_GUI_EVENT){
        if(event.GUIEvent.EventType==irr::gui::EGET_BUTTON_CLICKED){
          switch(event.GUIEvent.Caller->getID()){
            case GUI_ID_OK_BUTTON:
              okClicked=true;
              return true;
            case GUI_ID_GENERATE_BUTTON:
              generateClicked=true;
              return true;
            case GUI_ID_LOAD_BUTTON:
              loadClicked=true;
              return true;
            case GUI_ID_SAVE_BUTTON:
              saveClicked=true;
              return true;
          }
          return false;
        }
        if(event.GUIEvent.EventType == irr::gui::EGET_EDITBOX_ENTER){
          okClicked=true;
          return true;
        }
      }
      if(event.EventType == irr::EET_KEY_INPUT_EVENT && event.KeyInput.Key==irr::KEY_ESCAPE){
        okClicked=true;
        return true;
      }
      return false;
    };


  public:
    bool won(irr::IrrlichtDevice* _device,PuzzleDisplay& pd){
      okClicked=generateClicked=loadClicked=saveClicked=false;
      apply(_device);
      
      irr::IVideoDriver* driver = device->getVideoDriver();
      irr::ISceneManager* smgr = device->getSceneManager();
      irr::IGUIEnvironment *guienv = device->getGUIEnvironment();
      
      irr::rect<irr::s32> rect=driver->getViewPort();
      irr::position2d<irr::s32> center=rect.getCenter();
      irr::dimension2d<irr::s32> size=rect.getSize();
      size.Width=min(600,size.Width-10);
      size.Height=min(600,size.Height-10);
      
      guienv->addStaticText(L"Congratulations!!!\n\n\nWhat would you like to do now?",irr::rect<irr::s32>(center.X-size.Width/2,center.Y-size.Height/2,center.X+size.Width/2,center.Y+size.Height/2-10-32-10-32),true,true,0,-1,true)->setTextAlignment(irr::EGUIA_CENTER,irr::EGUIA_CENTER);
      guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-120,center.Y+size.Height/2-32-32-10,center.X+size.Width/2,center.Y+size.Height/2-32-10),0,GUI_ID_SAVE_BUTTON,L"Save Maze");
      guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-120,center.Y+size.Height/2-32,center.X+size.Width/2,center.Y+size.Height/2),0,GUI_ID_LOAD_BUTTON,L"Load a Maze");
      guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-300,center.Y+size.Height/2-32,center.X+size.Width/2-130,center.Y+size.Height/2),0,GUI_ID_GENERATE_BUTTON,L"Generate New Maze");
      guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-500,center.Y+size.Height/2-32,center.X+size.Width/2-310,center.Y+size.Height/2),0,GUI_ID_OK_BUTTON,L"Back to Current Maze");
      
//      guienv->setFocus();
        
      device->setWindowCaption(L"Hyper Maze Help");
      
      while(true)
      {
        if(!device->run())
          break;
        
        driver->beginScene(true, true, irr::SColor(255,113,113,133));

        smgr->drawAll();
        
        guienv->drawAll();

        driver->endScene();

        
        if(okClicked){
          break;
        }
        if(saveClicked){
          guienv->clear();
          unapply();
          SaveGui sg;
          return sg.save(_device,pd.m);
        }
        if(loadClicked){
          guienv->clear();
          unapply();
          OpenGui og;
          if(og.open(_device,pd.m)){
            pd.mazeUpdated();
          }
          return true;
        }
        if(generateClicked){
          guienv->clear();
          unapply();
          GenerateGui gg;
          if(gg.generate(_device,pd.m)){
            pd.mazeUpdated();
          }
          return true;
        }
      }
      guienv->clear();
      unapply();
      return okClicked;
    }
};


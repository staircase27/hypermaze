//    irr::IMesh* mesh=smgr->getMesh("irrlicht/dfg.obj");
//    irr::IMeshSceneNode* node = smgr->addMeshSceneNode(mesh);
//    node->setMaterialTexture( 0, driver->getTexture("irrlicht/dfg.png"));

#include "irrlicht.h"
#include "gui.hh"
#include "keymapgui.hh"
#include "GUIFormattedText.hh"

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
    bool help(irr::IrrlichtDevice* _device,KeyMap& km){
      this->km=&km;
      main(_device);
      return okClicked;
    }
    
    void createGUI(){
      okClicked=keyMapClicked=false;

      irr::IVideoDriver* driver = device->getVideoDriver();
      irr::IGUIEnvironment *guienv = device->getGUIEnvironment();
      
      irr::rect<irr::s32> rect=driver->getViewPort();
      irr::position2d<irr::s32> center=rect.getCenter();
      irr::dimension2d<irr::s32> size=rect.getSize();
      size.Width=min(600,size.Width-10);
      size.Height=min(600,size.Height-10);
      
      GUIFormattedText* text=new GUIFormattedText(L"Hyper Maze Puzzle Game by Staircase",guienv,el,0,irr::rect<irr::s32>(center.X-size.Width/2,center.Y-size.Height/2,center.X+size.Width/2,center.Y+size.Height/2-10-32-10-32),true,true);
      text->setOverrideFont(0,guienv->getBuiltInFont());
      text->setAllTextAlignment(irr::EGUIA_CENTER,irr::EGUIA_CENTER);
      text->addText(L"\nBend, Stretch and Move the String through the maze to get it to the far side of the hyper maze.\n Either click and drag on the controls or use the keyboard controls set below.\n\n\nGame written by Staircase in association with Dark Field Games. http://www.darkfieldgames.com/\nThis game is released open source under the ***** licence. Feel free to edit and change as you like. If you make any improvements or new ideas please send them me them and I may include them in the next release (and thank you below)");
      guienv->addButton(irr::rect<irr::s32>(center.X-75,center.Y+size.Height/2-32-32-10,center.X+75,center.Y+size.Height/2-32-10),el,GUI_ID_KEY_MAP_BUTTON,L"Edit Key Map");
      irr::IGUIButton* ok=guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-100,center.Y+size.Height/2-32,center.X+size.Width/2,center.Y+size.Height/2),el,GUI_ID_OK_BUTTON,L"OK");
      
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
        kmg.edit(device,*km);
        el->setVisible(true);
        keyMapClicked=false;
        keyblock=device->getTimer()->getTime()+500;
      }
      return true;
    }
};


class WinGui: BaseGui{

  bool okClicked,generateClicked,loadClicked, saveClicked;
  
  PuzzleDisplay* pd;
  
  irr::u32 keyblock;
  
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
      if(event.EventType == irr::EET_KEY_INPUT_EVENT && event.KeyInput.Key==irr::KEY_ESCAPE && 
          (keyblock==0||device->getTimer()->getTime()>keyblock)){
        okClicked=true;
        keyblock=0;
        return true;
      }
      return false;
    };


  public:
    bool won(irr::IrrlichtDevice* _device,PuzzleDisplay& pd){
      this->pd=&pd;
      main(_device);
      return true;
    }
    
    void createGUI(){
      okClicked=generateClicked=loadClicked=saveClicked=false;
      keyblock=0;
      
      irr::IVideoDriver* driver = device->getVideoDriver();
      irr::IGUIEnvironment *guienv = device->getGUIEnvironment();
      
      irr::rect<irr::s32> rect=driver->getViewPort();
      irr::position2d<irr::s32> center=rect.getCenter();
      irr::dimension2d<irr::s32> size=rect.getSize();
      size.Width=min(600,size.Width-10);
      size.Height=min(600,size.Height-10);
      
      GUIFormattedText* text=new GUIFormattedText(L"Congratulations!",guienv,el,0,irr::rect<irr::s32>(center.X-size.Width/2,center.Y-size.Height/2,center.X+size.Width/2,center.Y+size.Height/2-10-32-10-32),true,true);
      text->setOverrideFont(0,guienv->getBuiltInFont());
      text->setAllTextAlignment(irr::EGUIA_CENTER,irr::EGUIA_CENTER);
      text->addText(L"\nWhat would you like to do now?");
      guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-120,center.Y+size.Height/2-32-32-10,center.X+size.Width/2,center.Y+size.Height/2-32-10),el,GUI_ID_SAVE_BUTTON,L"Save Maze");
      guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-120,center.Y+size.Height/2-32,center.X+size.Width/2,center.Y+size.Height/2),el,GUI_ID_LOAD_BUTTON,L"Load a Maze");
      guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-300,center.Y+size.Height/2-32,center.X+size.Width/2-130,center.Y+size.Height/2),el,GUI_ID_GENERATE_BUTTON,L"Generate New Maze");
      irr::IGUIButton* back=guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-500,center.Y+size.Height/2-32,center.X+size.Width/2-310,center.Y+size.Height/2),el,GUI_ID_OK_BUTTON,L"Back to Current Maze");
      
      guienv->setFocus(back);
        
      device->setWindowCaption(L"Hyper Maze Help");
    }
    bool run(){
      if(okClicked){
        return false;
      }
      if(saveClicked){
        el->setVisible(false);
        SaveGui sg;
        sg.save(device,pd->m);
        el->setVisible(true);
        saveClicked=false;
        keyblock=device->getTimer()->getTime()+500;
      }
      if(loadClicked){
        el->setVisible(false);
        OpenGui og;
        if(og.open(device,pd->m)){
          pd->mazeUpdated();
          return false;
        }
        el->setVisible(true);
        loadClicked=false;
        keyblock=device->getTimer()->getTime()+500;
      }
      if(generateClicked){
        el->setVisible(false);
        GenerateGui gg;
        if(gg.generate(device,pd->m)){
          pd->mazeUpdated();
          return false;
        }
        el->setVisible(true);
        generateClicked=false;
        keyblock=device->getTimer()->getTime()+500;
      }
      return true;
    }
};


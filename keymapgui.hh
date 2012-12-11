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

wstring toString(KeySpec ks){
  wstringstream ss;
  if(ks.chr==0)
    if(ks.key==irr::KEY_KEY_CODES_COUNT || ks.key==0)
      return L"None";
    else{
      if(ks.control)
        ss<<"Ctrl+";
      if(ks.shift)
        ss<<"Shift+";
      ss<<"{key "<<ks.key<<"}";
      return ss.str();
    }
  else{
    if(ks.control)
      ss<<"Ctrl+";
    if(ks.shift)
      ss<<"Shift+";
    ss<<ks.chr;
    return ss.str();
  }
}

      

class KeyMapGui: BaseGui{

  KeyMap* km;

  bool okClicked;
  bool cancelClicked;
  
  irr::IGUITable *table;
  int editing;
  
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
        if(event.GUIEvent.EventType==irr::EGET_TABLE_SELECTED_AGAIN){
          if(editing>=0)
            table->setCellText(editing,1,toString(km->getKeySpec(KeyMap::actionNames[editing].first)).c_str());
          editing=table->getSelected();
          table->setCellText(editing,1,L"Press New Key");
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
      if(event.EventType == irr::EET_KEY_INPUT_EVENT&&event.KeyInput.PressedDown){
        cout<<event.KeyInput.Char<<L" or "<<event.KeyInput.Key<<" ("<<event.KeyInput.Shift<<","<<event.KeyInput.Control<<")";
        if(editing>=0){
          KeySpec ks(event.KeyInput.Char,event.KeyInput.Shift,event.KeyInput.Control);
          if(event.KeyInput.Char==0)
            ks=KeySpec(event.KeyInput.Key,event.KeyInput.Shift,event.KeyInput.Control);
          KeyMap::Action old=km->addMapping(ks,KeyMap::actionNames[editing].first).second;
          table->setCellText(old-1,1,toString(km->getKeySpec(old)).c_str());
          table->setCellText(editing,1,toString(km->getKeySpec(KeyMap::actionNames[editing].first)).c_str());
         
          editing=-1;
          return true;
        }
      }
      return false;
    };


  public:
    bool edit(irr::IrrlichtDevice* _device,KeyMap& km){
      this->km=&km;
      editing=-1;
      okClicked=cancelClicked=false;
      apply(_device);
      
      irr::IVideoDriver* driver = device->getVideoDriver();
      irr::ISceneManager* smgr = device->getSceneManager();
      irr::IGUIEnvironment *guienv = device->getGUIEnvironment();
      
      irr::rect<irr::s32> rect=driver->getViewPort();
      irr::position2d<irr::s32> center=rect.getCenter();
      irr::dimension2d<irr::s32> size=rect.getSize();
      size.Width=min(600,size.Width-10);
      size.Height=min(600,size.Height-10);
      
      table = guienv->addTable(irr::rect<irr::s32>(center.X-size.Width/2,center.Y-size.Height/2,center.X+size.Width/2,center.Y+size.Height/2-10-32),0,-1,true);
      table->addColumn(L"Action",0);
      table->addColumn(L"Key",1);
      table->setColumnWidth(0,size.Width/2-10);
      table->setColumnWidth(1,size.Width/2-10);
      table->setColumnOrdering(0,irr::EGCO_NONE);
      table->setColumnOrdering(1,irr::EGCO_NONE);
      for(int i=0;i<KeyMap::A_COUNT-1;++i){
        table->addRow(i);
        table->setCellText(i,0,KeyMap::actionNames[i].second.c_str());
        table->setCellText(i,1,toString(km.getKeySpec(KeyMap::actionNames[i].first)).c_str());
      }
      
      guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-210,center.Y+size.Height/2-32,center.X+size.Width/2-100,center.Y+size.Height/2),0,GUI_ID_CANCEL_BUTTON,L"Cancel");
      guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-100,center.Y+size.Height/2-32,center.X+size.Width/2,center.Y+size.Height/2),0,GUI_ID_OK_BUTTON,L"Open");
      
      guienv->setFocus(table);
        
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


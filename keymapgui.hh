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


wostream& operator<<(wostream& os,const irr::EKEY_CODE& k){
  switch(k){
    case irr::KEY_BACK: return os<<L"BACKSPACE key";
    case irr::KEY_TAB: return os<<L"TAB key";
    case irr::KEY_CLEAR: return os<<L"CLEAR key";
    case irr::KEY_RETURN: return os<<L"ENTER key";
    case irr::KEY_SHIFT: return os<<L"SHIFT key";
    case irr::KEY_CONTROL: return os<<L"CTRL key";
    case irr::KEY_MENU: return os<<L"ALT key";
    case irr::KEY_PAUSE: return os<<L"PAUSE key";
    case irr::KEY_CAPITAL: return os<<L"CAPS LOCK key";
    case irr::KEY_ESCAPE: return os<<L"ESC key";
    case irr::KEY_CONVERT: return os<<L"IME convert";
    case irr::KEY_NONCONVERT: return os<<L"IME nonconvert";
    case irr::KEY_ACCEPT: return os<<L"IME accept";
    case irr::KEY_MODECHANGE: return os<<L"IME mode change request";
    case irr::KEY_SPACE: return os<<L"SPACEBAR";
    case irr::KEY_PRIOR: return os<<L"PAGE UP key";
    case irr::KEY_NEXT: return os<<L"PAGE DOWN key";
    case irr::KEY_END: return os<<L"END key";
    case irr::KEY_HOME: return os<<L"HOME key";
    case irr::KEY_LEFT: return os<<L"LEFT ARROW key";
    case irr::KEY_UP: return os<<L"UP ARROW key";
    case irr::KEY_RIGHT: return os<<L"RIGHT ARROW key";
    case irr::KEY_DOWN: return os<<L"DOWN ARROW key";
    case irr::KEY_SELECT: return os<<L"SELECT key";
    case irr::KEY_PRINT: return os<<L"PRINT key";
    case irr::KEY_EXECUT: return os<<L"EXECUTE key";
    case irr::KEY_SNAPSHOT: return os<<L"PRINT SCREEN key";
    case irr::KEY_INSERT: return os<<L"INS key";
    case irr::KEY_DELETE: return os<<L"DEL key";
    case irr::KEY_HELP: return os<<L"HELP key";
    case irr::KEY_KEY_0: return os<<L"0 key";
    case irr::KEY_KEY_1: return os<<L"1 key";
    case irr::KEY_KEY_2: return os<<L"2 key";
    case irr::KEY_KEY_3: return os<<L"3 key";
    case irr::KEY_KEY_4: return os<<L"4 key";
    case irr::KEY_KEY_5: return os<<L"5 key";
    case irr::KEY_KEY_6: return os<<L"6 key";
    case irr::KEY_KEY_7: return os<<L"7 key";
    case irr::KEY_KEY_8: return os<<L"8 key";
    case irr::KEY_KEY_9: return os<<L"9 key";
    case irr::KEY_KEY_A: return os<<L"A key";
    case irr::KEY_KEY_B: return os<<L"B key";
    case irr::KEY_KEY_C: return os<<L"C key";
    case irr::KEY_KEY_D: return os<<L"D key";
    case irr::KEY_KEY_E: return os<<L"E key";
    case irr::KEY_KEY_F: return os<<L"F key";
    case irr::KEY_KEY_G: return os<<L"G key";
    case irr::KEY_KEY_H: return os<<L"H key";
    case irr::KEY_KEY_I: return os<<L"I key";
    case irr::KEY_KEY_J: return os<<L"J key";
    case irr::KEY_KEY_K: return os<<L"K key";
    case irr::KEY_KEY_L: return os<<L"L key";
    case irr::KEY_KEY_M: return os<<L"M key";
    case irr::KEY_KEY_N: return os<<L"N key";
    case irr::KEY_KEY_O: return os<<L"O key";
    case irr::KEY_KEY_P: return os<<L"P key";
    case irr::KEY_KEY_Q: return os<<L"Q key";
    case irr::KEY_KEY_R: return os<<L"R key";
    case irr::KEY_KEY_S: return os<<L"S key";
    case irr::KEY_KEY_T: return os<<L"T key";
    case irr::KEY_KEY_U: return os<<L"U key";
    case irr::KEY_KEY_V: return os<<L"V key";
    case irr::KEY_KEY_W: return os<<L"W key";
    case irr::KEY_KEY_X: return os<<L"X key";
    case irr::KEY_KEY_Y: return os<<L"Y key";
    case irr::KEY_KEY_Z: return os<<L"Z key";
    case irr::KEY_LWIN: return os<<L"Left Windows key";
    case irr::KEY_RWIN: return os<<L"Right Windows key";
    case irr::KEY_APPS: return os<<L"Applications key";
    case irr::KEY_SLEEP: return os<<L"Computer Sleep key";
    case irr::KEY_NUMPAD0: return os<<L"Numeric keypad 0 key";
    case irr::KEY_NUMPAD1: return os<<L"Numeric keypad 1 key";
    case irr::KEY_NUMPAD2: return os<<L"Numeric keypad 2 key";
    case irr::KEY_NUMPAD3: return os<<L"Numeric keypad 3 key";
    case irr::KEY_NUMPAD4: return os<<L"Numeric keypad 4 key";
    case irr::KEY_NUMPAD5: return os<<L"Numeric keypad 5 key";
    case irr::KEY_NUMPAD6: return os<<L"Numeric keypad 6 key";
    case irr::KEY_NUMPAD7: return os<<L"Numeric keypad 7 key";
    case irr::KEY_NUMPAD8: return os<<L"Numeric keypad 8 key";
    case irr::KEY_NUMPAD9: return os<<L"Numeric keypad 9 key";
    case irr::KEY_MULTIPLY: return os<<L"Multiply key";
    case irr::KEY_ADD: return os<<L"Add key";
    case irr::KEY_SEPARATOR: return os<<L"Separator key";
    case irr::KEY_SUBTRACT: return os<<L"Subtract key";
    case irr::KEY_DECIMAL: return os<<L"Decimal key";
    case irr::KEY_DIVIDE: return os<<L"Divide key";
    case irr::KEY_F1: return os<<L"F1 key";
    case irr::KEY_F2: return os<<L"F2 key";
    case irr::KEY_F3: return os<<L"F3 key";
    case irr::KEY_F4: return os<<L"F4 key";
    case irr::KEY_F5: return os<<L"F5 key";
    case irr::KEY_F6: return os<<L"F6 key";
    case irr::KEY_F7: return os<<L"F7 key";
    case irr::KEY_F8: return os<<L"F8 key";
    case irr::KEY_F9: return os<<L"F9 key";
    case irr::KEY_F10: return os<<L"F10 key";
    case irr::KEY_F11: return os<<L"F11 key";
    case irr::KEY_F12: return os<<L"F12 key";
    case irr::KEY_F13: return os<<L"F13 key";
    case irr::KEY_F14: return os<<L"F14 key";
    case irr::KEY_F15: return os<<L"F15 key";
    case irr::KEY_F16: return os<<L"F16 key";
    case irr::KEY_F17: return os<<L"F17 key";
    case irr::KEY_F18: return os<<L"F18 key";
    case irr::KEY_F19: return os<<L"F19 key";
    case irr::KEY_F20: return os<<L"F20 key";
    case irr::KEY_F21: return os<<L"F21 key";
    case irr::KEY_F22: return os<<L"F22 key";
    case irr::KEY_F23: return os<<L"F23 key";
    case irr::KEY_F24: return os<<L"F24 key";
    case irr::KEY_NUMLOCK: return os<<L"NUM LOCK key";
    case irr::KEY_SCROLL: return os<<L"SCROLL LOCK key";
    case irr::KEY_LSHIFT: return os<<L"Left SHIFT key";
    case irr::KEY_RSHIFT: return os<<L"Right SHIFT key";
    case irr::KEY_LCONTROL: return os<<L"Left CONTROL key";
    case irr::KEY_RCONTROL: return os<<L"Right CONTROL key";
    case irr::KEY_LMENU: return os<<L"Left MENU key";
    case irr::KEY_RMENU: return os<<L"Right MENU key";
    case irr::KEY_PLUS: return os<<L"Plus Key (+)";
    case irr::KEY_COMMA: return os<<L"Comma Key (,)";
    case irr::KEY_MINUS: return os<<L"Minus Key (-)";
    case irr::KEY_PERIOD: return os<<L"Period Key (.)";
    case irr::KEY_ATTN: return os<<L"Attn key";
    case irr::KEY_CRSEL: return os<<L"CrSel key";
    case irr::KEY_EXSEL: return os<<L"ExSel key";
    case irr::KEY_EREOF: return os<<L"Erase EOF key";
    case irr::KEY_PLAY: return os<<L"Play key";
    case irr::KEY_ZOOM: return os<<L"Zoom key";
    case irr::KEY_PA1: return os<<L"PA1 key";
    case irr::KEY_OEM_CLEAR: return os<<L"Clear key";
    default:
      return os<<L"{key "<<((int)k)<<L"}";
  }
}


wstring toString(KeySpec ks){
  wstringstream ss;
  if(ks.chr==0)
    if(ks.key==irr::KEY_KEY_CODES_COUNT || ks.key==0)
      return L"None";
    else{
      if(ks.control)
        ss<<L"Ctrl+";
      if(ks.shift)
        ss<<L"Shift+";
      ss<<ks.key;
      return ss.str();
    }
  else{
    if(ks.control)
      ss<<L"Ctrl+";
    if(ks.shift)
      ss<<L"Shift+";
    ss<<ks.chr;
    return ss.str();
  }
}

      

class KeyMapGui: BaseGui{

  KeyMap km;
  KeyMap* kmp;

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
            table->setCellText(editing,1,toString(km.getKeySpec(KeyMap::actionNames[editing].first)).c_str());
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
          KeyMap::Action old=km.addMapping(ks,KeyMap::actionNames[editing].first).second;
          for(int i=0;i<KeyMap::A_COUNT-1;++i){
            if(old==KeyMap::actionNames[i].first)
              table->setCellText(i,1,toString(km.getKeySpec(old)).c_str());
          }
          table->setCellText(editing,1,toString(km.getKeySpec(KeyMap::actionNames[editing].first)).c_str());
         
          editing=-1;
          return true;
        }
      }
      return false;
    };


  public:
    bool edit(irr::IrrlichtDevice* _device,KeyMap& _km){
      km=_km;
      kmp=&_km;
      main(_device);
      return okClicked;
    }
    
    void createGUI(){
      editing=-1;
      okClicked=cancelClicked=false;
      
      irr::IVideoDriver* driver = device->getVideoDriver();
      irr::IGUIEnvironment *guienv = device->getGUIEnvironment();
      
      irr::rect<irr::s32> rect=driver->getViewPort();
      irr::position2d<irr::s32> center=rect.getCenter();
      irr::dimension2d<irr::s32> size=rect.getSize();
      size.Width=min(600,size.Width-10);
      size.Height=min(600,size.Height-10);
      
      table = guienv->addTable(irr::rect<irr::s32>(center.X-size.Width/2,center.Y-size.Height/2,center.X+size.Width/2,center.Y+size.Height/2-10-32),el,-1,true);
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
      
      guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-210,center.Y+size.Height/2-32,center.X+size.Width/2-100,center.Y+size.Height/2),el,GUI_ID_CANCEL_BUTTON,L"Cancel");
      guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-100,center.Y+size.Height/2-32,center.X+size.Width/2,center.Y+size.Height/2),el,GUI_ID_OK_BUTTON,L"Apply");
      
      guienv->setFocus(table);
        
      device->setWindowCaption(L"Configure Hyper Maze");
      
    }
    bool run(){
      if(cancelClicked)
        return false;
      if(okClicked){
        *kmp=km;
        wofstream ofs("hypermaze.keymap.conf");
        ofs<<km;
        ofs.close();
        return false;
      }
      return true;
    }
};


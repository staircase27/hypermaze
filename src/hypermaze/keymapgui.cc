#include "keymapgui.hh"
#include "keymap.hh"

irr::core::stringw toString(const irr::EKEY_CODE& k){
  switch(k){
    case irr::KEY_BACK: return L"BACKSPACE key";
    case irr::KEY_TAB: return L"TAB key";
    case irr::KEY_CLEAR: return L"CLEAR key";
    case irr::KEY_RETURN: return L"ENTER key";
    case irr::KEY_SHIFT: return L"SHIFT key";
    case irr::KEY_CONTROL: return L"CTRL key";
    case irr::KEY_MENU: return L"ALT key";
    case irr::KEY_PAUSE: return L"PAUSE key";
    case irr::KEY_CAPITAL: return L"CAPS LOCK key";
    case irr::KEY_ESCAPE: return L"ESC key";
    case irr::KEY_CONVERT: return L"IME convert";
    case irr::KEY_NONCONVERT: return L"IME nonconvert";
    case irr::KEY_ACCEPT: return L"IME accept";
    case irr::KEY_MODECHANGE: return L"IME mode change request";
    case irr::KEY_SPACE: return L"SPACEBAR";
    case irr::KEY_PRIOR: return L"PAGE UP key";
    case irr::KEY_NEXT: return L"PAGE DOWN key";
    case irr::KEY_END: return L"END key";
    case irr::KEY_HOME: return L"HOME key";
    case irr::KEY_LEFT: return L"LEFT ARROW key";
    case irr::KEY_UP: return L"UP ARROW key";
    case irr::KEY_RIGHT: return L"RIGHT ARROW key";
    case irr::KEY_DOWN: return L"DOWN ARROW key";
    case irr::KEY_SELECT: return L"SELECT key";
    case irr::KEY_PRINT: return L"PRINT key";
    case irr::KEY_EXECUT: return L"EXECUTE key";
    case irr::KEY_SNAPSHOT: return L"PRINT SCREEN key";
    case irr::KEY_INSERT: return L"INS key";
    case irr::KEY_DELETE: return L"DEL key";
    case irr::KEY_HELP: return L"HELP key";
    case irr::KEY_KEY_0: return L"0 key";
    case irr::KEY_KEY_1: return L"1 key";
    case irr::KEY_KEY_2: return L"2 key";
    case irr::KEY_KEY_3: return L"3 key";
    case irr::KEY_KEY_4: return L"4 key";
    case irr::KEY_KEY_5: return L"5 key";
    case irr::KEY_KEY_6: return L"6 key";
    case irr::KEY_KEY_7: return L"7 key";
    case irr::KEY_KEY_8: return L"8 key";
    case irr::KEY_KEY_9: return L"9 key";
    case irr::KEY_KEY_A: return L"A key";
    case irr::KEY_KEY_B: return L"B key";
    case irr::KEY_KEY_C: return L"C key";
    case irr::KEY_KEY_D: return L"D key";
    case irr::KEY_KEY_E: return L"E key";
    case irr::KEY_KEY_F: return L"F key";
    case irr::KEY_KEY_G: return L"G key";
    case irr::KEY_KEY_H: return L"H key";
    case irr::KEY_KEY_I: return L"I key";
    case irr::KEY_KEY_J: return L"J key";
    case irr::KEY_KEY_K: return L"K key";
    case irr::KEY_KEY_L: return L"L key";
    case irr::KEY_KEY_M: return L"M key";
    case irr::KEY_KEY_N: return L"N key";
    case irr::KEY_KEY_O: return L"O key";
    case irr::KEY_KEY_P: return L"P key";
    case irr::KEY_KEY_Q: return L"Q key";
    case irr::KEY_KEY_R: return L"R key";
    case irr::KEY_KEY_S: return L"S key";
    case irr::KEY_KEY_T: return L"T key";
    case irr::KEY_KEY_U: return L"U key";
    case irr::KEY_KEY_V: return L"V key";
    case irr::KEY_KEY_W: return L"W key";
    case irr::KEY_KEY_X: return L"X key";
    case irr::KEY_KEY_Y: return L"Y key";
    case irr::KEY_KEY_Z: return L"Z key";
    case irr::KEY_LWIN: return L"Left Windows key";
    case irr::KEY_RWIN: return L"Right Windows key";
    case irr::KEY_APPS: return L"Applications key";
    case irr::KEY_SLEEP: return L"Computer Sleep key";
    case irr::KEY_NUMPAD0: return L"Numeric keypad 0 key";
    case irr::KEY_NUMPAD1: return L"Numeric keypad 1 key";
    case irr::KEY_NUMPAD2: return L"Numeric keypad 2 key";
    case irr::KEY_NUMPAD3: return L"Numeric keypad 3 key";
    case irr::KEY_NUMPAD4: return L"Numeric keypad 4 key";
    case irr::KEY_NUMPAD5: return L"Numeric keypad 5 key";
    case irr::KEY_NUMPAD6: return L"Numeric keypad 6 key";
    case irr::KEY_NUMPAD7: return L"Numeric keypad 7 key";
    case irr::KEY_NUMPAD8: return L"Numeric keypad 8 key";
    case irr::KEY_NUMPAD9: return L"Numeric keypad 9 key";
    case irr::KEY_MULTIPLY: return L"Multiply key";
    case irr::KEY_ADD: return L"Add key";
    case irr::KEY_SEPARATOR: return L"Separator key";
    case irr::KEY_SUBTRACT: return L"Subtract key";
    case irr::KEY_DECIMAL: return L"Decimal key";
    case irr::KEY_DIVIDE: return L"Divide key";
    case irr::KEY_F1: return L"F1 key";
    case irr::KEY_F2: return L"F2 key";
    case irr::KEY_F3: return L"F3 key";
    case irr::KEY_F4: return L"F4 key";
    case irr::KEY_F5: return L"F5 key";
    case irr::KEY_F6: return L"F6 key";
    case irr::KEY_F7: return L"F7 key";
    case irr::KEY_F8: return L"F8 key";
    case irr::KEY_F9: return L"F9 key";
    case irr::KEY_F10: return L"F10 key";
    case irr::KEY_F11: return L"F11 key";
    case irr::KEY_F12: return L"F12 key";
    case irr::KEY_F13: return L"F13 key";
    case irr::KEY_F14: return L"F14 key";
    case irr::KEY_F15: return L"F15 key";
    case irr::KEY_F16: return L"F16 key";
    case irr::KEY_F17: return L"F17 key";
    case irr::KEY_F18: return L"F18 key";
    case irr::KEY_F19: return L"F19 key";
    case irr::KEY_F20: return L"F20 key";
    case irr::KEY_F21: return L"F21 key";
    case irr::KEY_F22: return L"F22 key";
    case irr::KEY_F23: return L"F23 key";
    case irr::KEY_F24: return L"F24 key";
    case irr::KEY_NUMLOCK: return L"NUM LOCK key";
    case irr::KEY_SCROLL: return L"SCROLL LOCK key";
    case irr::KEY_LSHIFT: return L"Left SHIFT key";
    case irr::KEY_RSHIFT: return L"Right SHIFT key";
    case irr::KEY_LCONTROL: return L"Left CONTROL key";
    case irr::KEY_RCONTROL: return L"Right CONTROL key";
    case irr::KEY_LMENU: return L"Left MENU key";
    case irr::KEY_RMENU: return L"Right MENU key";
    case irr::KEY_PLUS: return L"Plus Key (+)";
    case irr::KEY_COMMA: return L"Comma Key (,)";
    case irr::KEY_MINUS: return L"Minus Key (-)";
    case irr::KEY_PERIOD: return L"Period Key (.)";
    case irr::KEY_ATTN: return L"Attn key";
    case irr::KEY_CRSEL: return L"CrSel key";
    case irr::KEY_EXSEL: return L"ExSel key";
    case irr::KEY_EREOF: return L"Erase EOF key";
    case irr::KEY_PLAY: return L"Play key";
    case irr::KEY_ZOOM: return L"Zoom key";
    case irr::KEY_PA1: return L"PA1 key";
    case irr::KEY_OEM_CLEAR: return L"Clear key";
    default:
      {
        irr::core::stringw tmp(L"{key ");
        tmp+=((int)k);
        return tmp+=L"}";
      }
  }
}


inline irr::core::stringw toString(KeySpec ks){
  irr::core::stringw s;
  if(ks.chr==0 && (ks.key==irr::KEY_KEY_CODES_COUNT || ks.key==0))
    return L"None";
  if(ks.control)
    s+=L"Ctrl+";
  if(ks.shift)
    s+=L"Shift+";
  if(ks.chr==0)
    s+=toString(ks.key);
  else
    s+=ks.chr;
  return s;
}

bool KeyMapGui::OnEventImpl(const irr::SEvent &event){
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
    if(event.GUIEvent.EventType==irr::gui::EGET_TABLE_SELECTED_AGAIN){
      if(editing>=0)
        table->setCellText(editing,1,toString(kmtmp->getKeySpec(KeyMap::actionNames[editing].first)));
      editing=table->getSelected();
      table->setCellText(editing,1,L"Press New Key");
      return true;
    }
  }
  if(event.GUIEvent.EventType == irr::gui::EGET_EDITBOX_ENTER){
    okClicked=true;
    return true;
  }
  if(event.EventType == irr::EET_KEY_INPUT_EVENT&&event.KeyInput.PressedDown){
    if(event.KeyInput.Key==irr::KEY_ESCAPE){
      cancelClicked=true;
      return true;
    }else if(editing>=0){
      KeySpec ks(event.KeyInput.Char,event.KeyInput.Shift,event.KeyInput.Control);
      if(event.KeyInput.Char==0)
        ks=KeySpec(event.KeyInput.Key,event.KeyInput.Shift,event.KeyInput.Control);
      KeyMap::Action old=kmtmp->addMapping(ks,KeyMap::actionNames[editing].first).second;
      for(int i=0;i<KeyMap::A_COUNT-1;++i){
        if(old==KeyMap::actionNames[i].first)
          table->setCellText(i,1,toString(kmtmp->getKeySpec(old)).c_str());
      }
      table->setCellText(editing,1,toString(kmtmp->getKeySpec(KeyMap::actionNames[editing].first)).c_str());

      editing=-1;
      return true;
    }else if(event.KeyInput.Key==irr::KEY_RETURN){
      okClicked=true;
      return true;
    }
  }
  return false;
};

bool KeyMapGui::edit(irr::IrrlichtDevice* _device,FontManager* _fm,KeyMap& _km){
  if(kmtmp==0)
    kmtmp=new KeyMap();
  *kmtmp=_km;
  kmp=&_km;
  main(_device,_fm);
  return okClicked;
}

void KeyMapGui::createGUI(){
  editing=-1;
  okClicked=cancelClicked=false;

  irr::video::IVideoDriver* driver = getDevice()->getVideoDriver();
  irr::gui::IGUIEnvironment *guienv = getDevice()->getGUIEnvironment();

  irr::core::rect<irr::s32> rect=driver->getViewPort();
  irr::core::position2d<irr::s32> center=rect.getCenter();
  irr::core::dimension2d<irr::s32> size=rect.getSize();
  size.Width=min(600,size.Width-10);
  size.Height=min(600,size.Height-10);

  table = guienv->addTable(irr::core::rect<irr::s32>(center.X-size.Width/2,center.Y-size.Height/2,center.X+size.Width/2,center.Y+size.Height/2-10-32),getTopElement(),-1,true);
  table->addColumn(L"Action",0);
  table->addColumn(L"Key",1);
  table->setColumnWidth(0,size.Width/2-10);
  table->setColumnWidth(1,size.Width/2-10);
  table->setColumnOrdering(0,irr::gui::EGCO_NONE);
  table->setColumnOrdering(1,irr::gui::EGCO_NONE);
  for(int i=0;i<KeyMap::A_COUNT-1;++i){
    table->addRow(i);
    table->setCellText(i,0,KeyMap::actionNames[i].second);
    table->setCellText(i,1,toString(kmtmp->getKeySpec(KeyMap::actionNames[i].first)).c_str());
  }

  guienv->addButton(irr::core::rect<irr::s32>(center.X+size.Width/2-210,center.Y+size.Height/2-32,center.X+size.Width/2-100,center.Y+size.Height/2),getTopElement(),GUI_ID_CANCEL_BUTTON,L"Cancel");
  guienv->addButton(irr::core::rect<irr::s32>(center.X+size.Width/2-100,center.Y+size.Height/2-32,center.X+size.Width/2,center.Y+size.Height/2),getTopElement(),GUI_ID_OK_BUTTON,L"Apply");

  guienv->setFocus(table);

  getDevice()->setWindowCaption(L"Hyper Maze: Configure");

}
bool KeyMapGui::run(){
  if(cancelClicked)
    return false;
  if(okClicked){
    *kmp=*kmtmp;
    return !kmtmp->save(getDevice()->getFileSystem());
  }
  return true;
}
KeyMapGui::~KeyMapGui(){
  delete kmtmp;
}

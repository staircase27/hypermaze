#include "irrlicht.h"
#include <iostream>
#include <sstream>

#ifndef KEYMAP_HH_INC
#define KEYMAP_HH_INC

namespace irr{
  using namespace core;
  using namespace scene;
  using namespace io;
  using namespace video;
};

using namespace std;

struct KeySpec{
  wchar_t chr;
  irr::EKEY_CODE key;

  bool shift;
  bool control;
  KeySpec():chr(0),key(irr::KEY_KEY_CODES_COUNT),shift(false),control(false){};
  KeySpec(wchar_t chr,bool shift=false,bool control=false):chr(chr),shift(shift),control(control),key(irr::KEY_KEY_CODES_COUNT){};
  KeySpec(irr::EKEY_CODE key,bool shift=false,bool control=false):key(key),shift(shift),control(control),chr(0){};
  
  bool operator<(const KeySpec& o) const{
    if(chr<o.chr)
      return true;
    if(key<o.key)
      return true;
    if(!(shift)&&o.shift)
      return true;
    if((!control)&&o.control)
      return true;
    return false;
  }
  bool operator ==(const KeySpec& o) const{
    return chr==o.chr&&key==o.key&&shift==o.shift&&control==o.control;
  }
};




class KeyMap{
  
  public:
    enum Action{
      A_NONE=0,
      A_MOVE_UP,A_MOVE_DOWN,
      A_MOVE_LEFT,A_MOVE_RIGHT,
      A_MOVE_FORWARD,A_MOVE_BACK,
      A_SLIDE_START_OUT,A_SLIDE_START_IN,
      A_SLIDE_END_IN,A_SLIDE_END_OUT,
      A_SLICE_UP_IN,A_SLICE_UP_OUT,
      A_SLICE_DOWN_IN,A_SLICE_DOWN_OUT,
      A_SLICE_LEFT_IN,A_SLICE_LEFT_OUT,
      A_SLICE_RIGHT_IN,A_SLICE_RIGHT_OUT,
      A_SLICE_FORWARD_IN,A_SLICE_FORWARD_OUT,
      A_SLICE_BACK_IN,A_SLICE_BACK_OUT,
      A_GENERATE,A_LOAD,
      A_SAVE,A_CONF,
      A_COUNT};
      
    static const pair<Action,pair<Dirn,bool> > sliceActions[12];
    static const pair<Action,pair<bool,bool> > slideActions[4];
    static const pair<Action,Dirn> moveActions[6];
    static const pair<Action,wstring> actionNames[A_COUNT-1];
  private:
    map<KeySpec,Action> keyMap;
    map<Action,KeySpec> revMap;
  public:
    pair<KeySpec,Action> addMapping(KeySpec key,Action a){
      pair<KeySpec,Action> old(revMap[a],keyMap[key]);
      keyMap.erase(old.first);
      revMap.erase(old.second);
      keyMap[key]=a;
      revMap[a]=key;
      return old;
    }
    Action getAction(KeySpec key){
      return keyMap[key];
    }
    KeySpec getKeySpec(Action a){
      return revMap[a];
    }
    Action removeMapping(KeySpec key){
      Action old=keyMap[key];
      keyMap.erase(key);
      revMap.erase(old);
      return old;
    }
    KeySpec removeMapping(Action a){
      KeySpec old(revMap[a]);
      revMap.erase(a);
      keyMap.erase(old);
      return old;
    }
    const map<KeySpec,Action> getKeyMap(){
      return keyMap;
    }
    const map<Action,KeySpec> getRevMap(){
      return revMap;
    }
    
    friend wostream& operator<<(wostream& os,const KeyMap& km);
    friend wistream& operator>>(wistream& os,const KeyMap& km);
};

const pair<KeyMap::Action,pair<Dirn,bool> > KeyMap::sliceActions[12]={
      pair<KeyMap::Action,pair<Dirn,bool> >(KeyMap::A_SLICE_UP_OUT,pair<Dirn,bool>(UP,true)),
      pair<KeyMap::Action,pair<Dirn,bool> >(KeyMap::A_SLICE_UP_IN,pair<Dirn,bool>(UP,false)),
      pair<KeyMap::Action,pair<Dirn,bool> >(KeyMap::A_SLICE_DOWN_OUT,pair<Dirn,bool>(DOWN,true)),
      pair<KeyMap::Action,pair<Dirn,bool> >(KeyMap::A_SLICE_DOWN_IN,pair<Dirn,bool>(DOWN,false)),
      pair<KeyMap::Action,pair<Dirn,bool> >(KeyMap::A_SLICE_LEFT_OUT,pair<Dirn,bool>(LEFT,true)),
      pair<KeyMap::Action,pair<Dirn,bool> >(KeyMap::A_SLICE_LEFT_IN,pair<Dirn,bool>(LEFT,false)),
      pair<KeyMap::Action,pair<Dirn,bool> >(KeyMap::A_SLICE_RIGHT_OUT,pair<Dirn,bool>(RIGHT,true)),
      pair<KeyMap::Action,pair<Dirn,bool> >(KeyMap::A_SLICE_RIGHT_IN,pair<Dirn,bool>(RIGHT,false)),
      pair<KeyMap::Action,pair<Dirn,bool> >(KeyMap::A_SLICE_FORWARD_OUT,pair<Dirn,bool>(FORWARD,true)),
      pair<KeyMap::Action,pair<Dirn,bool> >(KeyMap::A_SLICE_FORWARD_IN,pair<Dirn,bool>(FORWARD,false)),
      pair<KeyMap::Action,pair<Dirn,bool> >(KeyMap::A_SLICE_BACK_OUT,pair<Dirn,bool>(BACK,true)),
      pair<KeyMap::Action,pair<Dirn,bool> >(KeyMap::A_SLICE_BACK_IN,pair<Dirn,bool>(BACK,false))};

const pair<KeyMap::Action,pair<bool,bool> > KeyMap::slideActions[4]={
      pair<KeyMap::Action,pair<bool,bool> >(KeyMap::A_SLIDE_END_OUT,pair<bool,bool>(true,true)),
      pair<KeyMap::Action,pair<bool,bool> >(KeyMap::A_SLIDE_END_IN,pair<bool,bool>(true,false)),
      pair<KeyMap::Action,pair<bool,bool> >(KeyMap::A_SLIDE_START_IN,pair<bool,bool>(false,false)),
      pair<KeyMap::Action,pair<bool,bool> >(KeyMap::A_SLIDE_START_OUT,pair<bool,bool>(false,true))};

const pair<KeyMap::Action,Dirn> KeyMap::moveActions[6]={
      pair<KeyMap::Action,Dirn>(KeyMap::A_MOVE_UP,UP),
      pair<KeyMap::Action,Dirn>(KeyMap::A_MOVE_DOWN,DOWN),
      pair<KeyMap::Action,Dirn>(KeyMap::A_MOVE_LEFT,LEFT),
      pair<KeyMap::Action,Dirn>(KeyMap::A_MOVE_RIGHT,RIGHT),
      pair<KeyMap::Action,Dirn>(KeyMap::A_MOVE_FORWARD,FORWARD),
      pair<KeyMap::Action,Dirn>(KeyMap::A_MOVE_BACK,BACK)};

const pair<KeyMap::Action,wstring> KeyMap::actionNames[A_COUNT-1]={
      pair<KeyMap::Action,wstring>(KeyMap::A_GENERATE,L"A_GENERATE"),
      pair<KeyMap::Action,wstring>(KeyMap::A_LOAD,L"A_LOAD"),
      pair<KeyMap::Action,wstring>(KeyMap::A_SAVE,L"A_SAVE"),
      pair<KeyMap::Action,wstring>(KeyMap::A_CONF,L"A_CONF"),
      pair<KeyMap::Action,wstring>(KeyMap::A_MOVE_UP,L"A_MOVE_UP"),
      pair<KeyMap::Action,wstring>(KeyMap::A_MOVE_DOWN,L"A_MOVE_DOWN"),
      pair<KeyMap::Action,wstring>(KeyMap::A_MOVE_LEFT,L"A_MOVE_LEFT"),
      pair<KeyMap::Action,wstring>(KeyMap::A_MOVE_RIGHT,L"A_MOVE_RIGHT"),
      pair<KeyMap::Action,wstring>(KeyMap::A_MOVE_FORWARD,L"A_MOVE_FORWARD"),
      pair<KeyMap::Action,wstring>(KeyMap::A_MOVE_BACK,L"A_MOVE_BACK"),
      pair<KeyMap::Action,wstring>(KeyMap::A_SLIDE_START_OUT,L"A_SLIDE_START_OUT"),
      pair<KeyMap::Action,wstring>(KeyMap::A_SLIDE_START_IN,L"A_SLIDE_START_IN"),
      pair<KeyMap::Action,wstring>(KeyMap::A_SLIDE_END_IN,L"A_SLIDE_END_IN"),
      pair<KeyMap::Action,wstring>(KeyMap::A_SLIDE_END_OUT,L"A_SLIDE_END_OUT"),
      pair<KeyMap::Action,wstring>(KeyMap::A_SLICE_UP_IN,L"A_SLICE_UP_IN"),
      pair<KeyMap::Action,wstring>(KeyMap::A_SLICE_UP_OUT,L"A_SLICE_UP_OUT"),
      pair<KeyMap::Action,wstring>(KeyMap::A_SLICE_DOWN_IN,L"A_SLICE_DOWN_IN"),
      pair<KeyMap::Action,wstring>(KeyMap::A_SLICE_DOWN_OUT,L"A_SLICE_DOWN_OUT"),
      pair<KeyMap::Action,wstring>(KeyMap::A_SLICE_LEFT_IN,L"A_SLICE_LEFT_IN"),
      pair<KeyMap::Action,wstring>(KeyMap::A_SLICE_LEFT_OUT,L"A_SLICE_LEFT_OUT"),
      pair<KeyMap::Action,wstring>(KeyMap::A_SLICE_RIGHT_IN,L"A_SLICE_RIGHT_IN"),
      pair<KeyMap::Action,wstring>(KeyMap::A_SLICE_RIGHT_OUT,L"A_SLICE_RIGHT_OUT"),
      pair<KeyMap::Action,wstring>(KeyMap::A_SLICE_FORWARD_IN,L"A_SLICE_FORWARD_IN"),
      pair<KeyMap::Action,wstring>(KeyMap::A_SLICE_FORWARD_OUT,L"A_SLICE_FORWARD_OUT"),
      pair<KeyMap::Action,wstring>(KeyMap::A_SLICE_BACK_IN,L"A_SLICE_BACK_IN"),
      pair<KeyMap::Action,wstring>(KeyMap::A_SLICE_BACK_OUT,L"A_SLICE_BACK_OUT")};
      
      
    
wostream& operator<<(wostream& os,const KeyMap& km){
  for(map<KeySpec,KeyMap::Action>::const_iterator it=km.keyMap.begin();it!=km.keyMap.end();++it){
    os<<it->first.chr<<" "<<it->first.key<<" "<<it->first.shift<<" "<<it->first.shift<<" "<<it->second<<endl;
  }
  return os;
}
wistream& operator>>(wistream& is,KeyMap& km){
  wstring line;
  getline(is,line);
  while(!line.empty()){
    wstringstream ss(line);
    KeySpec ks;
    KeyMap::Action a;
    int i;
    ss>>ks.chr>>i>>ks.shift>>ks.shift;
    ks.key=(irr::EKEY_CODE)i;
    ss>>i;
    a=(KeyMap::Action)i;
    km.addMapping(ks,a);
    getline(is,line);
  }
  return is;
}

#endif

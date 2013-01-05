#include "irrlicht.h"
#include "dirns.hh"
#include <iostream>
#include <sstream>
#include <map>

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
  
  inline bool operator<(const KeySpec& o) const{
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
  inline bool operator ==(const KeySpec& o) const{
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

inline wostream& operator<<(wostream& os,const KeyMap& km){
  for(map<KeySpec,KeyMap::Action>::const_iterator it=km.keyMap.begin();it!=km.keyMap.end();++it){
    os<<it->first.chr<<" "<<it->first.key<<" "<<it->first.shift<<" "<<it->first.shift<<" "<<it->second<<endl;
  }
  return os;
}
inline wistream& operator>>(wistream& is,KeyMap& km){
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

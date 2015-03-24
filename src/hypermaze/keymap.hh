#include "irrlicht.h"
#include "../irrshared/platformcompat.hh"
#include "../core/dirns.hh"
#include <iostream>
#include <map>

#ifndef KEYMAP_HH_INC
#define KEYMAP_HH_INC

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
      A_UNDO,
      A_GENERATE,A_LOAD,
      A_SAVE,A_CONF,
      A_COUNT};

    static const std::pair<Action,std::pair<Dirn,bool> > sliceActions[12];
    static const std::pair<Action,std::pair<bool,bool> > slideActions[4];
    static const std::pair<Action,Dirn> moveActions[6];
    static const std::pair<Action,const wchar_t*> actionNames[A_COUNT-1];
  private:
    std::map<KeySpec,Action> keyMap;
    std::map<Action,KeySpec> revMap;
  public:
    inline std::pair<KeySpec,Action> addMapping(KeySpec key,Action a){
      std::pair<KeySpec,Action> old(revMap[a],keyMap[key]);
      keyMap.erase(old.first);
      revMap.erase(old.second);
      keyMap[key]=a;
      revMap[a]=key;
      return old;
    }
    inline Action getAction(KeySpec key){
      return keyMap[key];
    }
    inline KeySpec getKeySpec(Action a){
      return revMap[a];
    }
    inline Action removeMapping(KeySpec key){
      Action old=keyMap[key];
      keyMap.erase(key);
      revMap.erase(old);
      return old;
    }
    inline KeySpec removeMapping(Action a){
      KeySpec old(revMap[a]);
      revMap.erase(a);
      keyMap.erase(old);
      return old;
    }
    inline const std::map<KeySpec,Action> getKeyMap() const{
      return keyMap;
    }
    inline const std::map<Action,KeySpec> getRevMap() const{
      return revMap;
    }

  private:
    irr::u32 doparse(char* data,irr::u32 length);

  public:
    bool load(irr::io::IFileSystem* fs);
    bool save(irr::io::IFileSystem* fs);
};

#endif

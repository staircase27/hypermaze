#include "irrlicht.h"

#ifndef KEYMAP_HH_INC
#define KEYMAP_HH_INC

namespace irr{
  using namespace core;
  using namespace scene;
  using namespace io;
  using namespace video;
};

class KeyMap{
  
  public:
    enum Action{
      A_NONE=0,
      A_GENERATE,
      A_LOAD, A_SAVE,A_CONF,
      A_MOVE_UP,A_MOVE_DOWN,
      A_MOVE_LEFT,A_MOVE_RIGHT,
      A_MOVE_FORWARD,A_MOVE_BACK,
      A_SLIDE_START_START,A_SLIDE_START_END,
      A_SLIDE_END_START,A_SLIDE_END_END,
      A_SLICE_UP_IN,A_SLICE_UP_OUT,
      A_SLICE_DOWN_IN,A_SLICE_DOWN_OUT,
      A_SLICE_LEFT_IN,A_SLICE_LEFT_OUT,
      A_SLICE_RIGHT_IN,A_SLICE_RIGHT_OUT,
      A_SLICE_FORWARD_IN,A_SLICE_FORWARD_OUT,
      A_SLICE_BACK_IN,A_SLICE_BACK_OUT,
      A_COUNT};
      
    static const pair<Action,pair<Dirn,bool> > sliceActions[12];
    static const pair<Action,pair<bool,bool> > slideActions[4];
    static const pair<Action,Dirn> moveActions[6];
  private:
    map<int,Action> keyMap;
  public:
    Action addMapping(irr::EKEY_CODE code,Action a){
      Action old=keyMap[code];
      keyMap[code]=a;
      return old;
    }
    Action getAction(irr::EKEY_CODE code){
      return keyMap[code];
    }
    Action removeMapping(irr::EKEY_CODE code){
      Action old=keyMap[code];
      keyMap.erase(code);
      return old;
    }
//    const map<irr:EKEY_CODE,Action> getKeyMap(){
//      return keyMap;
//    }
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
      pair<KeyMap::Action,pair<bool,bool> >(KeyMap::A_SLIDE_END_END,pair<bool,bool>(true,true)),
      pair<KeyMap::Action,pair<bool,bool> >(KeyMap::A_SLIDE_END_START,pair<bool,bool>(true,false)),
      pair<KeyMap::Action,pair<bool,bool> >(KeyMap::A_SLIDE_START_END,pair<bool,bool>(false,true)),
      pair<KeyMap::Action,pair<bool,bool> >(KeyMap::A_SLIDE_START_START,pair<bool,bool>(false,false))};

const pair<KeyMap::Action,Dirn> KeyMap::moveActions[6]={
      pair<KeyMap::Action,Dirn>(KeyMap::A_MOVE_UP,UP),
      pair<KeyMap::Action,Dirn>(KeyMap::A_MOVE_DOWN,DOWN),
      pair<KeyMap::Action,Dirn>(KeyMap::A_MOVE_LEFT,LEFT),
      pair<KeyMap::Action,Dirn>(KeyMap::A_MOVE_RIGHT,RIGHT),
      pair<KeyMap::Action,Dirn>(KeyMap::A_MOVE_FORWARD,FORWARD),
      pair<KeyMap::Action,Dirn>(KeyMap::A_MOVE_BACK,BACK)};

#endif

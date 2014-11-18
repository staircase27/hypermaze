#include "keymap.hh"
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

const pair<KeyMap::Action,const wchar_t*> KeyMap::actionNames[A_COUNT-1]={
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_GENERATE,L"A_GENERATE"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_LOAD,L"A_LOAD"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_SAVE,L"A_SAVE"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_CONF,L"A_CONF"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_MOVE_UP,L"A_MOVE_UP"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_MOVE_DOWN,L"A_MOVE_DOWN"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_MOVE_LEFT,L"A_MOVE_LEFT"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_MOVE_RIGHT,L"A_MOVE_RIGHT"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_MOVE_FORWARD,L"A_MOVE_FORWARD"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_MOVE_BACK,L"A_MOVE_BACK"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_SLIDE_START_OUT,L"A_SLIDE_START_OUT"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_SLIDE_START_IN,L"A_SLIDE_START_IN"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_SLIDE_END_IN,L"A_SLIDE_END_IN"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_SLIDE_END_OUT,L"A_SLIDE_END_OUT"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_SLICE_UP_IN,L"A_SLICE_UP_IN"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_SLICE_UP_OUT,L"A_SLICE_UP_OUT"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_SLICE_DOWN_IN,L"A_SLICE_DOWN_IN"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_SLICE_DOWN_OUT,L"A_SLICE_DOWN_OUT"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_SLICE_LEFT_IN,L"A_SLICE_LEFT_IN"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_SLICE_LEFT_OUT,L"A_SLICE_LEFT_OUT"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_SLICE_RIGHT_IN,L"A_SLICE_RIGHT_IN"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_SLICE_RIGHT_OUT,L"A_SLICE_RIGHT_OUT"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_SLICE_FORWARD_IN,L"A_SLICE_FORWARD_IN"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_SLICE_FORWARD_OUT,L"A_SLICE_FORWARD_OUT"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_SLICE_BACK_IN,L"A_SLICE_BACK_IN"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_SLICE_BACK_OUT,L"A_SLICE_BACK_OUT"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_UNDO,L"A_UNDO")};
      

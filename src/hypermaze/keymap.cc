/**
 * @file keymap.cc
 * @brief The implementation of keymap.hh
 */
#include "keymap.hh"

namespace irr{
  using namespace core;
  using namespace io;
}
using namespace std;

/// Convert a multi byte character to a wide character
/**
 * Wrapper round mbtowc that handles null bytes and returns a size of 1
 * @param pwc pointer to a wchar_t object to place the character in
 * @param pmb a multi byte string to read a single wide character from
 * @param max the maximum amount of chars to process from pmb
 * @return the number of characters to process
 */
inline int mbtowc0( wchar_t * pwc, const char * pmb, size_t max ){
  if(*pmb==0){
    *pwc=0;
    return 1;
  }else
    return mbtowc(pwc,pmb,max);
}

irr::u32 KeyMap::doparse(char* data,irr::u32 length){
  irr::u32 totalused=0;
  char* start=data;
  char* end=data+length;
  const char* tmp;
  while(true){
    KeySpec ks;
    while(irr::isspace(*data)&&*data!=' '){++data;}
    if(data>=end) return totalused;

    data+=mbtowc0(&ks.chr,data,end-data);
    if(data>=end) return totalused;

    while(irr::isspace(*data)){++data;}
    if(data>=end) return totalused;

    ks.key=(irr::EKEY_CODE)irr::strtol10(data,&tmp);
    data+=tmp-data;
    if(data>=end) return totalused;

    while(irr::isspace(*data)){++data;}
    if(data>=end) return totalused;

    ks.shift=(bool)irr::strtol10(data,&tmp);
    data+=tmp-data;
    if(data>=end) return totalused;

    while(irr::isspace(*data)){++data;}
    if(data>=end) return totalused;

    ks.control=(bool)irr::strtol10(data,&tmp);
    data+=tmp-data;
    if(data>=end) return totalused;

    while(irr::isspace(*data)){++data;}
    if(data>=end) return totalused;

    KeyMap::Action a=(KeyMap::Action)irr::strtol10(data,&tmp);
    data+=tmp-data;
    if(data>=end) return totalused;

    this->addMapping(ks,a);
    totalused=data-start;
  }
}

bool KeyMap::load(irr::io::IFileSystem* fs){
  irr::IReadFile* f=fs->createAndOpenFile(irr::path(getUserConfigPath())+"keymap.conf");
  if(!f){
    const irr::fschar_t* dir=getSystemConfigPath();
    if(dir)
      f=fs->createAndOpenFile(irr::path(dir)+"keymap.conf");
  }
  if(!f)
    f=fs->createAndOpenFile(irr::path(getDefaultConfigPath())+"keymap.conf");
  if(!f)
    return false;
  {
    irr::u32 len=256;
    char *buf=new char[len+1];
    irr::u32 avail=0;
    irr::u32 used=0;
    while(true){
      if(used!=0){
        memmove(buf,buf+used,avail-used);
        avail-=used;
      }else if(avail==len){
        len*=2;
        char* tmp=new char[len+1];
        memcpy(tmp,buf,avail);
        delete[] buf;
        buf=tmp;
      }
      irr::s32 read=f->read(buf+avail,len-avail);
      avail+=read;
      buf[avail]='\0';
      used=this->doparse(buf,avail);
      if(read==0)
        break;
    }
    delete [] buf;
  }
  f->drop();
  return true;
}
bool KeyMap::save(irr::io::IFileSystem* fs){
  irr::io::IWriteFile* f=fs->createAndWriteFile(irr::path(getUserConfigPath())+"keymap.conf");
  if(!f)
    return false;
  {
    irr::stringc str;
    char*tmp=new char[MB_CUR_MAX];
    for(map<KeySpec,KeyMap::Action>::const_iterator it=keyMap.begin();it!=keyMap.end();++it){
      if(it->second==A_NONE)
        continue;
      int len=wctomb(tmp,it->first.chr);
      str+=irr::stringc(tmp,len);
      str+=" ";
      str+=it->first.key;
      str+=" ";
      str+=it->first.shift;
      str+=" ";
      str+=it->first.control;
      str+=" ";
      str+=it->second;
      str+="\n";
      if(str.size()>256){
        f->write(str.c_str(),str.size());
        str=irr::stringc();
      }
    }
    f->write(str.c_str(),str.size());
  }
  f->drop();
  return true;
}

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
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_GENERATE,L"Generate Maze"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_LOAD,L"Load Maze"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_SAVE,L"Save Maze"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_CONF,L"Show Help"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_MOVE_UP,L"Move String Up"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_MOVE_DOWN,L"Move String Down"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_MOVE_LEFT,L"Move String Left"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_MOVE_RIGHT,L"Move String Right"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_MOVE_FORWARD,L"Move String Forwards"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_MOVE_BACK,L"Move String Backwards"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_SLIDE_START_OUT,L"Extend Start of Selection Outwards"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_SLIDE_START_IN,L"Shrink Start of Selection Inwards"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_SLIDE_END_IN,L"Shrink End of Selection Inwards"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_SLIDE_END_OUT,L"Extend End of Selection Outwards"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_SLICE_UP_IN,L"Hide Top of Maze"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_SLICE_UP_OUT,L"Show Top of Maze"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_SLICE_DOWN_IN,L"Hide Bottom of Maze"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_SLICE_DOWN_OUT,L"Show Bottom of Maze"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_SLICE_LEFT_IN,L"Hide Left of Maze"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_SLICE_LEFT_OUT,L"Show Left of Maze"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_SLICE_RIGHT_IN,L"Hide Right of Maze"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_SLICE_RIGHT_OUT,L"Show Right of Maze"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_SLICE_FORWARD_IN,L"Hide Front of Maze"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_SLICE_FORWARD_OUT,L"Show Front of Maze"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_SLICE_BACK_IN,L"Hide Back of Maze"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_SLICE_BACK_OUT,L"Show Back of Maze"),
      pair<KeyMap::Action,const wchar_t*>(KeyMap::A_UNDO,L"Undo")};


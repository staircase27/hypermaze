#include "irrlicht.h"
#include "dirns.hh"
#include <map>

#ifndef KEYMAP_HH_INC
#define KEYMAP_HH_INC

inline int mbtowc0( wchar_t * pwc, const char * pmb, size_t max ){
  if(*pmb==0){
    *pwc=0;
    return 1;
  }else
    return mbtowc(pwc,pmb,max);
}

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
    static const pair<Action,const wchar_t*> actionNames[A_COUNT-1];
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
    
  private:
    virtual irr::u32 doparse(char* data,irr::u32 length){
      irr::u32 totalused=0;
      char* start=data;
      char* end=data+length;
      const char* tmp;
      while(true){
        KeySpec ks;
        while(irr::isspace(*data)){++data;}
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
    
  public:
    
    void save(irr::IWriteFile* out){
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
          out->write(str.c_str(),str.size());
          str=irr::stringc();
        }
      }
      out->write(str.c_str(),str.size());
    }
    

    void load(irr::IReadFile* in){
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
        irr::s32 read=in->read(buf+avail,len-avail);
        avail+=read;
        buf[avail]='\0';
        used=this->doparse(buf,avail);
        if(read==0)
          break;
      }
      delete [] buf;
    }
};

#endif

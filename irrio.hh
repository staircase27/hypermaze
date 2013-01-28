#include "irrlicht.h"
namespace irr{
  using namespace core;
  using namespace scene;
  using namespace io;
  using namespace video;
};

#ifndef IRRIO_HH_INC
#define IRRIO_HH_INC

struct Used{
  irr::u32 amount;
  bool finished;
  Used(irr::u32 amount,bool finished):amount(amount),finished(finished){};
};

class InputParser{
  public:
    virtual Used parse(char* data,irr::u32 length,bool eof)=0;
    virtual ~InputParser(){};
};

template <class T,class PP>
class Derefer{
  public:
    PP data;
    Derefer<T,PP>& operator++(){
      data++;
      return *this;
    }
    bool operator!=(const Derefer<T,PP>& o){
      return *data!=*o.data;
    }
    T* operator->(){
      return *data;
    }
    Derefer(PP data):data(data){};
};

template <class P>
class SequentialInputParser: public InputParser{
  P current;
  public:
    const P end;
    SequentialInputParser(P begin,P end):end(end),current(begin){};
    virtual Used parse(char* data,irr::u32 length,bool eof){
      Used total(0,true);
      while(current!=end && total.finished){
        Used used=current->parse(data+total.amount,length-total.amount,eof);
        total.amount+=used.amount;
        if(used.finished)
          ++current;
        else
          total.finished=false;
      }
      if(eof && current!=end)
        while(++current!=end)
          current->parse((char*)0,0,true);
      return total;
    }
};

inline void parse(irr::IReadFile* in,InputParser* parser){
  irr::u32 len=256;
  char *buf=new char[len];
  irr::u32 avail=0;
  Used used(0,false);
  while(!used.finished){
    if(used.amount!=0){
      memmove(buf,buf+used.amount,avail-used.amount);
      avail-=used.amount;
    }else if(avail==len){
      len*=2;
      char* tmp=new char[len];
      memcpy(tmp,buf,avail);
      delete[] buf;
      buf=tmp;
    }
    irr::s32 read=in->read(buf+avail,len-avail);
    avail+=read;
    used=parser->parse(buf,avail,read==0);
    if(read==0)
      used.finished=true;
  }
  delete [] buf;
}

#endif

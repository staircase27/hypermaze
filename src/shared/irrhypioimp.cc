/**
 * @file irrhypioimp.cc
 * @brief Implementation of irrhypioimp.hh
 */
#include "irrhypioimp.hh"
#include <cstring>
using namespace std;
namespace irr{
  using namespace io;
}

#ifdef IRRLICHT
IrrHypIStream::IrrHypIStream(irr::IReadFile* f):f(f){f->grab();}

IrrHypIStream::~IrrHypIStream(){f->drop();}

void IrrHypIStream::readtobuf(){
  if(eof)
    return;
  if(start!=0){
    memmove(buf,buf+start,end-start);
    end-=start;
    start=0;
  }
  if(end==len){
    len*=2;
    char* tmp=new char[len+1];
    memcopy(tmp,buf,end);
    delete[] buf;
    buf=tmp;
  }
  irr::s32 read=f->read(buf+end,len-end);
  end+=read;
  buf[end]='\0';
  if(read==0)
    eof=true;
}
IrrHypOStream::IrrHypOStream(irr::IWriteFile* f):f(f){f->grab();}
IrrHypOStream::~IrrHypOStream(){
  writeToSink();
  f->drop();
}

bool IrrHypOStream::writeToSink(){
  bool valid=f->write(buf,end)==end;
  end=0;
  return valid;
}
#endif

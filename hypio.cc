#include "hypio.hh"
#include <iostream>
using namespace std;
BufHypIStream::BufHypIStream():len(255),buf(new char[len+1]),start(0),end(0),eof(false){}

BufHypIStream::~BufHypIStream(){
  delete[] buf;
}

void BufHypIStream::consumewhitespace(){
  if(start==end)
    if(eof)
      return;
    else
      readtobuf();
  while(HypIStream::isspace(buf[start]))
    if(++start==end)
      if(eof)
        return;
      else
        readtobuf();
}
IOResult BufHypIStream::read(int& i,const int& base){
  consumewhitespace();//ensure first char is not whitespace so can check if anything was parsed
  char* rend=0;
  long l=strtol(buf+start,&rend,base);
  while(rend==buf+end && !eof){//try reading more data
    readtobuf();
    l=strtol(buf+start,&rend,base);//try again with more data
  }
  //must have used something
  if(rend==buf+start)
    return IOResult(false,rend==buf+end);
  start=rend-buf;
  //must either be at end of file or have whitespace after the number
  if(! (HypIStream::isspace(*rend) || start==end) )
    return IOResult(false,start==end);
  i=(int)l;//cast. Am just going to force the cast for now may add checks later
  return IOResult(true,start==end);
}
void BufHypIStream::mergebufs(char*& addto,int& tolen,char* addfrom,int& fromstart,int& fromlen){
  char* tmp=new char[tolen+fromlen+1];
  memcpy(tmp,addto,tolen);
  memcpy(tmp+tolen,addfrom+fromstart,fromlen);
  tmp[tolen+fromlen]='\0';
  delete[] addto;
  addto=tmp;
  fromstart+=fromlen;
  tolen+=fromlen;
  fromlen=0;
}
IOResult BufHypIStream::read(char*& str,const bool& delim){
  char d;
  consumewhitespace();
  if(delim){
    d=*(buf+start);
    ++start;
  }
  char* sb=0;
  int sblen=0;
  int l=0;
  while(! (delim ? *(buf+start+l)==d : HypIStream::isspace(*(buf+start+l))) ){
    ++l;
    if(start+l==end){
      if(eof)
        break;
      mergebufs(sb,sblen,buf,start,l);
      readtobuf();
    }
  }
  mergebufs(sb,sblen,buf,start,l);
  if( delim && !*(buf+start)==d){
    delete[] sb;
    return IOResult(false,start==end);
  }
  str=sb;
  return IOResult(true,start==end);
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
    memcpy(tmp,buf,end);
    delete[] buf;
    buf=tmp;
  }
  irr::s32 read=f->read(buf+end,len-end);
  end+=read;
  buf[end]='\0';
  if(read==0)
    eof=true;
}
#endif
IOResult read(HypIStream& s,int& i,const int& base){
  return s.read(i,base);
}
IOResult read(HypIStream& s,char* str,const bool& delim){
  return s.read(str,delim);
}

BufHypOStream::~BufHypOStream(){
  delete[] buf;
}



bool write(HypOStream& s,const int& i,const int& base){
  return s.write(i,base);
}
bool write(HypOStream& s,const char*& str,const bool& delim){
  return s.write(str,delim);
}


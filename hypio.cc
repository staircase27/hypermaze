/**
 * @file hypio.cc
 * @brief Implementation of hypio.hh
 */
#include "hypio.hh"
#include "hypioimp.hh"
#include <cstdlib>


#ifdef IOSTREAM

#include <string>
#include <iostream>
using namespace std;

IOResult CPPHypIStream::read(int& i, const int& base){
  std::istream::sentry s(is);
  if (!s)
    return IOResult(false,is.eof());
  if (base==0){
    char c=is.peek();
    if(c!='0'){
      is>>dec;
    }else{
      s>>c;
      c=is.peek();
      if(c=='x'||c=='X'){
        is>>hex;
      }else{
        is>>oct;
      }
    }
  }else if (base==10){
    is>>dec;
  }else if(base==8){
    is>>oct;
  }else if(base==16){
    is>>hex;
  }
  is>>i;
  return IOResult(is.good(),is.eof());
}

IOResult CPPHypIStream::read(SPA<char const>& str,const bool& quote){
  is>>ws;
  if(!is.good())
    return IOResult(false,is.eof());
  string st;
  if(quote){
    char d;
    is>>d;
    getline(is,st,d);
  }else{
    is>>st;
  }
  SPA<char> tmp(st.length()+1);
  memcopy(tmp,st.c_str(),st.length());
  tmp[st.length()]='\0';
  str=tmp;
  return IOResult(is.good(),is.eof());
}
#endif

BufHypIStream::BufHypIStream():len(255),buf(new char[len+1]),start(0),end(0),eof(false){}

BufHypIStream::~BufHypIStream(){
  delete[] buf;
}

void BufHypIStream::consumewhitespace(){
  if(start>=end)
    if(eof)
      return;
    else
      readtobuf();
  while(HypIStream::isspace(buf[start])){
    if(++start>=end){
      if(eof)
        return;
      else
        readtobuf();
    }
  }
}
IOResult BufHypIStream::read(int& i,const int& base){
  consumewhitespace();//ensure first char is not whitespace so can check if anything was parsed
  char* rend=0;
  long l=strtol(buf+start,&rend,base);
  while(rend>=buf+end && !eof){//try reading more data
    readtobuf();
    l=strtol(buf+start,&rend,base);//try again with more data
  }
  //must have used something
  if(rend==buf+start){
    bool neg=false;
    if(*rend=='-'){
      neg=true;
      ++rend;
    }
    if(!(*rend=='*'))
      return IOResult(false,rend==buf+end);
    ++rend;
    if(neg)
      l=INT_MIN;
    else
      l=INT_MAX;
  }
  start=rend-buf;
  //must either be at end of file or have whitespace after the number
  if(! (start>=end || HypIStream::isspace(*rend)) )
    return IOResult(false,start==end);
  i=(int)l;//cast. Am just going to force the cast for now may add checks later
  return IOResult(true,start==end);
};
void BufHypIStream::mergebufs(SPA<char>& addto,int& tolen,char const* const& addfrom,int& fromstart,int& fromlen){
  SPA<char> tmp(tolen+fromlen+1);
  memcopy(tmp,addto,tolen);
  memcopy(tmp+tolen,addfrom+fromstart,fromlen);
  tmp[tolen+fromlen]='\0';
  addto=tmp;
  fromstart+=fromlen;
  tolen+=fromlen;
  fromlen=0;
}
IOResult BufHypIStream::read(SPA<char const>& str,const bool& quote){
  char d;
  consumewhitespace();
  if(quote){
    d=*(buf+start);
    ++start;
  }
  SPA<char> sb  ;
  int sblen=0;
  int l=0;
  while(! (quote ? *(buf+start+l)==d : HypIStream::isspace(*(buf+start+l))) ){
    ++l;
    if(start+l==end){
      if(eof)
        break;
      mergebufs(sb,sblen,buf,start,l);
      readtobuf();
    }
  }
  mergebufs(sb,sblen,buf,start,l);
  if( quote )
    if(!(*(buf+start)==d))
      return IOResult(false,start==end);
    else
      ++start;
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
#endif
MemoryHypIStream::MemoryHypIStream(SPA<const char> _buf,int _len){
  delete[] buf;
  len=_len+1;
  buf=new char[len];
  memcopy(buf,_buf,_len);
  buf[_len]='\0';
  end=_len;
  eof=true;
}
MemoryHypIStream::MemoryHypIStream(const char* _buf,int _len){
  delete[] buf;
  len=_len+1;
  buf=new char[len];
  memcopy(buf,_buf,_len);
  buf[_len]='\0';
  end=_len;
  eof=true;
}

IOResult read(HypIStream& s,int& i,const int& base){
  return s.read(i,base);
}
IOResult read(HypIStream& s,SPA<char const>& str,const bool& quote){
  return s.read(str,quote);
}


const char* BaseHypOStream::quotechars="\"'|\\/^_!@#~.=+-$*\3\1\2";


#ifdef IOSTREAM


bool CPPHypOStream::write(const int& i,const int& base){
  os<<nextSpace();
  if(base==16){
    os<<hex;
  }else if(base==8){
    os<<oct;
  }else{
    os<<dec;
  }
  os<<i;
  return os.good();
}

bool CPPHypOStream::write(const char*& str,const bool& quote){
  os<<nextSpace();
  const char* d=quotechars;
  if(quote){
    while(strchr(str,*d)!=0)
      ++d;
    os<<*d;
  }
  os<<str;
  if(quote)
    os<<*d;
  return os.good();  
}
#endif

BufHypOStream::BufHypOStream():len(255),buf(new char[len+1]),end(0){}

BufHypOStream::~BufHypOStream(){
  delete[] buf;
}

bool BufHypOStream::addSpace(){
  const char* space=nextSpace();
  int l=strlen(space);
  if(end+l>len)
    writeToSink();
  memcopy(buf+end,space,l);
  end+=l;
}

bool BufHypOStream::write(const int& _i,const int& _base){
  addSpace();
  int i=_i;
  int base=_base;
  // if negative add "-" to buffer and make positive
  if (i < 0){
    if(end+1>len)
      writeToSink();
    buf[end]='-';
    ++end;
    if(i==INT_MIN){
      if(end+1>len)
        writeToSink();
      buf[end]='*';
      ++end;
      return true;
    }
    i *= -1;
  }else
    if(i==INT_MAX){
      if(end+1>len)
        writeToSink();
      buf[end]='*';
      ++end;
      return true;
    }
  if(!i){
    if(end+1>len)
      writeToSink();
    buf[end]='0';
    ++end;
  }else{
    if(base==0)
      base=10;
    char* ibuf=new char[16];
    int idx = 16;
    int ilen = 16;
    // add numbers
    while(i){
      if(--idx<0){
        char* tmp=new char[len+16];
        memcopy(tmp+16,ibuf,len);
        delete[] ibuf;
        ibuf=tmp;
        ilen+=16;
        idx+=16;
      }
      if((i % base)>9)
        ibuf[idx] = (char)('a' - 10 + (i % base));
      else
        ibuf[idx] = (char)('0' + (i % base));
      i /= base;
    }
    if(end+ilen-idx>len)
      writeToSink();
    memcopy(buf+end,ibuf+idx,ilen-idx);
    end+=ilen-idx;
    delete[] ibuf;
  }
  return true;
}
bool BufHypOStream::write(const char*& str,const bool& quote){
  addSpace();
  const char* d=quotechars;
  if(quote){
    while(strchr(str,*d)!=0)
      ++d;
    if(end+1>len)
      writeToSink();
    buf[end]=*d;
    ++end;
  }
  int l=strlen(str);
  int s=0;
  while(l-s>len-end){
    memcopy(buf+end,str+s,len-end);
    end=len;
    s+=len-end;
    writeToSink();
  }
  memcopy(buf+end,str+s,l-s);
  end+=l-s;
  if(quote){
    if(end+1>len)
      writeToSink();
    buf[end]=*d;
    ++end;
  }
  return true;
}

#ifdef IRRLICHT
IrrHypOStream::IrrHypOStream(irr::IWriteFile* f):f(f){f->grab();}
IrrHypOStream::~IrrHypOStream(){
  writeToSink();
  f->drop();
}

bool IrrHypOStream::writeToSink(){
  bool valid=f->write(buf,end)==end;
  end=0;
}
#endif
MemoryHypOStream::MemoryHypOStream(SPA<char>& str):str(str),strlen(0){};

bool MemoryHypOStream::writeToSink(){
  SPA<char> tmp(strlen+end+1);
  memcopy(tmp,str,strlen);
  memcopy((tmp+strlen),buf,end);
  tmp[strlen+end]='\0';
  str=tmp;
  strlen=strlen+end;
  end=0;
}

bool write(HypOStream& s,const int& i,const int& base){
  return s.write(i,base);
}
bool write(HypOStream& s,const char*& str,const bool& quote){
  return s.write(str,quote);
}

IOResult read(HypIStream& s,bool& b){
  int tmp;
  IOResult r=read(s,tmp,0);
  b=(bool)tmp;
  return r;
}
bool write(HypOStream& s,const bool& b){
  return write(s,(int)b,0);
}

bool write(HypOStream& s,const SPA<const char>& str,const bool& quote){
  const char* tmp=&*str;
  return write(s,tmp,quote);
}


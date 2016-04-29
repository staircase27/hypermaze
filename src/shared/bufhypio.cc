/**
 * @file bufhypio.cc
 * @brief Implementation of bufhypio.hh
 */
#include "bufhypio.hh"
#include <cstdlib>
#include <cstring>

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
  // There is at least one char left after start as it was checked and found to not be whitespace
  if(quote){
    d=*(buf+start);
    if(++start>=end){
      if(eof)
        return IOResult(false,true);
      else
        readtobuf();
    }
  }
  SPA<char> sb;
  int sblen=0;
  int l=0;
  while(! (quote ? *(buf+start+l)==d : HypIStream::isspace(*(buf+start+l))) ){
    ++l;
    if(start+l>=end){
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
  return true;
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

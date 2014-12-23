/**
 * @file hypio.cc
 * @brief Implementation of hypio.hh
 */
#include "memoryhypioimp.hh"
#include <cstdlib>

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

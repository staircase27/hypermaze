/**
 * @file hypio.cc
 * @brief Implementation of hypio.hh
 */
#include "hypio.hh"

IOResult read(HypIStream& s,int& i,const int& base){
  return s.read(i,base);
}
IOResult read(HypIStream& s,SPA<char const>& str,const bool& quote){
  return s.read(str,quote);
}

const char* HypOStream::quotechars="\"'|\\/^_!@#~.=+-$*\3\1\2";

bool write(HypOStream& s,const int& i,const int& base){
  return s.write(i,base);
}
bool write(HypOStream& s,const char*& str,const bool& quote){
  return s.write(str,quote);
}

IOResult read(HypIStream& s,bool& b){
  int tmp;
  IOResult r=read(s,tmp,0);
  b=tmp!=0;
  return r;
}
bool write(HypOStream& s,const bool& b){
  return write(s,(int)b,0);
}

bool write(HypOStream& s,const SPA<const char>& str,const bool& quote){
  const char* tmp=&*str;
  return write(s,tmp,quote);
}


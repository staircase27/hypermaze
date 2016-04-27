/**
 * @file cpphypioimp.cc
 * @brief Implementation of cpphypioimp.hh
 */
#include "cpphypioimp.hh"
#include <string>
using namespace std;

#ifdef IOSTREAM

IOResult CPPHypIStream::read(int& i, const int& base){
  std::istream::sentry s(is);
  if (!s)
    return IOResult(false,is.eof());
  bool neg=false;
  int c=is.peek();
  if(c=='-')
    neg=true;
  else if(c=='+')
    neg=false;
  if (base==0){
    is.unsetf(ios_base::basefield);
  }else if (base==10){
    is>>dec;
  }else if(base==8){
    is>>oct;
  }else if(base==16){
    is>>hex;
  }
  if(is.fail()){
    is.clear();
    c=is.peek();
    if(c=='*'){
      is>>c;
      if(neg)
        i=INT_MIN;
      else
        i=INT_MAX;
    }else
      is.clear(is.failbit);
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

bool CPPHypOStream::write(const int& i,const int& base){
  os<<nextSpace();
  if(i==INT_MIN){
    os<<"-*";
    return os.good();
  }else if(i==INT_MAX){
    os<<"*";
    return os.good();
  }
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

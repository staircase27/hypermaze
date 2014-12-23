/**
 * @file hypio.cc
 * @brief Implementation of hypio.hh
 */
#include "cpphypioimp.hh"

#ifdef IOSTREAM

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

#include "bufhypio.hh"
#include <cstring>

#ifndef MEMORYHYPIOIMP_HH_INC
#define MEMORYHYPIOIMP_HH_INC

class MemoryHypIStream: public BufHypIStream{
  public:
    MemoryHypIStream(SPA<const char> buf,int len);
    MemoryHypIStream(char const* buf,int len);
    ~MemoryHypIStream(){};

  protected:
    void readtobuf(){};
};

class MemoryHypOStream: public BufHypOStream{
  public:
    SPA<char>& str;
    int strlen;
    MemoryHypOStream(SPA<char>& str);
    
  protected:
    bool writeToSink();
    
};
#endif

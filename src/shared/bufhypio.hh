#include "hypio.hh"
#include <cstring>

#ifndef BUFHYPIOIMP_HH_INC
#define BUFHYPIOIMP_HH_INC

class BufHypIStream: public HypIStream{
  protected:
    int len;
    char* buf;
    int start;
    int end;
    bool eof;
    
    BufHypIStream();
    
    virtual void readtobuf()=0;
    void consumewhitespace();
    IOResult read(int&,const int&);
    void mergebufs(SPA<char>&,int&,char const* const&,int&,int&);
    IOResult read(SPA<char const>&,const bool&);
  public:
    ///virtual destructor to allow deletion of base class type objects
    virtual ~BufHypIStream();
};
 
class BufHypOStream: public HypOStream{
  protected:
    int len;
    char* buf;
    int end;
    virtual bool writeToSink()=0;
    
    BufHypOStream();
    virtual ~BufHypOStream();
    
    bool addSpace();
    
    bool write(const int&,const int&);
    bool write(const char*&,const bool&);
  public:
    virtual void flush(){
      writeToSink();
    }
    
};
#endif

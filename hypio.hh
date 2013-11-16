#ifdef IRRLICHT
#include "irrlicht.h"
#include <cstring>
namespace irr{
  using namespace core;
  using namespace scene;
  using namespace io;
  using namespace video;
};
#endif
#ifndef HYPIO_HH_INC
#define HYPIO_HH_INC 

struct IOResult{
  bool ok;
  bool eof;
  IOResult(bool ok,bool eof):ok(ok),eof(eof){};
  IOResult():ok(true),eof(false){};
};

class HypIStream{
  protected:
    virtual IOResult read(int&,const int&)=0;
    virtual IOResult read(char*&,const bool&)=0;
    friend IOResult read(HypIStream&,int&,const int&);
    friend IOResult read(HypIStream&,char*,const bool&);
  public:
    inline static bool isspace(const char& c){return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v';}
};
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
    virtual IOResult read(int&,const int&);
    void mergebufs(char*&,int&,char*,int&,int&);
    virtual IOResult read(char*&,const bool&);
};
 
#ifdef IRRLICHT
class IrrHypIStream: public BufHypIStream{
  irr::IReadFile* f;

  public:
    IrrHypIStream(irr::IReadFile* f);

  protected:
    virtual void readtobuf();
};
#endif

IOResult read(HypIStream& s,int& i,const int& base);
IOResult read(HypIStream& s,char* str,const bool& delim);
#endif
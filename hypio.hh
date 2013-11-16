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
    virtual ~HypIStream(){};
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
  public:
    virtual ~BufHypIStream();
};
 
#ifdef IRRLICHT
class IrrHypIStream: public BufHypIStream{
  irr::IReadFile* f;

  public:
    IrrHypIStream(irr::IReadFile* f);
    ~IrrHypIStream();

  protected:
    virtual void readtobuf();
};
#endif

IOResult read(HypIStream& s,int& i,const int& base);
IOResult read(HypIStream& s,char* str,const bool& delim);

class HypOStream{
  const char* defaultspace;
  const char* nextspace;
  protected:
    HypOStream():defaultspace(" "),nextspace(" "){}
    
    virtual bool write(const int&,const int&)=0;
    virtual bool write(const char*&,const bool&)=0;
    friend bool write(HypOStream&,const int&,const int&);
    friend bool write(HypOStream&,const char*&,const bool&);
    const char* nextSpace(){
      const char* tmp=nextspace;
      nextspace=defaultspace;
      return tmp;
    }
  public:
    virtual ~HypOStream(){};
    
    void setNextSpace(const char* space){
      nextspace=space;
    }
    void setDefaultSpace(const char* space){
      defaultspace=space;
    }
};

class BufHypOStream: public HypOStream{
  protected:
    int len;
    char* buf;
    int end;
    virtual void writetosink()=0;
    const char* delimchars;
    
    BufHypOStream():delimchars("\"'|\\//^_!@#~.=+-$*\3\1\2"),len(255),buf(new char[len+1]),end(0){}
    
    ~BufHypOStream();
    
    virtual bool write(const int&,const int&){
      return false;
    }
    virtual bool write(const char*&,const bool&){
      return false;
    }
    
};
bool write(HypOStream&,const int&,const int&);
bool write(HypOStream&,const char*&,const bool&);

#endif
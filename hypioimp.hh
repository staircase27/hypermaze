#include "hypio.hh"
#ifdef IRRLICHT
#include "irrlicht.h"
namespace irr{
  using namespace core;
  using namespace scene;
  using namespace io;
  using namespace video;
};
#endif
#include <cstring>

#ifndef HYPIOIMP_HH_INC
#define HYPIOIMP_HH_INC
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
    void mergebufs(char*&,int&,char*,int&,int&);
    IOResult read(char*&,const bool&);
  public:
    virtual ~BufHypIStream();
};
 
#ifdef IRRLICHT
class IrrHypIStream: public BufHypIStream{
  irr::IReadFile* f;

  public:
    IrrHypIStream(irr::IReadFile*);
    ~IrrHypIStream();

  protected:
    void readtobuf();
};
#endif

class BufHypOStream: public HypOStream{
  protected:
    int len;
    char* buf;
    int end;
    virtual bool writeToSink()=0;
    const char* delimchars;
    bool needspace;
    
    BufHypOStream();
    virtual ~BufHypOStream();
    
    bool addSpace();
    
    bool write(const int&,const int&);
    bool write(const char*&,const bool&);
    
};

#ifdef IRRLICHT
class IrrHypOStream: public BufHypOStream{
  irr::IWriteFile* f;
  
  public:
    IrrHypOStream(irr::IWriteFile*);
    virtual ~IrrHypOStream();
    
  protected:
    bool writeToSink();
    
};
#endif

//test
#endif
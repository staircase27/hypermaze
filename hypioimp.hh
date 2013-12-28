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
    ///virtual destructor to allow deletion of base class type objects
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

class MemoryHypIStream: public BufHypIStream{
  public:
    MemoryHypIStream(SPA<const char> buf,int len);
    MemoryHypIStream(char const* buf,int len);
    ~MemoryHypIStream(){};

  protected:
    void readtobuf(){};
};

class BufHypOStream: public HypOStream{
  protected:
    int len;
    char* buf;
    int end;
    virtual bool writeToSink()=0;
    const char* quotechars;
    bool needspace;
    
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
class MemoryHypOStream: public BufHypOStream{
  public:
    SPA<char>& str;
    int strlen;
    MemoryHypOStream(SPA<char>& str);
    
  protected:
    bool writeToSink();
    
};

#endif

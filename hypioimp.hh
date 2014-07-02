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

#ifdef IOSTREAM
class CPPHypIStream: public HypIStream{
  std::istream &is;
  public:
    CPPHypIStream(std::istream& is):is(is){};
  protected:
    IOResult read(int&,const int&);
    IOResult read(SPA<char const>&,const bool&);
};
#endif

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


class BaseHypOStream: public HypOStream{
  protected:
    static const char* quotechars;
    
    BaseHypOStream(){};
    virtual ~BaseHypOStream(){};
};

#ifdef IOSTREAM
class CPPHypOStream: public BaseHypOStream{
  std::ostream &os;
  public:
    CPPHypOStream(std::ostream& os):os(os){};
  protected:
    bool write(const int&,const int&);
    bool write(const char*&,const bool&);
    virtual void flush(){
      os.flush();
    }
};
#endif

class BufHypOStream: public BaseHypOStream{
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

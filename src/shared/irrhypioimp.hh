#include "bufhypio.hh"
#ifdef IRRLICHT
#include "irrlicht.h"
namespace irr{
  using namespace core;
  using namespace scene;
  using namespace io;
  using namespace video;
};
#include <cstring>

#ifndef IRRHYPIOIMP_HH_INC
#define IRRHYPIOIMP_HH_INC

class IrrHypIStream: public BufHypIStream{
  irr::IReadFile* f;

  public:
    IrrHypIStream(irr::IReadFile*);
    ~IrrHypIStream();

  protected:
    void readtobuf();
};

class IrrHypOStream: public BufHypOStream{
  irr::IWriteFile* f;
  
  public:
    IrrHypOStream(irr::IWriteFile*);
    virtual ~IrrHypOStream();
    
  protected:
    bool writeToSink();
    
};
#endif
#endif

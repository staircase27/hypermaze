#include "bufhypio.hh"
#ifdef IRRLICHT
#include "irrlicht.h"

#ifndef IRRHYPIOIMP_HH_INC
#define IRRHYPIOIMP_HH_INC

class IrrHypIStream: public BufHypIStream{
  irr::io::IReadFile* f;

  public:
    IrrHypIStream(irr::io::IReadFile*);
    ~IrrHypIStream();

  protected:
    void readtobuf();
};

class IrrHypOStream: public BufHypOStream{
  irr::io::IWriteFile* f;

  public:
    IrrHypOStream(irr::io::IWriteFile*);
    virtual ~IrrHypOStream();

  protected:
    bool writeToSink();

};
#endif
#endif

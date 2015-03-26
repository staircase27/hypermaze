/**
 * @file irrhypioimp.hh
 * @brief An implementation of hypio that uses c++ iostreams
 * @note if IRRLICHT isn't defined then this header doesn't define anything
 */
#include "bufhypio.hh"
#ifdef IRRLICHT
#include "irrlicht.h"

#ifndef IRRHYPIOIMP_HH_INC
#define IRRHYPIOIMP_HH_INC

///An implementation of HypIStream using an Irrlicht IReadFile
class IrrHypIStream: public BufHypIStream{
  irr::io::IReadFile* f;///< the IReadFile this reads from

  public:
    ///Create a HypIStream wrapping the provided IReadFile
    /**
     * @param f the file to read from
     */
    IrrHypIStream(irr::io::IReadFile* f);
    ///flush data and release the IReadFile
    ~IrrHypIStream();

  protected:
    //doc copied
    void readtobuf();
};

///An implementation of HypOStream using an Irrlicht IWriteFile
class IrrHypOStream: public BufHypOStream{
  irr::io::IWriteFile* f;///< the IWriteFile this writes to

  public:
    ///Create a HypOStream wrapping the provided IWriteFile
    /**
     * @param f the file to write to
     */
    IrrHypOStream(irr::io::IWriteFile* f);
    ///flush data and release the IWriteFile
    virtual ~IrrHypOStream();

  protected:
    //doc copied
    bool writeToSink();

};
#endif
#endif

/**
 * @file cpphypioimp.hh
 * @brief An implementation of hypio that uses c++ iostreams
 * @note if IOSTREAM isn't defined then this header doesn't define anything
 */
#include "../core/hypio.hh"
#include <cstring>

#ifdef IOSTREAM
#include <iostream>

#ifndef CPPHYPIOIMP_HH_INC
#define CPPHYPIOIMP_HH_INC

///An implementation of HypIStream using a c++ std:istream
class CPPHypIStream: public HypIStream{
  std::istream &is;///< The istream that this reads from
  public:
    /// Create a CPPHypIStream for the specified istream
    /**
     * @param is the stream to read from
     */
    CPPHypIStream(std::istream& is):is(is){};
  protected:
    //doc copied
    IOResult read(int&,const int&);
    //doc copied
    IOResult read(SPA<char const>&,const bool&);
};
///An implementation of HypOStream using a c++ std:ostream
class CPPHypOStream: public HypOStream{
  std::ostream &os;///< The ostream that this writes to
  public:
    /// Create a CPPHypOStream for the specified ostream
    /**
     * @param os the stream to write to
     */
    CPPHypOStream(std::ostream& os):os(os){};
  protected:
    //doc copied
    bool write(const int&,const int&);
    //doc copied
    bool write(const char*&,const bool&);
    /// Flushes the ostream we wrap
    virtual void flush(){
      os.flush();
    }
};
#endif
#endif

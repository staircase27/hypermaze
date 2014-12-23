#include "../core/hypio.hh"
#include <cstring>

#ifdef IOSTREAM
#include <iostream>

#ifndef CPPHYPIOIMP_HH_INC
#define CPPHYPIOIMP_HH_INC
class CPPHypIStream: public HypIStream{
  std::istream &is;
  public:
    CPPHypIStream(std::istream& is):is(is){};
  protected:
    IOResult read(int&,const int&);
    IOResult read(SPA<char const>&,const bool&);
};
class CPPHypOStream: public HypOStream{
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
#endif

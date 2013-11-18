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

IOResult read(HypIStream& s,int& i,const int& base=0);
IOResult read(HypIStream& s,char* str,const bool& delim);

class HypOStream{
  const char* defaultspace;
  const char* nextspace;
  bool atstart;
  protected:
    HypOStream():defaultspace(" "),nextspace(" "),atstart(true){}
    
    virtual bool write(const int&,const int&)=0;
    virtual bool write(const char*&,const bool&)=0;
    friend bool write(HypOStream&,const int&,const int&);
    friend bool write(HypOStream&,const char*&,const bool&);
    const char* nextSpace(){
      const char* tmp=nextspace;
      if(atstart){
        tmp="";
        atstart=false;
      }
      nextspace=defaultspace;
      return tmp;
    }
  public:
    virtual ~HypOStream(){};
    
    inline void setNextSpace(const char* space){
      nextspace=space;
    }
    inline void setDefaultSpace(const char* space){
      defaultspace=space;
    }
};

bool write(HypOStream& s,const int& i,const int& base=0);
bool write(HypOStream& s,const char*& str,const bool& delim);

#endif
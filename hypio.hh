struct IOResult{
  bool ok;
};


Class HypIStream{
  public:
    bool isspace(const char& c){return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v'}
    virtual IOResult read(int&,const int&)=0;
    virtual IOResult read(char*&,const bool&)=0;
}
Class IrrHypIStream: public HypIStream{
  irr::IReadFile* f;
  char* buf;
  int len;
  int start;
  int end;
  bool eof;

  public:
    IrrHypIStream(irr::IReadFile* f):f(f),len(256),buf(new char[len+1]),start(0),end(0),eof(false){}

  private:  
    void readtobuf(){
      if(eof)
        return
      if(start!=0){
        memmove(buf,buf+start,end-start);
        end-=start;
        start=0;
      }
      if(end==len){
        len*=2;
        char* tmp=new char[len+1];
        memcpy(tmp,buf,end);
        delete[] buf;
        buf=tmp;
      }
      irr::s32 read=f->read(buf+end,len-end);
      buf[end]='\0';
      if(read==0)
        eof=true;
      end+=read;
    }
    void consumewhitespace(){
      while(isspace(buf[start]))
        if(++start==end)
          if(eof)
            return
          else
            readtobuf()
    }
      
  public:
    virtual IOResult read(int& i,const int& base){
      char* end=0;
      long l=strtol(buf+start,&end,base)
      //check end and convert to int from long
    }

    virtual IOResult read(char*& str,const bool& delim){
      
    }
}

IOResult read(HypIStream* s,int& i,const int& base){
  return s->read(i,base);
}
IOResult read(HypIStream* s,char* str,const bool& delim){
  return s->read(str,delim);
}

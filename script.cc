#include "script.hh"
#include "scriptimpl.hh"

class ConditionParser:public InputParser{
  InputParser** dataParser;
  Condition** newCondition;
  public:
    virtual Used parse(char* data,irr::u32 length,bool eof);
    ConditionParser(InputParser** dataParser,Condition** newCondition):dataParser(dataParser),newCondition(newCondition){};
    virtual ~ConditionParser();
};
Used ConditionParser::parse(char* data,irr::u32 length,bool eof){
  char* start=data;
  char* end=data+length;
  if(!eof)
    end-=1;
  int type=strtol(data,&data,10);
  if(data>=end) return Used(0,false);
  switch(type){
    case 1:
      *newCondition=new ConditionTrue();
      *dataParser=(*newCondition)->createParser();
      break;
    case 2:
      *newCondition=new ConditionOr();
      *dataParser=(*newCondition)->createParser();
      break;
    case 3:
      *newCondition=new ConditionAnd();
      *dataParser=(*newCondition)->createParser();
      break;
    case 4:
      *newCondition=new ConditionNot();
      *dataParser=(*newCondition)->createParser();
      break;
    case 5:
      *newCondition=new ConditionAfter();
      *dataParser=(*newCondition)->createParser();
      break;
    case 6:
      *newCondition=new ConditionBefore();
      *dataParser=(*newCondition)->createParser();
      break;
    case 7:
      *newCondition=new ConditionStringPosition();
      *dataParser=(*newCondition)->createParser();
      break;
    case 8:
      *newCondition=new ConditionStringSelection();
      *dataParser=(*newCondition)->createParser();
      break;
    default:
      *newCondition=new ConditionTrue();
      *dataParser=(*newCondition)->createParser();
  }
  return Used(data-start,true);
}
ConditionParser::~ConditionParser(){
  (*newCondition)->returnParser(*dataParser);
}
class ConditionListParser:public InputParser{
  InputParser** dataParser;
  Condition*** newConditions;
  int* count;
  public:
    virtual Used parse(char* data,irr::u32 length,bool eof);
    ConditionListParser(InputParser** dataParser,Condition*** newConditions,int* count):
        dataParser(dataParser),newConditions(newConditions),count(count){};
    virtual ~ConditionListParser();
};
Used ConditionListParser::parse(char* data,irr::u32 length,bool eof){
  char* start=data;
  char* end=data+length;
  if(!eof)
    end-=1;
  *count=strtol(data,&data,10);
  if(data>=end) return Used(0,false);
  *newConditions=new Condition*[*count];
  InputParser** parsers=new InputParser*[2*(*count)];
  for(int i=0;i<*count;++i)
    parsers[i*2]=new ConditionParser(parsers+i*2+1,(*newConditions)+i);
  *dataParser=new SequentialInputParser<Derefer<InputParser,InputParser**> >(
      Derefer<InputParser,InputParser**>(parsers),
      Derefer<InputParser,InputParser**>(parsers+2*(*count)));
  return Used(data-start,true);
}
ConditionListParser::~ConditionListParser(){
  SequentialInputParser<Derefer<InputParser,InputParser**> >* p=(SequentialInputParser<Derefer<InputParser,InputParser**> >*)(*dataParser);
  for(int i=0;i<*count;++i)
    delete *(p->end.data-2*(*count)+2*i);
  delete[] (p->end.data-2*(*count));
  delete p;
}


bool ConditionOr::is(int time,Script script,const String& s){
  for(int i=0;i<count;++i)
    if(conditions[i]->is(time,script,s))
      return true;
  return false;
}
InputParser* ConditionOr::createParser(){
  InputParser** parsers=new InputParser*[2];
  parsers[0]=new ConditionListParser(parsers+1,&conditions,&count);
  return new SequentialInputParser<Derefer<InputParser,InputParser**> >(
    Derefer<InputParser,InputParser**>(parsers),
    Derefer<InputParser,InputParser**>(parsers+2));
};
void ConditionOr::returnParser(InputParser* parser){
  SequentialInputParser<Derefer<InputParser,InputParser**> >* p=(SequentialInputParser<Derefer<InputParser,InputParser**> >*)parser;
  delete *(p->end.data-2);
  delete[] (p->end.data-2);
  delete parser;
};
void ConditionOr::output(irr::stringc* s,irr::IWriteFile* file){
  (*s)+="2 ";(*s)+=count;(*s)+="\n";
  for(int i=0;i<count;++i)
    conditions[i]->output(s,file);
  if(file && s->size()>256){
    file->write(s->c_str(),s->size());
    *s=irr::stringc();
  }
}
ConditionOr::~ConditionOr(){
  for(int i=0;i<count;++i)
    delete conditions[i];
  delete[] conditions;
}

bool ConditionAnd::is(int time,Script script,const String& s){
  for(int i=0;i<count;++i)
    if(!conditions[i]->is(time,script,s))
      return false;
  return true;
}
InputParser* ConditionAnd::createParser(){
  InputParser** parsers=new InputParser*[2];
  parsers[0]=new ConditionListParser(parsers+1,&conditions,&count);
  return new SequentialInputParser<Derefer<InputParser,InputParser**> >(
    Derefer<InputParser,InputParser**>(parsers),
    Derefer<InputParser,InputParser**>(parsers+2));
};
void ConditionAnd::returnParser(InputParser* parser){
  SequentialInputParser<Derefer<InputParser,InputParser**> >* p=(SequentialInputParser<Derefer<InputParser,InputParser**> >*)parser;
  delete *(p->end.data-2);
  delete[] (p->end.data-2);
  delete parser;
};
void ConditionAnd::output(irr::stringc* s,irr::IWriteFile* file){
  (*s)+="3 ";(*s)+=count;(*s)+="\n";
  for(int i=0;i<count;++i)
    conditions[i]->output(s,file);
  if(file && s->size()>256){
    file->write(s->c_str(),s->size());
    *s=irr::stringc();
  }
}
ConditionAnd::~ConditionAnd(){
  for(int i=0;i<count;++i)
    delete conditions[i];
  delete[] conditions;
}

InputParser* ConditionNot::createParser(){
  InputParser** parsers=new InputParser*[2];
  parsers[0]=new ConditionParser(parsers+1,&condition);
  return new SequentialInputParser<Derefer<InputParser,InputParser**> >(
    Derefer<InputParser,InputParser**>(parsers),
    Derefer<InputParser,InputParser**>(parsers+2));
};
void ConditionNot::returnParser(InputParser* parser){
  SequentialInputParser<Derefer<InputParser,InputParser**> >* p=(SequentialInputParser<Derefer<InputParser,InputParser**> >*)parser;
  delete *(p->end.data-2);
  delete[] (p->end.data-2);
  delete parser;
};
void ConditionNot::output(irr::stringc* s,irr::IWriteFile* file){
  (*s)+="4\n";
  condition->output(s,file);
  if(file && s->size()>256){
    file->write(s->c_str(),s->size());
    *s=irr::stringc();
  }
}

bool ConditionAfter::is(int time,Script script,const String& s){
  return script.getTime(event)+delay<=time;
}
Used ConditionAfter::parse(char* data,irr::u32 length,bool eof){
  char* start=data;
  char* end=data+length;
  if(!eof)
    end-=1;
  event=strtol(data,&data,10);
  if(data>=end) return Used(0,false);
  delay=strtol(data,&data,10);
  if(data>=end) return Used(0,false);
  return Used(data-start,true);
}
void ConditionAfter::output(irr::stringc* s,irr::IWriteFile* file){
  (*s)+="5 ";(*s)+=event;(*s)+=" ";(*s)+=delay;(*s)+="\n";
  if(file && s->size()>256){
    file->write(s->c_str(),s->size());
    *s=irr::stringc();
  }
}

bool ConditionBefore::is(int time,Script script,const String& s){
  return script.getTime(event)==0;
}
Used ConditionBefore::parse(char* data,irr::u32 length,bool eof){
  char* start=data;
  char* end=data+length;
  if(!eof)
    end-=1;
  event=strtol(data,&data,10);
  if(data>=end) return Used(0,false);
  return Used(data-start,true);
}
void ConditionBefore::output(irr::stringc* s,irr::IWriteFile* file){
  (*s)+="6 ";(*s)+=event;(*s)+="\n";
  if(file && s->size()>256){
    file->write(s->c_str(),s->size());
    *s=irr::stringc();
  }
}

class ConditionStringPositionParser: public InputParser{
  ConditionStringPosition* c;
  int i;
  public:
    ConditionStringPositionParser(ConditionStringPosition* c):c(c),i(-2){};
  
    Used parse(char* data,irr::u32 length,bool eof){
      char* start=data;
      char* end=data+length;
      if(!eof)
        end-=1;
      if(i==-1){
        c->tiestart=strtol(data,&data,10);
        if(data>=end) return Used(0,false);

        c->tieend=strtol(data,&data,10);
        if(data>=end) return Used(0,false);

        c->count=strtol(data,&data,10);
        if(data>=end) return Used(0,false);
        c->poss=new Vector[c->count];
        i=0;
      }
      
      char* tmp;
      while(i<c->count){
        c->poss[i].X=strtol(data,&tmp,10);
        if(tmp>=end) return Used(data-start,false);
        c->poss[i].Y=strtol(tmp,&tmp,10);
        if(tmp>=end) return Used(data-start,false);
        c->poss[i].Z=strtol(tmp,&tmp,10);
        if(tmp>=end) return Used(data-start,false);
        ++i;
        data=tmp;
      }
      return Used(data-start,true);
    }
};
InputParser* ConditionStringPosition::createParser(){
  return new ConditionStringPositionParser(this);
}
void ConditionStringPosition::output(irr::stringc* s,irr::IWriteFile* file){
  (*s)+="7 ";(*s)+=tiestart;(*s)+=" ";(*s)+=tieend;(*s)+=" ";(*s)+=count;(*s)+=" ";
  for(int i=0;i<count;++i){
    (*s)+=poss[i].X;(*s)+=" ";(*s)+=poss[i].Y;(*s)+=" ";(*s)+=poss[i].Z;(*s)+=" ";
  }
  (*s)+="\n";
  if(file && s->size()>256){
    file->write(s->c_str(),s->size());
    *s=irr::stringc();
  }
}
ConditionStringPosition::~ConditionStringPosition(){
  delete[] poss;
}

class ConditionStringSelectionParser: public InputParser{
  ConditionStringSelection* c;
  int i;
  public:
    ConditionStringSelectionParser(ConditionStringSelection* c):c(c),i(-2){};
  
    Used parse(char* data,irr::u32 length,bool eof){
      char* start=data;
      char* end=data+length;
      if(!eof)
        end-=1;
      if(i==-1){
        c->tiestart=strtol(data,&data,10);
        if(data>=end) return Used(0,false);

        c->tieend=strtol(data,&data,10);
        if(data>=end) return Used(0,false);

        c->count=strtol(data,&data,10);
        if(data>=end) return Used(0,false);
        c->sels=new bool[c->count];
        i=0;
      }
      
      char* tmp;
      while(i<c->count){
        c->sels[i]=strtol(data,&tmp,10);
        if(tmp>=end) return Used(data-start,false);
        ++i;
        data=tmp;
      }
      return Used(data-start,true);
    }
};
InputParser* ConditionStringSelection::createParser(){
  return new ConditionStringSelectionParser(this);
}
void ConditionStringSelection::output(irr::stringc* s,irr::IWriteFile* file){
  (*s)+="7 ";(*s)+=tiestart;(*s)+=" ";(*s)+=tieend;(*s)+=" ";(*s)+=count;(*s)+=" ";
  for(int i=0;i<count;++i)
    (*s)+=sels[i]+=" ";
  (*s)+="\n";
  if(file && s->size()>256){
    file->write(s->c_str(),s->size());
    *s=irr::stringc();
  }
}
ConditionStringSelection::~ConditionStringSelection(){
  delete[] sels;
}

void Message::output(irr::stringc* s,irr::IWriteFile* file){
  *s+=count;
  *s+="\n";
  const char* const delims="\"'|^_!\\\3";
  for(int i=0;i<count;++i){
    *s+=paragraphs[i].a;
    *s+=" ";
    const char* delim=delims;
    while(paragraphs[i].b.findFirst(*delim)!=-1)
      ++delim; // if this fails then you have a string with '\3' in it!!!!!!!!
    *s+=*delim;
    *s+=paragraphs[i].b;
    *s+=*delim;
    *s+="\n";
    if(file && s->size()>256){
      file->write(s->c_str(),s->size());
      *s=irr::stringc();
    }
  }
}

class MessageParser: public InputParser{
  Message* m;
  int pos;
  irr::stringc lines;
  char delim;
  public:
    MessageParser(Message* m):m(m),pos(-1),lines(),delim(0){};
    Used parse(char* data,irr::u32 length,bool eof){
      char* start=data;
      char* end=data+length;
      if(!eof)
        end-=1;
      if(pos==-1){
        int len=strtol(data,&data,10);
        if(data>=end) return Used(0,false);
        if(m->paragraphs)
          delete[] m->paragraphs;
        m->paragraphs=new Pair<irr::stringc,irr::stringc>[len];
        m->count=len;
        pos=0;
      }
      
      char* tmp;
      while(pos<2*m->count){
        if(pos%2==0){
          while(isspace(*data))
            if(++data>=end)
              return Used(data-start,false);
          tmp=data;
          while(!isspace(*tmp))
            if(++tmp>=end)
              return Used(data-start,false);
          m->paragraphs[pos/2].a=irr::stringc(data,tmp-data);
          data=tmp;
          ++pos;
        }
        if(!delim){
          while(isspace(*data))
            if(++data>=end)
              return Used(data-start,false);
          cout<<"delim is '"<<*data<<"'"<<endl;
          delim=*data;
          if(++data>=end)
            return Used(data-start,false);
        }
        tmp=(char*)memchr(data,delim,end-data);
        if(!tmp){
          lines.append(data,end-data);
          return Used(end-start,false);
        }
        lines+=irr::stringc(data,tmp-data);
        m->paragraphs[pos/2].b=lines;
        lines=irr::stringc();
        delim=0;
        ++pos;
        data=tmp+1;
      }
      return Used(data-start,true);
    }
};

void ActionMessage::doCommon(ScriptResponse& r,String&){
  if(r.messageCount==0){
    r.messages=new Message[1];
  }else{
    Message* tmp=new Message[r.messageCount+1];
    memcpy(tmp,r.messages,r.messageCount*sizeof(Message));
    r.messages=tmp;
  }
  r.messages[r.messageCount]=m;
  ++r.messageCount;
};
InputParser* ActionMessage::createParser(){
  return new MessageParser(&m);
}
void ActionMessage::output(irr::stringc* s,irr::IWriteFile* file){
  *s+="1 ";
  m.output(s,file);
}
















InputParser* Script::createParser(Condition** condition){
  InputParser** parsers=new InputParser*[2];
  parsers[0]=new ConditionParser(parsers+1,condition);
  InputParser* parser=new SequentialInputParser<Derefer<InputParser,InputParser**> >(
      Derefer<InputParser,InputParser**>(parsers),
      Derefer<InputParser,InputParser**>(parsers+2));
}
void Script::returnParser(InputParser* parser){
  SequentialInputParser<Derefer<InputParser,InputParser**> >* p=(SequentialInputParser<Derefer<InputParser,InputParser**> >*)parser;
  delete *(p->end.data-2);
  delete[] (p->end.data-2);
  delete parser;
};


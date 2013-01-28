#include "maze.hh"
#include "string.hh"
#include "irrdisp.hh"
#include "irrio.hh"

class Script;


class Condition{
  public:
    virtual bool is(int time,Script s,PuzzleDisplay pd)=0;
    virtual InputParser* createParser()=0;
    virtual void returnParser(InputParser*)=0;
};

class Action;

class Event;

class ConditionParser:public InputParser{
  InputParser** dataParser;
  Condition** newCondition;
  public:
    virtual Used parse(char* data,irr::u32 length,bool eof);
    ConditionParser(InputParser** dataParser,Condition** newCondition):dataParser(dataParser),newCondition(newCondition){};
    virtual ~ConditionParser();
};
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

class Script{
  public:
    enum Trigger{
      SOMETHING
    };
  
  private:
    Event* event;
    int* times;
    int count;
  
  public:
    int getTime(int event){
      return times[event];
    }
};

class Event{
  Script::Trigger trigger;
  Condition* condition;
  Action* a;
};

class ConditionTrue: public Condition, public InputParser{
  public:
    virtual bool is(int time,Script s,PuzzleDisplay pd){
      return true;
    }
    virtual Used parse(char* data,irr::u32 length,bool eof){
      return Used(0,true);
    }
    virtual InputParser* createParser(){return this;};
    virtual void returnParser(InputParser*){};
};

class ConditionOr: public Condition{
  Condition** conditions;
  int count;
  public:
    virtual bool is(int time,Script s,PuzzleDisplay pd){
      for(int i=0;i<count;++i)
        if(conditions[i]->is(time,s,pd))
          return true;
      return false;
    }
    virtual InputParser* createParser(){
      InputParser** parsers=new InputParser*[2];
      parsers[0]=new ConditionListParser(parsers+1,&conditions,&count);
      return new SequentialInputParser<Derefer<InputParser,InputParser**> >(
        Derefer<InputParser,InputParser**>(parsers),
        Derefer<InputParser,InputParser**>(parsers+2));
    };
    virtual void returnParser(InputParser* parser){
      SequentialInputParser<Derefer<InputParser,InputParser**> >* p=(SequentialInputParser<Derefer<InputParser,InputParser**> >*)parser;
      delete *(p->end.data-2);
      delete[] (p->end.data-2);
      delete parser;
    };
};
class ConditionAnd: public Condition{
  Condition** conditions;
  int count;
  public:
    virtual bool is(int time,Script s,PuzzleDisplay pd){
      for(int i=0;i<count;++i)
        if(!conditions[i]->is(time,s,pd))
          return false;
      return true;
    }
    virtual InputParser* createParser(){
      InputParser** parsers=new InputParser*[2];
      parsers[0]=new ConditionListParser(parsers+1,&conditions,&count);
      return new SequentialInputParser<Derefer<InputParser,InputParser**> >(
        Derefer<InputParser,InputParser**>(parsers),
        Derefer<InputParser,InputParser**>(parsers+2));
    };
    virtual void returnParser(InputParser* parser){
      SequentialInputParser<Derefer<InputParser,InputParser**> >* p=(SequentialInputParser<Derefer<InputParser,InputParser**> >*)parser;
      delete *(p->end.data-2);
      delete[] (p->end.data-2);
      delete parser;
    };
};
class ConditionNot: public Condition{
  Condition* condition;
  public:
    virtual bool is(int time,Script s,PuzzleDisplay pd){
      return !condition->is(time,s,pd);
    }
    virtual InputParser* createParser(){
      InputParser** parsers=new InputParser*[2];
      parsers[0]=new ConditionParser(parsers+1,&condition);
      return new SequentialInputParser<Derefer<InputParser,InputParser**> >(
        Derefer<InputParser,InputParser**>(parsers),
        Derefer<InputParser,InputParser**>(parsers+2));
    };
    virtual void returnParser(InputParser* parser){
      SequentialInputParser<Derefer<InputParser,InputParser**> >* p=(SequentialInputParser<Derefer<InputParser,InputParser**> >*)parser;
      delete *(p->end.data-2);
      delete[] (p->end.data-2);
      delete parser;
    };
};

class ConditionAfter: public Condition,public InputParser{
  int event;
  int delay;
  public:
    virtual bool is(int time,Script s,PuzzleDisplay pd){
      return s.getTime(event)+delay<=time;
    }
    virtual Used parse(char* data,irr::u32 length,bool eof){
      return Used(0,true);
    }
    virtual InputParser* createParser(){return this;};
    virtual void returnParser(InputParser*){};
};
class ConditionBefore: public Condition,public InputParser{
  int event;
  public:
    virtual bool is(int time,Script s,PuzzleDisplay pd){
      return s.getTime(event)==0;
    }
    virtual Used parse(char* data,irr::u32 length,bool eof){
      return Used(0,true);
    }
    virtual InputParser* createParser(){return this;};
    virtual void returnParser(InputParser*){};
};

class ConditionStringPosition: public Condition{
  Vector* poss;
  bool tiestart;
  bool tieend;
  int count;
  
  inline bool checkPath(String& s,StringPointer sp){
    Vector* pos=poss;
    for(;pos!=poss+count;++pos,++sp)
      if(sp==s.end())
        if(!(pos && *pos==s.getEnd()))
          return false;
      else if(!(pos && *pos==sp->pos))
        return false;
    return true;
  }
  
  inline StringPointer spsub(StringPointer sp,int d){
    for(int i=0;i<d;++i)--sp;
    return sp;
  }
  
  public:
    virtual bool is(int time,Script s,PuzzleDisplay pd){
      if(count>pd.s.length()+1)
        return false;
      if(tiestart||tieend){
        if(tieend && tiestart && count!=(pd.s.length()+1))
          return false;
        if(tieend && !tiestart)
          return checkPath(pd.s,spsub(pd.s.end(),count-1));
        else
          return checkPath(pd.s,pd.s.begin());
      }else{
        StringPointer spEnd=spsub(pd.s.end(),count-2);
        for(StringPointer sp=pd.s.begin();sp!=spEnd;++sp)
          if(checkPath(pd.s,sp))
            return true;
        return false;
      }
    }
    virtual InputParser* createParser(){return NULL;};
    virtual void returnParser(InputParser*){};
};
class ConditionStringSelection: public Condition{
  bool* sels;
  bool tiestart;
  bool tieend;
  int count;
  
  inline bool checkPath(String& s,StringPointer sp){
    bool* sel=sels;
    for(;sel!=sels+count;++sel,++sp)
      if(!(sel && *sel==sp->selected))
        return false;
    return true;
   }
    
  inline StringPointer spsub(StringPointer sp,int d){
    for(int i=0;i<d;++i)--sp;
    return sp;
  }
  
  public:
    virtual bool is(int time,Script s,PuzzleDisplay pd){
      if(count>pd.s.length())
        return false;
      if(tiestart||tieend){
        if(tieend && tiestart && count!=(pd.s.length()))
          return false;
        if(tieend && !tiestart)
          return checkPath(pd.s,spsub(pd.s.end(),count));
        else
          return checkPath(pd.s,pd.s.begin());
      }else{
        StringPointer spEnd=spsub(pd.s.end(),count-1);
        for(StringPointer sp=pd.s.begin();sp!=spEnd;++sp)
          if(checkPath(pd.s,sp))
            return true;
        return false;
      }
    }
    virtual InputParser* createParser(){return NULL;};
    virtual void returnParser(InputParser*){};
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

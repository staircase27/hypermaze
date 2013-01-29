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
    virtual void output(irr::stringc* s,irr::IWriteFile* file=0)=0;
    virtual ~Condition(){};
};

class Action;

class Event;

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
    
    InputParser* createParser(Condition** c);
    void returnParser(InputParser* p);
};

class Event{
  Script::Trigger trigger;
  Condition* condition;
  Action* a;
};

class ConditionTrue: public Condition, public InputParser{
  public:
    virtual bool is(int time,Script s,PuzzleDisplay pd){return true;}
    virtual Used parse(char* data,irr::u32 length,bool eof){return Used(0,true);}
    virtual InputParser* createParser(){return this;};
    virtual void returnParser(InputParser*){};
    virtual void output(irr::stringc* s,irr::IWriteFile* file=0){(*s)+="1\n";};
};

class ConditionOr: public Condition{
  Condition** conditions;
  int count;
  public:
    virtual bool is(int time,Script s,PuzzleDisplay pd);
    virtual InputParser* createParser();
    virtual void returnParser(InputParser* parser);
    virtual void output(irr::stringc* s,irr::IWriteFile* file=0);
    virtual ~ConditionOr();
};
class ConditionAnd: public Condition{
  Condition** conditions;
  int count;
  public:
    virtual bool is(int time,Script s,PuzzleDisplay pd);
    virtual InputParser* createParser();
    virtual void returnParser(InputParser* parser);
    virtual void output(irr::stringc* s,irr::IWriteFile* file=0);
    virtual ~ConditionAnd();
};
class ConditionNot: public Condition{
  Condition* condition;
  public:
    virtual bool is(int time,Script s,PuzzleDisplay pd){
      return !condition->is(time,s,pd);
    }
    virtual InputParser* createParser();
    virtual void returnParser(InputParser* parser);
    virtual void output(irr::stringc* s,irr::IWriteFile* file=0);
    virtual ~ConditionNot(){delete condition;}
};

class ConditionAfter: public Condition,private InputParser{
  int event;
  int delay;
  public:
    virtual bool is(int time,Script s,PuzzleDisplay pd);
    virtual Used parse(char* data,irr::u32 length,bool eof);
    virtual InputParser* createParser(){return this;};
    virtual void returnParser(InputParser*){};
    virtual void output(irr::stringc* s,irr::IWriteFile* file=0);
};
class ConditionBefore: public Condition,private InputParser{
  int event;
  public:
    virtual bool is(int time,Script s,PuzzleDisplay pd);
    virtual Used parse(char* data,irr::u32 length,bool eof);
    inline virtual InputParser* createParser(){return this;};
    inline virtual void returnParser(InputParser*){};
    virtual void output(irr::stringc* s,irr::IWriteFile* file=0);
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
    virtual InputParser* createParser();
    virtual void returnParser(InputParser* parser){delete parser;};
    friend class ConditionStringPositionParser;
    virtual void output(irr::stringc* s,irr::IWriteFile* file=0);
    virtual ~ConditionStringPosition();
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
    virtual InputParser* createParser();
    virtual void returnParser(InputParser* parser){delete parser;};
    friend class ConditionStringSelectionParser;
    virtual void output(irr::stringc* s,irr::IWriteFile* file=0);
    virtual ~ConditionStringSelection();
};


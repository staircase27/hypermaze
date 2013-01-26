#include "maze.hh"
#include "string.hh"
#include "irrdisp.hh"

class Script;

class Condition{
  public:
    virtual bool is(int time,Script s,PuzzleDisplay pd);
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
};

class Event{
  Script::Trigger trigger;
  Condition* condition;
  Action* a;
};


class ConditionTrue: public Condition{
  public:
    virtual bool is(int time,Script s,PuzzleDisplay pd){
      return true;
    }
};
  

class ConditionOr: public Condition{
  Condition* conditions;
  int count;
  public:
    virtual bool is(int time,Script s,PuzzleDisplay pd){
      for(int i=0;i<count;++i)
        if(conditions[i].is(time,s,pd))
          return true;
      return false;
    }
};
class ConditionAnd: public Condition{
  Condition* conditions;
  int count;
  public:
    virtual bool is(int time,Script s,PuzzleDisplay pd){
      for(int i=0;i<count;++i)
        if(!conditions[i].is(time,s,pd))
          return false;
      return true;
    }
};
class ConditionNot: public Condition{
  Condition* condition;
  public:
    virtual bool is(int time,Script s,PuzzleDisplay pd){
      return !condition->is(time,s,pd);
    }
};
  

class ConditionAfter: public Condition{
  int event;
  int delay;
  public:
    virtual bool is(int time,Script s,PuzzleDisplay pd){
      return s.getTime(event)+delay<=time;
    }
};
class ConditionBefor: public Condition{
  int event;
  public:
    virtual bool is(int time,Script s,PuzzleDisplay pd){
      return s.getTime(event)==0;
    }
};

class ConditionStringPosition{
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
};
class ConditionStringSelection{
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
};
      


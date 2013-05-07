#include "script.hh"

#ifndef SCRIPTIMPL_HH_INC
#define SCRIPTIMPL_HH_INC

struct Range{
  int start;
  int end;
  inline bool inRange(int val){
    return ((start==INT_MAX)||(val>=start))&&((end==INT_MAX)||(val<=end));
  }
  Range():start(0),end(-1){};
};

struct StringElementCondition{
  // first bit is if we care (1 care, 0 dont). second is the value to match if we do care
  int selectionCondition;

  int xrange_count;
  int yrange_count;
  int zrange_count;
  Range* xrange;
  Range* yrange;
  Range* zrange;
  template <class T>
  inline bool matches(T el){
    if((selectionCondition&1)==0||(((selectionCondition&2)==2)==el->selected)){
      bool match=false;
      for(int i=0;i<xrange_count&&!match;++i)
        if(xrange[i].inRange(el->pos.X))
          match=true;
      if(!match)
        return false;
      match=false;
      for(int i=0;i<yrange_count&&!match;++i)
        if(yrange[i].inRange(el->pos.Y))
          match=true;
      if(!match)
        return false;
      match=false;
      for(int i=0;i<zrange_count&&!match;++i)
        if(zrange[i].inRange(el->pos.Z))
          match=true;
      return match;
    }
    return false;
  }
  StringElementCondition():selectionCondition(0),xrange_count(0),yrange_count(0),zrange_count(0),xrange(0),yrange(0),zrange(0){};
  
	void output(irr::stringc* s,irr::IWriteFile* file=0);
};

class StringConditionParser:public InputParser{
  int stage;
  StringElementCondition* c;
  public:
    Used parse(char* data,irr::u32 length,bool eof);
    StringConditionParser(StringElementCondition *c):c(c),stage(0){}
};

class StringMatcher{
  //returns if the pattern matches the string and if it has a range marker the start and end of the last range found
  //pattern takes the form of a list of string specs each of which have a repeat range (min & max repeats) and a spec for the elements in that range.
};


class ConditionTrue: public Condition, public InputParser{
  public:
    virtual bool is(int time,Script script,const String& s){return true;}
    virtual Used parse(char* data,irr::u32 length,bool eof){return Used(0,true);}
    virtual InputParser* createParser(){return this;};
    virtual void returnParser(InputParser*){};
    virtual void output(irr::stringc* s,irr::IWriteFile* file=0){(*s)+="1\n";};
};

class ConditionOr: public Condition{
  Condition** conditions;
  int count;
  public:
    virtual bool is(int time,Script script,const String& s);
    virtual InputParser* createParser();
    virtual void returnParser(InputParser* parser);
    virtual void output(irr::stringc* s,irr::IWriteFile* file=0);
    virtual ~ConditionOr();
};
class ConditionAnd: public Condition{
  Condition** conditions;
  int count;
  public:
    virtual bool is(int time,Script script,const String& s);
    virtual InputParser* createParser();
    virtual void returnParser(InputParser* parser);
    virtual void output(irr::stringc* s,irr::IWriteFile* file=0);
    virtual ~ConditionAnd();
};
class ConditionNot: public Condition{
  Condition* condition;
  public:
    virtual bool is(int time,Script script,const String& s){
      return !condition->is(time,script,s);
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
    virtual bool is(int time,Script script,const String& s);
    virtual Used parse(char* data,irr::u32 length,bool eof);
    virtual InputParser* createParser(){return this;};
    virtual void returnParser(InputParser*){};
    virtual void output(irr::stringc* s,irr::IWriteFile* file=0);
};
class ConditionBefore: public Condition,private InputParser{
  int event;
  public:
    virtual bool is(int time,Script script,const String& s);
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
  
  inline bool checkPath(const String& s,ConstStringPointer sp){
    Vector* pos=poss;
    for(;pos!=poss+count;++pos,++sp)
      if(sp==s.end())
        if(!(pos && *pos==s.getEnd()))
          return false;
      else if(!(pos && *pos==sp->pos))
        return false;
    return true;
  }
  
  inline ConstStringPointer spsub(ConstStringPointer sp,int d){
    for(int i=0;i<d;++i)--sp;
    return sp;
  }
  
  public:
    virtual bool is(int time,Script script,const String& s){
      if(count>s.length()+1)
        return false;
      if(tiestart||tieend){
        if(tieend && tiestart && count!=(s.length()+1))
          return false;
        if(tieend && !tiestart)
          return checkPath(s,spsub(s.end(),count-1));
        else
          return checkPath(s,s.begin());
      }else{
        ConstStringPointer spEnd=spsub(s.end(),count-2);
        for(ConstStringPointer sp=s.begin();sp!=spEnd;++sp)
          if(checkPath(s,sp))
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
  
  inline bool checkPath(const String& s,ConstStringPointer sp){
    bool* sel=sels;
    for(;sel!=sels+count;++sel,++sp)
      if(!(sel && *sel==sp->selected))
        return false;
    return true;
   }
    
  inline ConstStringPointer spsub(ConstStringPointer sp,int d){
    for(int i=0;i<d;++i)--sp;
    return sp;
  }
  
  public:
    virtual bool is(int time,Script script,const String& s){
      if(count>s.length())
        return false;
      if(tiestart||tieend){
        if(tieend && tiestart && count!=(s.length()))
          return false;
        if(tieend && !tiestart)
          return checkPath(s,spsub(s.end(),count));
        else
          return checkPath(s,s.begin());
      }else{
        ConstStringPointer spEnd=spsub(s.end(),count-1);
        for(ConstStringPointer sp=s.begin();sp!=spEnd;++sp)
          if(checkPath(s,sp))
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


class ActionNothing:public ActionCommon,private InputParser{
  public:
    virtual void doCommon(ScriptResponse&,String&){};
    virtual Used parse(char* data,irr::u32 length,bool eof){return Used(0,true);}
    virtual InputParser* createParser(){return this;};
    virtual void returnParser(InputParser*){};
    virtual void output(irr::stringc* s,irr::IWriteFile* file=0){(*s)+="0\n";};
    virtual ~ActionNothing(){};
};

class ActionMessage:public ActionCommon{
  public:
    Message m;
    virtual void doCommon(ScriptResponse& r,String&);
      
    virtual InputParser* createParser();
    virtual void returnParser(InputParser* parser){delete parser;};
    virtual void output(irr::stringc* s,irr::IWriteFile* file=0);
    virtual ~ActionMessage(){};
};
class ActionBlockWin:public ActionWin,private InputParser{
  public:
    virtual void doWin(ScriptResponseWin& r,String&){
      r.block=true;
    };
    virtual Used parse(char* data,irr::u32 length,bool eof){return Used(0,true);}
    virtual InputParser* createParser(){return this;};
    virtual void returnParser(InputParser*){};
    virtual void output(irr::stringc* s,irr::IWriteFile* file=0){(*s)+="2\n";};
    virtual ~ActionBlockWin(){};
};
class ActionWinMessage:public ActionWin{
  public:
    Message m;
    virtual void doWin(ScriptResponseWin& r,String&){
      r.winMessage=m;
    };
    virtual InputParser* createParser();
    virtual void returnParser(InputParser* parser){delete parser;};
    virtual void output(irr::stringc* s,irr::IWriteFile* file=0);
    virtual ~ActionWinMessage(){};
};
class ActionWinNextLevel:public ActionWin{
  Pair<irr::stringc,irr::stringc> nextLevel;
  public:
    virtual void doWin(ScriptResponseWin& r,String&){
      r.nextLevel=nextLevel;
    };
    virtual InputParser* createParser();
    virtual void returnParser(InputParser* parser){delete parser;};
    virtual void output(irr::stringc* s,irr::IWriteFile* file=0);
    virtual ~ActionWinNextLevel(){};
};

class ActionForceWin:public Action,private InputParser{
  public:
		virtual void doStart(ScriptResponseStart&,String&){};
		virtual void doWin(ScriptResponseWin&,String&){};
		virtual void doMove(ScriptResponseMove& r,String&){r.forceWin=true;};
		virtual void doSelect(ScriptResponseSelect& r,String&){r.forceWin=true;};
		
    virtual Used parse(char* data,irr::u32 length,bool eof){return Used(0,true);}
    virtual InputParser* createParser(){return this;};
    virtual void returnParser(InputParser*){};
    virtual void output(irr::stringc* s,irr::IWriteFile* file=0){(*s)+="5\n";};
    virtual ~ActionForceWin(){};
};

/*
class ActionSelectString:public ActionCommon{
  bool fromStart;
  int length;
  bool* change;
  bool* val;
  
  public:
    virtual void doCommon(ScriptResponse& r,String&);
  
}
//*/

class SomeAction{
  virtual void doStart(ScriptResponseStart&,String&)=0;
  virtual void doWin(ScriptResponseWin&,String&)=0;
  virtual void doMove(ScriptResponseMove&,String&)=0;
  virtual void doSelect(ScriptResponseSelect&,String&)=0;
  virtual void doCommon(ScriptResponse&,String&)=0;
};
#endif

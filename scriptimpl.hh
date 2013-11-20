#include "script.hh"

#ifndef SCRIPTIMPL_HH_INC
#define SCRIPTIMPL_HH_INC

class ConditionTrue;

template <class B,class T>
struct TypeToID{
  public:
    static const int ID=0;
};
template <class B,int i>
struct IDToType{
};
template <int i>
struct IDToType<Condition,i>{
  public:
    typedef ConditionTrue type;
};

#define ADD_TO_TYPE_MAP(BASE,ID,TYPE) template<> class TypeToID<BASE,TYPE>{public: static const int id=ID;};template<> class IDToType<BASE,ID>{public: typedef TYPE type;};

template <class B,class T>
class PolymorphicHypIOImpl{
  protected:
    IOResult doread(HypIStream& s){
      return read(s,*((T*)this));
    }
    bool dowrite(HypOStream& s){
      if(!write(s,TypeToID<B,T>::id,0))
      return write(s,*((T*)this));
    }
};

struct Range{
  int start;
  int end;
  inline bool inRange(int val){
    return ((start==INT_MAX)||(val>=start))&&((end==INT_MAX)||(val<=end));
  }
  Range():start(0),end(INT_MAX){};
};

struct StringElementCondition{
  // first bit is if we care (1 care, 0 dont). second is the value to match if we do care
  int selectionCondition;
  int dirnsCondition;

  int xrange_count;
  int yrange_count;
  int zrange_count;
  Range* xrange;
  Range* yrange;
  Range* zrange;
  template <class T>
  inline bool matches(T el){
    cout<<"try match "<<el<<" "<<(dirnsCondition&to_mask(el->d)!=0)<<endl;
    if((selectionCondition&1)==0||(((selectionCondition&2)==2)==el->selected)&&(dirnsCondition&to_mask(el->d)!=0)){
      bool match=xrange_count==0;
      for(int i=0;i<xrange_count&&!match;++i)
        if(xrange[i].inRange(el->pos.X))
          match=true;
      if(!match)
        return false;
      match=yrange_count==0;
      for(int i=0;i<yrange_count&&!match;++i)
        if(yrange[i].inRange(el->pos.Y))
          match=true;
      if(!match)
        return false;
      match=zrange_count==0;
      for(int i=0;i<zrange_count&&!match;++i)
        if(zrange[i].inRange(el->pos.Z))
          match=true;
      return match;
    }
    return false;
  }
  StringElementCondition():selectionCondition(0),dirnsCondition(ALLDIRNSMASK),xrange_count(0),yrange_count(0),zrange_count(0),xrange(0),yrange(0),zrange(0){};
  
  ~StringElementCondition(){
    delete[] xrange;
    delete[] yrange;
    delete[] zrange;
  }
  
	void output(irr::stringc* s,irr::IWriteFile* file=0);
};

class StringConditionParser:public InputParser{
  int stage;
  StringElementCondition* c;
  public:
    Used parse(char* data,irr::u32 length,bool eof);
    StringConditionParser(StringElementCondition *c):c(c),stage(0){}
};

class StringMatcher;

struct PatternTag:private InputParser{
  int min;
  int max;
  bool greedy;
  PatternTag():min(1),max(1),greedy(true){};
  private:
	  Used parse(char* data,irr::u32 length,bool eof);
	  virtual void output(irr::stringc* s,irr::IWriteFile* file=0);
	  friend class StringMatcher;
	  friend class StringMatcherParser;
};

template <class POINTER>
struct PatternMatch{
  SP<POINTER> start;
  SP<POINTER> end;
  int length;
  PatternMatch():start(0),end(0),length(0){};
};

template <class POINTER>
class StringMatcherCallback{
  public:
    virtual bool process(pair<SP<POINTER>,SP<POINTER> >* groups)=0;
};

class StringMatcher{
public:
  int count;
  pair<PatternTag,StringElementCondition>* pattern;
  int group_count;
  pair<int,int>* groups;
  public:
  
	  StringMatcher():count(0),group_count(0),pattern(0),groups(0){};
    virtual void output(irr::stringc* s,irr::IWriteFile* file=0);
    virtual ~StringMatcher(){
      delete[] pattern;
      delete[] groups;
    }
    virtual InputParser* createParser();
    virtual void returnParser(InputParser*);
    inline int groupCount(){return group_count;}
	  bool match(const String& s,pair<SP<ConstStringPointer>,SP<ConstStringPointer> >* groups=0);
	  bool match(String& s,pair<SP<StringPointer>,SP<StringPointer> >* groups=0);
	  bool match(String& s,StringMatcherCallback<StringPointer>* cb,pair<SP<StringPointer>,SP<StringPointer> >* groups=0);
	  
	private:
	  template <class STRING,class POINTER>
	  bool match(STRING& s,pair<SP<POINTER>,SP<POINTER> >* groups,StringMatcherCallback<POINTER>* cb);
	  template <class STRING,class POINTER>
	  bool matchStep(STRING& s,POINTER p,PatternMatch<POINTER>* matches,int level,pair<SP<POINTER>,SP<POINTER> >* groups,StringMatcherCallback<POINTER>* cb);
};


class ConditionTrue: public Condition, public InputParser{
  public:
    virtual bool is(int time,const Script& script,const String& s){return true;}
    virtual Used parse(char* data,irr::u32 length,bool eof){return Used(0,true);}
    virtual InputParser* createParser(){return this;};
    virtual void returnParser(InputParser*){};
    virtual void output(irr::stringc* s,irr::IWriteFile* file=0){(*s)+="1\n";};
};

class ConditionOr: public Condition{
  Condition** conditions;
  int count;
  public:
    virtual bool is(int time,const Script& script,const String& s);
    virtual InputParser* createParser();
    virtual void returnParser(InputParser* parser);
    virtual void output(irr::stringc* s,irr::IWriteFile* file=0);
    virtual ~ConditionOr();
};
class ConditionAnd: public Condition{
  Condition** conditions;
  int count;
  public:
    virtual bool is(int time,const Script& script,const String& s);
    virtual InputParser* createParser();
    virtual void returnParser(InputParser* parser);
    virtual void output(irr::stringc* s,irr::IWriteFile* file=0);
    virtual ~ConditionAnd();
};
class ConditionNot: public Condition{
  Condition* condition;
  public:
    virtual bool is(int time,const Script& script,const String& s){
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
    virtual bool is(int time,const Script& script,const String& s);
    virtual Used parse(char* data,irr::u32 length,bool eof);
    virtual InputParser* createParser(){return this;};
    virtual void returnParser(InputParser*){};
    virtual void output(irr::stringc* s,irr::IWriteFile* file=0);
};
class ConditionBefore: public Condition,private InputParser{
  int event;
  public:
    virtual bool is(int time,const Script& script,const String& s);
    virtual Used parse(char* data,irr::u32 length,bool eof);
    inline virtual InputParser* createParser(){return this;};
    inline virtual void returnParser(InputParser*){};
    virtual void output(irr::stringc* s,irr::IWriteFile* file=0);
};

class ConditionStringPattern: public Condition{
  StringMatcher sm;
  
  public:
    virtual bool is(int time,const Script& script,const String& s){
      return sm.match(s);
    }
    virtual InputParser* createParser(){return sm.createParser();}
    virtual void returnParser(InputParser* parser){sm.returnParser(parser);};
    virtual void output(irr::stringc* s,irr::IWriteFile* file=0){
      (*s)+="7 ";
      sm.output(s,file);
    }
    virtual ~ConditionStringPattern(){};
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
      cout<<"Win Blocked"<<endl;
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

class ActionSelectStringPattern:public ActionCommon{
  StringElementCondition change;
  StringElementCondition select;
  
  public:
    virtual void doCommon(ScriptResponse& r,String&);
    virtual InputParser* createParser();
    virtual void returnParser(InputParser* p);
    virtual void output(irr::stringc* s,irr::IWriteFile* file=0){(*s)+="6 ";change.output(s,file);select.output(s,file);};
};

class ActionSetStringRoute:public ActionCommon,private InputParser,private StringMatcherCallback<StringPointer>{
  StringMatcher ranges;
  int count;
  Dirn* route;
  bool all;
  
  StringEdit* se;
  
  virtual bool process(pair<SP<StringPointer>,SP<StringPointer> >* groups);
  public:
    virtual void doCommon(ScriptResponse& r,String&);
    virtual InputParser* createParser();
    virtual Used parse(char* data,irr::u32 length,bool eof);
    virtual void returnParser(InputParser* p);
    virtual void output(irr::stringc* s,irr::IWriteFile* file=0);
};


class SomeAction{
  virtual void doStart(ScriptResponseStart&,String&)=0;
  virtual void doWin(ScriptResponseWin&,String&)=0;
  virtual void doMove(ScriptResponseMove&,String&)=0;
  virtual void doSelect(ScriptResponseSelect&,String&)=0;
  virtual void doCommon(ScriptResponse&,String&)=0;
};
#endif

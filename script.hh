#include "maze.hh"
#include "string.hh"
#include "irrio.hh"

#ifndef SCRIPT_HH_INC
#define SCRIPT_HH_INC

template <class T>
IOResult read(HypIStream& s,SPA<SP<T>>& a, int& c);
template <class T>
bool write(HypOStream& s,const SPA<const SP<const T>>& a, const int& c);

enum Trigger{
  TRIGGER_START=1,
  TRIGGER_WIN=2,
  TRIGGER_MOVE=4,
  TRIGGER_SELECT=8
};

class Script;

class Condition{
  public:
    virtual bool is(int time,const Script& script,const String& s)=0;
    virtual InputParser* createParser()=0;
    virtual void returnParser(InputParser*)=0;
    virtual void output(irr::stringc* s,irr::IWriteFile* file=0)=0;
    virtual ~Condition(){};

    public static const SP<Condition> defaultvalue;
};

IOResult read(HypIStream& s,SP<Condition>& c);
bool write(HypOStream& s,const SP<const Condition>& c);

template <class T1,class T2>
struct Pair{
  T1 a;
  T2 b;
  Pair(T1 a,T2 b):a(a),b(b){};
  Pair():a(),b(){};
};

struct Message{
  int count;
  SPA<Pair<irr::stringc,irr::stringc> > paragraphs;
  Message():paragraphs(0),count(0){};
  void output(irr::stringc* s,irr::IWriteFile* file=0);
};

struct ScriptResponse{
  bool stringChanged;
  bool stringSelectionChanged;
  int messageCount;
  Message* messages;
  ScriptResponse():stringChanged(false),stringSelectionChanged(false),messageCount(0),messages(0){};
};
struct ScriptResponseStart:public ScriptResponse{};
struct ScriptResponseWin:public ScriptResponse{
  bool block;
  Message winMessage;
  Pair<irr::stringc,irr::stringc> nextLevel;
  ScriptResponseWin():block(false),winMessage(),nextLevel("",""){}
};
struct ScriptResponseMove:public ScriptResponse{
  bool forceWin;
  ScriptResponseMove():forceWin(false){};
};
struct ScriptResponseSelect:public ScriptResponse{
  bool forceWin;
  ScriptResponseSelect():forceWin(false){};
};

class Action{
  public:
		virtual void doStart(ScriptResponseStart&,String&)=0;
		virtual void doWin(ScriptResponseWin&,String&)=0;
		virtual void doMove(ScriptResponseMove&,String&)=0;
		virtual void doSelect(ScriptResponseSelect&,String&)=0;
		
		virtual InputParser* createParser()=0;
		virtual void returnParser(InputParser*)=0;
		virtual void output(irr::stringc* s,irr::IWriteFile* file=0)=0;
		virtual ~Action(){};
};
class ActionStart: public virtual Action{
  public:
		virtual void doStart(ScriptResponseStart&,String&)=0;
		virtual void doWin(ScriptResponseWin&,String& s){};
		virtual void doMove(ScriptResponseMove&,String&){};
		virtual void doSelect(ScriptResponseSelect&,String&){};
};
class ActionWin: public virtual Action{
  public:
		virtual void doStart(ScriptResponseStart&,String&){};
		virtual void doWin(ScriptResponseWin&,String&)=0;
		virtual void doMove(ScriptResponseMove&,String&){};
		virtual void doSelect(ScriptResponseSelect&,String&){};
};
class ActionMove: public virtual Action{
  public:
		virtual void doStart(ScriptResponseStart&,String&){};
		virtual void doWin(ScriptResponseWin&,String&){};
		virtual void doMove(ScriptResponseMove&,String&)=0;
		virtual void doSelect(ScriptResponseSelect&,String&){};
};
class ActionSelect: public virtual Action{
  public:
		virtual void doStart(ScriptResponseStart&,String&){};
		virtual void doWin(ScriptResponseWin&,String&){};
		virtual void doMove(ScriptResponseMove&,String&){};
		virtual void doSelect(ScriptResponseSelect&,String&)=0;
};
class ActionCommon: public virtual Action{
  public:
		virtual void doCommon(ScriptResponse&,String&)=0;
		virtual void doStart(ScriptResponseStart& r,String& s){doCommon(r,s);};
		virtual void doWin(ScriptResponseWin& r,String& s){doCommon(r,s);};
		virtual void doMove(ScriptResponseMove& r,String& s){doCommon(r,s);};
		virtual void doSelect(ScriptResponseSelect& r,String& s){doCommon(r,s);};
};

class Event{
  public:
		int trigger;
		int conditionCount;
		Condition* condition;
		int actionCount;
		Action* actions;
};

class Script{
  private:
    Event* event;
    int* times;
    int count;
  
  public:
    int getTime(int event) const{
      return times[event];
    }
    
    InputParser* createParser(Condition** c);
    InputParser* createParser(Action*** a,int* count);
    void returnParser(InputParser* p);
};
#endif

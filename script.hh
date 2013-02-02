#include "maze.hh"
#include "string.hh"
#include "irrio.hh"

#ifndef SCRIPT_HH_INC
#define SCRIPT_HH_INC
enum Trigger{
  TRIGGER_START,
  TRIGGER_WIN,
  TRIGGER_MOVE,
  TRIGGER_SELECT
};

class Script;

class Condition{
  public:
    virtual bool is(int time,Script script,const String& s)=0;
    virtual InputParser* createParser()=0;
    virtual void returnParser(InputParser*)=0;
    virtual void output(irr::stringc* s,irr::IWriteFile* file=0)=0;
    virtual ~Condition(){};
};

template <class T1,class T2>
class Pair{
  T1 a;
  T2 b;
  Pair(T1 a,T2 b):a(a),b(b){};
};

class Message{
  int count;
  Pair<irr::stringc,irr::stringc>* paragraphs;
};

struct ScriptResponse{
  bool stringChanged;
  bool stringSelectionChanged;
  int messageCount;
  Message* messages;
};
struct ScriptResponseStart:public ScriptResponse{};
struct ScriptResponseWin:public ScriptResponse{
  bool block;
  Message winMessage;
  Pair<irr::stringc,irr::stringc> nextLevel;
};
struct ScriptResponseMove:public ScriptResponse{
  bool forceWin;
};
struct ScriptResponseSelect:public ScriptResponse{
  bool forceWin;
};

class Action{
  virtual void doStart(ScriptResponseStart&,String&)=0;
  virtual void doWin(ScriptResponseWin&,String&)=0;
  virtual void doMove(ScriptResponseMove&,String&)=0;
  virtual void doSelect(ScriptResponseSelect&,String&)=0;
};
class ActionStart: public Action{
  virtual void doStart(ScriptResponseStart&,String&)=0;
  virtual void doWin(ScriptResponseWin&,String& s){};
  virtual void doMove(ScriptResponseMove&,String&){};
  virtual void doSelect(ScriptResponseSelect&,String&){};
};
class ActionWin: public Action{
  virtual void doStart(ScriptResponseStart&,String&){};
  virtual void doWin(ScriptResponseWin&,String&)=0;
  virtual void doMove(ScriptResponseMove&,String&){};
  virtual void doSelect(ScriptResponseSelect&,String&){};
};
class ActionMove: public Action{
  virtual void doStart(ScriptResponseStart&,String&){};
  virtual void doWin(ScriptResponseWin&,String&){};
  virtual void doMove(ScriptResponseMove&,String&)=0;
  virtual void doSelect(ScriptResponseSelect&,String&){};
};
class ActionSelect: public Action{
  virtual void doStart(ScriptResponseStart&,String&){};
  virtual void doWin(ScriptResponseWin&,String&)=0;
  virtual void doMove(ScriptResponseMove&,String&){};
  virtual void doSelect(ScriptResponseSelect&,String&){};
};
class ActionCommon: public Action{
  virtual void doCommon(ScriptResponse&,String&)=0;
  virtual void doStart(ScriptResponseStart& r,String& s){doCommon(r,s);};
  virtual void doWin(ScriptResponseWin& r,String& s){doCommon(r,s);};
  virtual void doMove(ScriptResponseMove& r,String& s){doCommon(r,s);};
  virtual void doSelect(ScriptResponseSelect& r,String& s){doCommon(r,s);};
};

class Event{
  Trigger trigger;
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
    int getTime(int event){
      return times[event];
    }
    
    InputParser* createParser(Condition** c);
    void returnParser(InputParser* p);
};
#endif

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



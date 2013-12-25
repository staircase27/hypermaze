#include "maze.hh"
#include "string.hh"
#include "irrio.hh"

#ifndef SCRIPT_HH_INC
#define SCRIPT_HH_INC

///holds a pair of values of possibly different types
/**
 * @tparam T1 the type of the first member
 * @tparam T2 optional type of the second member. defaults to same as first.
 */
template <class T1,class T2=T1>
struct Pair{
  T1 a;///<The first member
  T2 b;///<The second member
  ///constructor for a pair from the two items
  /**
   * @param a the first value
   * @param b the second value
   */
  Pair(T1 a,T2 b):a(a),b(b){};
  ///default constructor that initialises both members with their default constructors
  Pair():a(),b(){};
};

///read an array of pointers to items from a stream
/**
 * the type T must define a const static member variable 
 * SP<T> T::defaultValue that is used to fill in values in the array that can't be read.
 * when this function returns the array will have at least length c and will contain fully
 * valid objects
 * @tparam the type of the elements in the array of pointers to read
 * @param s the stream to read from
 * @param a the array variable to read into
 * @param c an int variable to store the length of the array into
 * @return an IOResult object that contains the status of the read
 */
template <class T>
IOResult read(HypIStream& s,SPA<SP<T> >& a, int& c);
///write an array of pointers to items to a stream
/**
 * the array must have at least length c and must contain fully valid objects
 * @tparam the type of the elements in the array of pointers to write
 * @param s the stream to write to
 * @param a the array to write
 * @param c the length of the array to write
 * @return true if the array was written ok
 */
template <class T>
bool write(HypOStream& s,const SPA<const SP<const T> >& a, const int& c);

///Helper class to allow classes to be written to a stream using a pointer to a base type
/**
 * @see PolymorphicHypIOImpl
 */
class PolymorphicHypIO{
  protected:
    ///Method to implement to allow polymorphic writing
    /**
     * @param s the stream to write this object to
     * @return true if the array was written ok
     */
    virtual bool dowrite(HypOStream& s)=0;
};

///the different maze events that can trigger scripts  
enum Trigger{
  TRIGGER_START=1,///<trigger when the maze is loaded
  TRIGGER_WIN=2,///<trigger when they have won the maze
  TRIGGER_MOVE=4,///<trigger when the string is moved
  TRIGGER_SELECT=8///<trigger when the string selection is changed
};

class Script;

///A Condition to select if an event should trigger or not
class Condition: protected PolymorphicHypIO{
  public:
    ///Check if the condition is matched
    /**
     * @param time the current time in seconds
     * @param script the script object for the currently running script
     * @param s the current string object
     * @return true if the condition is matched and the event should trigger
     */
    virtual bool is(int time,const Script& script,const String& s)=0;
    ///virtual destructor to allow overiding in subclasses
    virtual ~Condition(){};
  public:
    ///the default condition to use when creating lists of conditions
    static const SP<Condition> defaultvalue;
};

///Read a Condition (as a pointer) from a stream
/**
 * fills the pointer with a new condition object. The actual instance type
 * is read from the stream.
 * @param s the stream to read from
 * @param c pointer to condition variable to stick the read condition in
 * @return an IOResult object that contains the status of the read
 */
IOResult read(HypIStream& s,SP<Condition>& c);
///write a Condition (from a pointer) to a stream
/**
 * @param s the stream to write to
 * @param c the Condition to write
 * @return true if i was written ok
 */
bool write(HypOStream& s,const SP<const Condition>& c);

///A message to display in the gui
struct Message{
  int count;///<the number of paragaphs
  ///list of the paragraphs that make up the message
  /**
   * first member of the pair is a style string. second member is the text for the paragraph
   */
  SPA<Pair<char*> > paragraphs;
  ///default constructor that initialises this as having no paragraphs
  Message():paragraphs(0),count(0){};
};

struct ScriptResponse{
  bool stringChanged;
  bool stringSelectionChanged;
  int messageCount;
  SPA<Message> messages;
  ScriptResponse():stringChanged(false),stringSelectionChanged(false),messageCount(0),messages(0){};
};
struct ScriptResponseStart:public ScriptResponse{};
struct ScriptResponseWin:public ScriptResponse{
  bool block;
  Message winMessage;
  Pair<irr::stringc> nextLevel;
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

class Action: protected PolymorphicHypIO{
  public:
		virtual void doStart(ScriptResponseStart&,String&)=0;
		virtual void doWin(ScriptResponseWin&,String&)=0;
		virtual void doMove(ScriptResponseMove&,String&)=0;
		virtual void doSelect(ScriptResponseSelect&,String&)=0;
		
		virtual ~Action(){};
};

IOResult read(HypIStream& s,SP<Action>& c);
bool write(HypOStream& s,const SP<const Action>& c);

class Event{
  public:
		int trigger;
		int conditionCount;
		SPA<SP<Condition>> condition;
		int actionCount;
		SPA<SP<Action>> actions;
};

class Script{
  private:
    int eventcount
    SPA<Event> events;
    int* times;
  
  public:
    int getTime(int event) const{
      return times[event];
    }
};
#endif

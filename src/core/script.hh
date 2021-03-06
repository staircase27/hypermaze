/**
 * @file script.hh
 * @brief The script classes.
 * This doesn't include the classes that implement most of the logic. These are
 * defined in scriptimpl.hh
 */
#include "maze.hh"
#include "string.hh"
#include "SmartPointer.hh"

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
     * @return true if the item was written ok
     */
    virtual bool dowrite(HypOStream& s) const=0;
  public:
    ///Get the id of this class
    /**
     * @return the id of this class
     */
    virtual int getid() const=0;
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
class Condition: public virtual PolymorphicHypIO{
  public:
    ///Check if the condition is matched
    /**
     * @param time the current time in seconds
     * @param script the script object for the currently running script
     * @param s the current string object
     * @return true if the condition is matched and the event should trigger
     */
    virtual bool is(int time,const Script& script,SP<const String> s)=0;
    ///virtual destructor to allow overiding in subclasses
    virtual ~Condition(){};
  public:
    ///the default condition to use when creating lists of conditions
    static const SP<Condition> defaultvalue;
    friend bool write(HypOStream& s,const SP<const Condition>& c);
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
  SPA<Pair<SPA<const char> > > paragraphs;
  ///default constructor that initialises this as having no paragraphs
  Message():paragraphs(),count(0){};
};

///Read a Message from a stream
/**
 * fills the pointer with a new message object
 * @param s the stream to read from
 * @param m pointer to Message variable to stick the read Message in
 * @return an IOResult object that contains the status of the read
 */
IOResult read(HypIStream& s,Message& m);
///write a Message to a stream
/**
 * @param s the stream to write to
 * @param m the message to write
 * @return true if i was written ok
 */
bool write(HypOStream& s,const Message& m);

///common part of the response from a script
struct ScriptResponse{
  bool stringChanged;///<has the string been changed by the script
  bool stringSelectionChanged;///<has the selection been changed by the script
  int messageCount;///<the number of messages that need to be shown
  SPA<Message> messages;///<the messages that need to be shown
  ///default constructor to setup the response as no actions taken or to take
  ScriptResponse():stringChanged(false),stringSelectionChanged(false),messageCount(0),messages(0){};
};
///response from a script for a start event
/**
 * no more script responses available
 */
struct ScriptResponseStart:public ScriptResponse{};
///response from a script for a win event
struct ScriptResponseWin:public ScriptResponse{
  bool block;///<block the winning
  Message winMessage;///<message to show on the win screen
  Pair<SPA<const char> > nextLevel;///<next level to offer on the win screen
  ///default constructor to setup the response as use defaults
  ScriptResponseWin():block(false),winMessage(),nextLevel(){}
};
///response from a script for a move event
struct ScriptResponseMove:public ScriptResponse{
  bool forceWin;///<force a win now
  ///default constructor that doesn't set a win forced
  ScriptResponseMove():forceWin(false){};
};
///response from a script for a select event
struct ScriptResponseSelect:public ScriptResponse{
  bool forceWin;///<force a win now
  ///default constructor that doesn't set a win forced
  ScriptResponseSelect():forceWin(false){};
};

///A general action object
class Action: public virtual PolymorphicHypIO{
  public:
    ///do the appropriate action for a start event
    /**
     * @param r the response to record our actions in
     * @param s the string to act on
     */
    virtual void doStart(ScriptResponseStart& r,SP<String> s)=0;
    ///do the appropriate action for a win event
    /**
     * @param r the response to record our actions in
     * @param s the string to act on
     */
    virtual void doWin(ScriptResponseWin& r,SP<String> s)=0;
    ///do the appropriate action for a move event
    /**
     * @param r the response to record our actions in
     * @param s the string to act on
     */
    virtual void doMove(ScriptResponseMove& r,SP<String> s)=0;
    ///do the appropriate action for a move event
    /**
     * @param r the response to record our actions in
     * @param s the string to act on
     */
    virtual void doSelect(ScriptResponseSelect& r,SP<String> s)=0;

    ///virtual destructor so that implementations will delete correctly
    virtual ~Action(){};

    ///the default Action to use when creating lists of Actions
    static const SP<Action> defaultvalue;
    ///enable write to access the protected method
    friend bool write(HypOStream& s,const SP<const Action>& c);
};

///Read an Action (as a pointer) from a stream
/**
 * fills the pointer with a new Action object. The actual instance type
 * is read from the stream.
 * @param s the stream to read from
 * @param c pointer to Action variable to stick the read Action in
 * @return an IOResult object that contains the status of the read
 */
IOResult read(HypIStream& s,SP<Action>& c);
///write an Action (from a pointer) to a stream
/**
 * @param s the stream to write to
 * @param c the Action to write
 * @return true if i was written ok
 */
bool write(HypOStream& s,const SP<const Action>& c);

///An event in a script
/**
 * contains trigger for an event and conditions on it's trigger and the actions
 * to execute when the event is triggered
 */
class Event{
  public:
    int trigger;///< The trigger(s) for the event as a bitmask
    SP<Condition> condition;///<The condition for the event to trigger
    SP<Action> action;///<The action to take when it triggers

    /// Create an empty event that never triggers and has a default condition and action
    Event():trigger(0),condition(Condition::defaultvalue),action(Action::defaultvalue){};
    ///Create an event with the specified triggers, conditions and actions
    /**
     * @param trigger the trigger(s) for the event as a bitmask
     * @param condition the condition for the event to trigger
     * @param action the action to take when it triggers
     */
    Event(int trigger,SP<Condition> condition,SP<Action> action):
       trigger(trigger),condition(condition),action(action){};
};

///The core script object
class Script{
  private:
    int eventcount; ///< The number of events in this script
    SPA<Event> events; ///< The array of events for this script
    SPA<int> times; ///< The last trigger times for each event
    int now; ///< The time now
  public:
    ///Create a new empty script
    Script():eventcount(0),events(),times(),now(0){};
    ///Create a script for some events
    /**
     * @param eventcount the number of events the new script will have
     * @param events the actual event for the new script
     */
    Script(int eventcount,const SPA<Event>& events):eventcount(eventcount),events(events),times(eventcount),now(0){
      for(int i=0;i<eventcount;++i)
        times[i]=-1;
    };
    /// Get the time the specified event last ran
    /**
     * @param event the index of the event in question
     * @return the time the specified event ran
     */
    inline int getTime(int event) const{
      if(event<0 || event>=eventcount)
        return *((int*)0);
      return times[event];
    }

    /// Update the scripts version of the current time
    /**
     * @param t the time now
     */
    void setnow(int t){
      now=t;
    }

    /// Run the script for a start event
    /**
     * @param s the string
     * @return the result of the script
     */
    ScriptResponseStart runStart(SP<String> s);
    /// Run the script for a win event
    /**
     * @param s the string
     * @return the result of the script
     */
    ScriptResponseWin runWin(SP<String> s);
    /// Run the script for a move event
    /**
     * @param s the string
     * @return the result of the script
     */
    ScriptResponseMove runMove(SP<String> s);
    /// Run the script for a select event
    /**
     * @param s the string
     * @return the result of the script
     */
    ScriptResponseSelect runSelect(SP<String> s);

    /// Private access so it can load the script into it
    friend IOResult read(HypIStream& s,Script& sc);
    /// Private access so it write the data out
    friend bool write(HypOStream& s,const Script& sc);

    /// Get the events in this script
    inline const SPA<Event>& getevents() const{
      return events;
    }
    /// Get the number of events in this script
    inline const int& geteventcount() const{
      return eventcount;
    }
};

///Read a script from a stream
/**
 * @param s the stream to read from
 * @param sc the script object to read into
 * @return the result of the read
 */
IOResult read(HypIStream& s,Script& sc);
///Write a script to a stream
/**
 * @param s the stream to write to
 * @param sc the script to write
 * @return true if the write succeded else false
 */
bool write(HypOStream& s,const Script& sc);

#endif

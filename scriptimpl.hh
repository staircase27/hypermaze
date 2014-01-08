#include "script.hh"

#ifndef SCRIPTIMPL_HH_INC
#define SCRIPTIMPL_HH_INC

///Utility class to implement polymorphic reading of a class
/**
 * Writes the ID for the class then delegates writing data for the object to a read function for that type
 * @tparam T the type this object should be output as. THIS MUST BE THE TYPE OF THE CLASS YOU ARE IMPLEMENTING
 * @tparam ID the id of this type. THIS MUST BE SAME AS IN THE SWITCH IN read(HypIStream,SP<Condition>)
 */
template <class T,int ID>
class PolymorphicHypIOImpl: protected virtual PolymorphicHypIO{
  protected:
    ///@copydoc PolymorphicHypIO
    ///writes the id provided as a template param then delegates writing data to the function for the templated type T
    virtual bool dowrite(HypOStream& s) const{
      if(!write(s,ID,0))
        return false;
      return write(s,(T&)*this);
    }
};

///A class to contain a range for a integer value
struct Range{
  int start;///<The lower limit for the value, INT_MAX means no limit
  int end;///<The upper limit for the value, INT_MAX means no limit
  ///Check if a value is in this range
  /**
   * @param val the value to check
   * @return true of the value is in the range this represents else false
   */
  inline bool inRange(int val){
    return ((start==INT_MAX)||(val>=start))&&((end==INT_MAX)||(val<=end));
  }\
  ///Default contructor that set the range to have no limit in either direction
  Range():start(INT_MAX),end(INT_MAX){};
};

///A condition for a section of string to match with
/**
 * Can contain conditions on selectedness, direction and location of the string elements
 */
struct StringElementCondition{
  /// the condition on the selectedness of the string element
  /**
   * Second bit is if we care (0 care, 1 dont). First is the value to match if we do care.
   * Valid values are 0,1,2. All others are equivelent to one of these
   */
  int selectionCondition;
  int dirnsCondition;///<Mask of directions to accept

  int xrange_count;///<How many different ranges in the x direction are valid
  SPA<Range> xrange;///<a list of the valid ranges in the x direction
  int yrange_count;///<How many different ranges in the y direction are valid
  SPA<Range> yrange;///<a list of the valid ranges in the y direction
  int zrange_count;///<How many different ranges in the z direction are valid
  SPA<Range> zrange;///<a list of the valid ranges in the z direction
  ///Check if a string element matches this condition
  /**
   * @tparam the type for the string pointer
   * @param el the element of the string
   * @returns true if the string matches this condition
   */
  template <class T>
  inline bool matches(T el);
  ///Default constructor that initialises the condition to match everything
  StringElementCondition():selectionCondition(2),dirnsCondition(ALLDIRNSMASK),xrange_count(0),xrange(),yrange_count(0),yrange(),zrange_count(0),zrange(){};
};

///Read a StringElementCondition from a stream
/**
 * @param s the stream to read from
 * @param c reference to a StringElementCondition variable to store the read data in
 * @return an IOResult object that contains the status of the read
 */
IOResult read(HypIStream& s,StringElementCondition& c);
///write a StringElementCondition to a stream
/**
 * @param s the stream to write to
 * @param c the StringElementCondition to write
 * @return true if i was written ok
 */
bool write(HypOStream& s,const StringElementCondition& c);

///A class to store properties for each StringElementCondition in a pattern
/**
 * Stores how to match the pattern against the string including allowed
 * number of elements to match and if we should match as many as possible
 * or as few as possible
 */
struct PatternTag{
  int min;///<the minimum number of times to match the associated condition
  int max;///<the maximum number of times to match the associated condition
  bool greedy;///<flag for i we should match greedily or not
  ///default constructor that sets the tag such that the assicated pattern matches exactly once and greedily
  PatternTag():min(1),max(1),greedy(true){};
};

///Read a PatternTag from a stream
/**
 * @param s the stream to read from
 * @param pt reference to a PatternTag variable to store the read data in
 * @return an IOResult object that contains the status of the read
 */
IOResult read(HypIStream& s,PatternTag& pt);
///write a PatternTag to a stream
/**
 * @param s the stream to write to
 * @param pt the PatternTag to write
 * @return true if i was written ok
 */
bool write(HypOStream& s,const PatternTag& pt);

///Stores the properties of a match during string matching.
/**
 * Used internally to store the state of the matcher if output is needed
 * @tparam POINTER the type of the string pointer we are storing
 */
template <class POINTER>
struct PatternMatch{
  SP<POINTER> start;///<Pointer to the string element at the start of this section of the match
  SP<POINTER> end;///<Pointer to the string element at the end of this section of the match
  ///default constructor that sets to matched nothing
  PatternMatch():start(),end(){};
};

///Class to implement to process matches
/**
 * @tparam POINTER the type of the string pointer we are storing
 */
template <class POINTER>
class StringMatcherCallback{
  public:
    ///method to implement to handle matches
    /**
     * @param groups a list of the ranges of groups matched by this solution
     */
    virtual void process(SPA<Pair<SP<POINTER> > > groups)=0;
};

///a class to implement matching against a string and optionally allow use of the matches
/**
 * The pattern must match the entire string from start to end. A pattern that can match anywhere in the string
 * can be implemented by adding a pattern element that can match anything and any length. The match is done
 * using a recursive backtracking matcher function. The sections of the patter that we are interested in are
 * specified by groups property.
 */ 
class StringMatcher{
  public:
    int count;///<the number of elements to the pattern
    SPA<Pair<PatternTag,StringElementCondition> > pattern;///<a List of the elements of the pattern
    int group_count;///<the number of groups to output
    SPA<Pair<int> > groups;///<the indicies in the part of the start and end of each group to output
    ///default constructor that sets up with no pattern elements and no groups
	  StringMatcher():count(0),pattern(),group_count(0),groups(){};
	  ///get the number of groups this matchers will return
	  /**
	   * @return how many groups this matcher will return
	   */
    inline int groupCount(){return group_count;}
    
    ///check a match against a constant String
    /**
     * can also return the groups if the optional paramiter groups is included. If included
     * it must have length groupCount()
     * @param s the string to match this pattern against
     * @param groups optional argument that will be filled with the groups matched by this pattern
     * @return true if there are any matches of this pattern to the string
     */
	  bool match(const String& s,SPA<Pair<SP<ConstStringPointer> > > groups=SPA<Pair<SP<ConstStringPointer> > >());
    ///check a match against a non-constant String
    /**
     * can also return the groups if the optional paramiter groups is included. If included
     * it must have length groupCount()
     * @param s the string to match this pattern against
     * @param groups optional argument that will be filled with the groups matched by this pattern
     * @return true if there are any matches of this pattern to the string
     */
	  bool match(      String& s,SPA<Pair<SP<     StringPointer> > > groups=SPA<Pair<SP<     StringPointer> > >());
    ///check a match against a constant String and process each match found
    /**
     * can also return the groups if the optional paramiter groups is included. If included
     * it must have length groupCount()
     * @param s the string to match this pattern against
     * @param cb the call back class to use to process the matches
     * @param groups optional argument that will be filled with the groups matched by this pattern
     * @return true if there are any matches of this pattern to the string
     */
	  bool match(const String& s,StringMatcherCallback<ConstStringPointer>& cb,
	      SPA<Pair<SP<ConstStringPointer> > > groups=SPA<Pair<SP<ConstStringPointer> > >());
    ///check a match against a non-constant String
    /**
     * can also return the groups if the optional paramiter groups is included. If included
     * it must have length groupCount()
     * @param s the string to match this pattern against
     * @param cb the call back class to use to process the matches
     * @param groups optional argument that will be filled with the groups matched by this pattern
     * @return true if there are any matches of this pattern to the string
     */
	  bool match(      String& s,StringMatcherCallback<     StringPointer>& cb,
	      SPA<Pair<SP<     StringPointer> > > groups=SPA<Pair<SP<     StringPointer> > >());
	  
	private:
	  ///The internal implementation function for matches
	  /**
	   * called by all the other match functions. This sets up some data storage and then called
	   * matchStep to do the matching.
	   * @tparam STRING the type of the string. Either String or const String
	   * @tparam POINTER the type of a pointer to an element of the string of type STRING.
	   * Either StringPointer or ConstStringPointer respectively
     * @param s the string to match this pattern against
     * @param groups optional argument that will be filled with the groups matched by this pattern
     * @param cb option pointer to the callback
     * @return true if there are any matches of this pattern to the string
     */
	  template <class STRING,class POINTER>
	  bool match(STRING& s,SPA<Pair<SP<POINTER> > > groups,StringMatcherCallback<POINTER>* cb);
	  ///The internal implementation function for a step in matching against a string
	  /**
	   * This function does the actual processing. It finds the "best" match for this segment of the pattern
	   * and calls it's self to check if later steps match. If it doesn't it keeps seaching through "worse"
	   * options till it finds one. Once the last step in the match has been found the data is stored and the
	   * callback if provided is called to proccess the match. If a call back was provided then we keep
	   * searching for solutions till all are found or else we return True immediately.
	   * @tparam STRING the type of the string. Either String or const String
	   * @tparam POINTER the type of a pointer to an element of the string of type STRING.
	   * Either StringPointer or ConstStringPointer respectively
     * @param s the string to match this pattern against
     * @param p the place to start matching this pattern element against
     * @param matches array of objects to use as storage for the match data
     * @param level the index into the pattern that we are matching against
     * @param groups optional argument that will be filled with the groups matched by this pattern
     * @param cb option pointer to the callback
     * @return true if there are any matches of this pattern to the string
     */
	  template <class STRING,class POINTER>
	  bool matchStep(STRING& s,POINTER p,SPA<PatternMatch<POINTER> > matches,int level,
	      SPA<Pair<SP<POINTER> > > groups,StringMatcherCallback<POINTER>* cb);
};

///Read a StringMatcher from a stream
/**
 * @param s the stream to read from
 * @param sm reference to a StringMatcher variable to store the read data in
 * @return an IOResult object that contains the status of the read
 */
IOResult read(HypIStream& s,StringMatcher& sm);
///write a StringMatcher to a stream
/**
 * @param s the stream to write to
 * @param sm the StringMatcher to write
 * @return true if i was written ok
 */
bool write(HypOStream& s,const StringMatcher& sm);

///a condition that always returns True
class ConditionTrue: public Condition, protected PolymorphicHypIOImpl<ConditionTrue,1>{
  public:
    ///Always return true as the condition is always matched
    /**
     * @copydoc Condition::is
     */
    virtual bool is(int time,const Script& script,const String& s){return true;}
};
///Read a ConditionTrue from a stream
/**
 * this is actually a no-op as a ConditionTrue has no data
 * @param s the stream to read from
 * @param c reference to a ConditionTrue variable. This isn't actually changed
 * @return an IOResult object that is always the fully ok read response
 */
inline IOResult read(HypIStream& s,ConditionTrue& c){return IOResult(true,false);}
///write a ConditionTrue to a stream
/**
 * this is actually a no-op as a ConditionTrue has no data
 * @param s the stream to write to
 * @param c the ConditionTrue that would be written if there was anything to write
 * @return true always as writing nothing always succeds
 */
inline bool write(HypOStream& s,const ConditionTrue& c){return true;}

///Condition that matches if any of the subconditions match
class ConditionOr: public Condition, protected PolymorphicHypIOImpl<ConditionOr,2>{
  public:
    SPA<SP<Condition> > conditions;///< the conditions whose results are ored together
    int count;///< the number of conditions we combine the results of
    ///return true if any of the contained conditions return true
    /**
     * @copydoc Condition::is
     */
    virtual bool is(int time,const Script& script,const String& s);
};
///Read a ConditionOr from a stream
/**
 * @param s the stream to read from
 * @param c reference to a ConditionOr variable to store the read data in
 * @return an IOResult object that contains the status of the read
 */
IOResult read(HypIStream& s,ConditionOr& c);
///write a ConditionOr to a stream
/**
 * @param s the stream to write to
 * @param c the ConditionOr to write
 * @return true if i was written ok
 */
bool write(HypOStream& s,const ConditionOr& c);

///Condition that matches if all of the subconditions match
class ConditionAnd: public Condition, protected PolymorphicHypIOImpl<ConditionAnd,3>{
  public:
    SPA<SP<Condition> > conditions;///< the conditions whose results are anded together
    int count;///< the number of conditions we combine the results of
    ///return false if any of the contained conditions return false
    /**
     * @copydoc Condition::is
     */
    virtual bool is(int time,const Script& script,const String& s);
};
///Read a ConditionAnd from a stream
/**
 * @param s the stream to read from
 * @param c reference to a ConditionAnd variable to store the read data in
 * @return an IOResult object that contains the status of the read
 */
IOResult read(HypIStream& s,ConditionAnd& c);
///write a ConditionAnd to a stream
/**
 * @param s the stream to write to
 * @param c the ConditionAnd to write
 * @return true if i was written ok
 */
bool write(HypOStream& s,const ConditionAnd& c);

///condition that matches if the contained condition doesn't match
class ConditionNot: public Condition, protected PolymorphicHypIOImpl<ConditionNot,4>{
  public:
    SP<Condition> condition;///<the condition whose result is negated
    ///return false if the contained condition returns true
    /**
     * @copydoc Condition::is
     */
    virtual bool is(int time,const Script& script,const String& s){
      return !condition->is(time,script,s);
    }
};
///Read a ConditionNot from a stream
/**
 * @param s the stream to read from
 * @param c reference to a ConditionNot variable to store the read data in
 * @return an IOResult object that contains the status of the read
 */
IOResult read(HypIStream& s,ConditionNot& c);
///write a ConditionNot to a stream
/**
 * @param s the stream to write to
 * @param c the ConditionNot to write
 * @return true if i was written ok
 */
bool write(HypOStream& s,const ConditionNot& c);

///condition that matches if another event has run and last ran a specified number of seconds ago
class ConditionAfter: public Condition, protected PolymorphicHypIOImpl<ConditionAfter,5>{
  public:
    int event;///<the id of the event that has to run before this can match
    int delay;///<the delay after the event has run before this can match
    ///return true if the specified event has run and the specified delay has passed
    /**
     * @copydoc Condition::is
     */
    virtual bool is(int time,const Script& script,const String& s);
};
///Read a ConditionAfter from a stream
/**
 * @param s the stream to read from
 * @param c reference to a ConditionAfter variable to store the read data in
 * @return an IOResult object that contains the status of the read
 */
IOResult read(HypIStream& s,ConditionAfter& c);
///write a ConditionAfter to a stream
/**
 * @param s the stream to write to
 * @param c the ConditionAfter to write
 * @return true if i was written ok
 */
bool write(HypOStream& s,const ConditionAfter& c);

///condition that matches if another event hasn't run yet
class ConditionBefore: public Condition, protected PolymorphicHypIOImpl<ConditionBefore,6>{
  public:
    int event;///<the id of the event that has to run before this can match
    ///return true if the specified event hasn't run
    /**
     * @copydoc Condition::is
     */
    virtual bool is(int time,const Script& script,const String& s);
};
///Read a ConditionBefore from a stream
/**
 * @param s the stream to read from
 * @param c reference to a ConditionBefore variable to store the read data in
 * @return an IOResult object that contains the status of the read
 */
IOResult read(HypIStream& s,ConditionBefore& c);
///write a ConditionBefore to a stream
/**
 * @param s the stream to write to
 * @param c the ConditionBefore to write
 * @return true if i was written ok
 */
bool write(HypOStream& s,const ConditionBefore& c);

///condition that matches if the string matches a StringMatcher pattern
class ConditionStringPattern: public Condition, protected PolymorphicHypIOImpl<ConditionStringPattern,7>{
  public:
    StringMatcher sm;///<the string matcher to check the string against
  
    ///return true if the string matches the StringMatcher
    /**
     * @copydoc Condition::is
     */
    virtual bool is(int time,const Script& script,const String& s){
      return sm.match(s);
    }
};
///Read a ConditionStringPattern from a stream
/**
 * @param s the stream to read from
 * @param c reference to a ConditionStringPattern variable to store the read data in
 * @return an IOResult object that contains the status of the read
 */
inline IOResult read(HypIStream& s,ConditionStringPattern& c){
  return read(s,c.sm);
}
///write a ConditionStringPattern to a stream
/**
 * @param s the stream to write to
 * @param c the ConditionStringPattern to write
 * @return true if i was written ok
 */
inline bool write(HypOStream& s,const ConditionStringPattern& c){
  return write(s,c.sm);
}

///A specialisation of Action for Actions that apply to win events only
class ActionWin: public virtual Action{
  public:
    ///@copydoc Action::doStart
    ///implemented as a no-op
		virtual void doStart(ScriptResponseStart& r,String& s){};
    ///@copydoc Action::doWin
    ///only do method to implement
		virtual void doWin(ScriptResponseWin& r,String& s)=0;
    ///@copydoc Action::doMove
    ///implemented as a no-op
		virtual void doMove(ScriptResponseMove& r,String& s){};
    ///@copydoc Action::doSelect
    ///implemented as a no-op
		virtual void doSelect(ScriptResponseSelect& r,String& s){};
		///virtual destructor so that implementations will delete correctly
		virtual ~ActionWin(){};
};
///A specialisation of Action for Actions that apply to all events the same way
class ActionCommon: public virtual Action{
  public:
    ///do the appropriate action for an event
    /**
     * @param r the response to record our actions in
     * @param s the string to act on
     */
		virtual void doCommon(ScriptResponse& r,String& s)=0;
    ///@copydoc Action::doStart
    ///calls doCommon
		virtual void doStart(ScriptResponseStart& r,String& s){doCommon(r,s);};
    ///@copydoc Action::doWin
    ///calls doCommon
		virtual void doWin(ScriptResponseWin& r,String& s){doCommon(r,s);};
    ///@copydoc Action::doMove
    ///calls doCommon
		virtual void doMove(ScriptResponseMove& r,String& s){doCommon(r,s);};
    ///@copydoc Action::doSelect
    ///calls doCommon
		virtual void doSelect(ScriptResponseSelect& r,String& s){doCommon(r,s);};
		///virtual destructor so that implementations will delete correctly
		virtual ~ActionCommon(){};
};

///an action that does nothing
class ActionNothing:public ActionCommon, protected PolymorphicHypIOImpl<ActionNothing,1>{
  public:
    ///@copydoc ActionCommon::doCommon
    ///implemented as a no-op
    virtual void doCommon(ScriptResponse& r,String& s){};
};
///Read an ActionNothing from a stream
/**
 * @param s the stream to read from
 * @param a ActionNothing variable to read the data into
 * @return an IOResult object that contains the status of the read
 */
inline IOResult read(HypIStream& s,ActionNothing& a){
  return IOResult(true,false);
}
///write an ActionNothing to a stream
/**
 * @param s the stream to write to
 * @param a the ActionNothing to write
 * @return true if i was written ok
 */
inline bool write(HypOStream& s,const ActionNothing& a){
  return true;
}

//an action to display a message to the player
class ActionMessage:public ActionCommon, protected PolymorphicHypIOImpl<ActionMessage,2>{
  public:
    Message m;///<the message to show
    ///@copydoc ActionCommon::doCommon
    ///adds a message to the message list in the response
    virtual void doCommon(ScriptResponse& r,String& s);
};
///Read an ActionMessage from a stream
/**
 * @param s the stream to read from
 * @param a ActionMessage variable to read the data into
 * @return an IOResult object that contains the status of the read
 */
IOResult read(HypIStream& s,ActionMessage& a);
///write an ActionMessage to a stream
/**
 * @param s the stream to write to
 * @param a the ActionMessage to write
 * @return true if i was written ok
 */
bool write(HypOStream& s,const ActionMessage& a);

///an action to block the win
class ActionBlockWin:public ActionWin, protected PolymorphicHypIOImpl<ActionBlockWin,3>{
  public:
    ///@copydoc Action::doWin
    ///sets the block win flag in the response object
    virtual void doWin(ScriptResponseWin& r,String& s){
      cout<<"Win Blocked"<<endl;
      r.block=true;
    };
};
///Read an ActionBlockWin from a stream
/**
 * @param s the stream to read from
 * @param a ActionBlockWin variable to read the data into
 * @return an IOResult object that contains the status of the read
 */
inline IOResult read(HypIStream& s,ActionBlockWin& a){
  return IOResult(true,false);
}
///write an ActionBlockWin to a stream
/**
 * @param s the stream to write to
 * @param a the ActionBlockWin to write
 * @return true if i was written ok
 */
inline bool write(HypOStream& s,const ActionBlockWin& a){
  return true;
}

///set the message to show in the win screen
class ActionWinMessage:public ActionWin, protected PolymorphicHypIOImpl<ActionWinMessage,4>{
  public:
    Message m;///<the message to show
    ///@copydoc Action::doWin
    ///sets the message to show on the win screen in the response object
    virtual void doWin(ScriptResponseWin& r,String& s){
      r.winMessage=m;
    };
};
///Read an ActionWinMessage from a stream
/**
 * @param s the stream to read from
 * @param a ActionWinMessage variable to read the data into
 * @return an IOResult object that contains the status of the read
 */
IOResult read(HypIStream& s,ActionWinMessage& a);
///write an ActionWinMessage to a stream
/**
 * @param s the stream to write to
 * @param a the ActionWinMessage to write
 * @return true if i was written ok
 */
bool write(HypOStream& s,const ActionWinMessage& a);

///set the next level to show on the win screen
class ActionWinNextLevel:public ActionWin, protected PolymorphicHypIOImpl<ActionWinNextLevel,5>{
  public:
    Pair<SPA<const char> > nextLevel;///<the next level's url and a "name" for it
    ///@copydoc Action::doWin
    ///sets the next level to show on the win screen in the response object
    virtual void doWin(ScriptResponseWin& r,String& s){
      r.nextLevel=nextLevel;
    };
};
///Read an ActionWinNextLevel from a stream
/**
 * @param s the stream to read from
 * @param a ActionWinNextLevel variable to read the data into
 * @return an IOResult object that contains the status of the read
 */
IOResult read(HypIStream& s,ActionWinNextLevel& a);
///write an ActionWinNextLevel to a stream
/**
 * @param s the stream to write to
 * @param a the ActionWinNextLevel to write
 * @return true if i was written ok
 */
bool write(HypOStream& s,const ActionWinNextLevel& a);

///force win after a move or select even when the normal win condition isn't true
class ActionForceWin:public Action, protected PolymorphicHypIOImpl<ActionForceWin,6>{
  public:
    ///@copydoc Action::doStart
    ///implemented as a no-op as doesn't make sence to force a win at startup
		virtual void doStart(ScriptResponseStart& r,String& s){};
    ///@copydoc Action::doWin
    ///implemented as a no-op as can't force win when already winning
		virtual void doWin(ScriptResponseWin& r,String& s){};
    ///@copydoc Action::doMove
    ///sets the force win flag in the response objct
		virtual void doMove(ScriptResponseMove& r,String& s){r.forceWin=true;};
    ///@copydoc Action::doSelect
    ///sets the force win flag in the response objct
		virtual void doSelect(ScriptResponseSelect& r,String& s){r.forceWin=true;};
};
///Read an ActionForceWin from a stream
/**
 * @param s the stream to read from
 * @param a ActionForceWin variable to read the data into
 * @return an IOResult object that contains the status of the read
 */
inline IOResult read(HypIStream& s,ActionForceWin& a){
  return IOResult(true,false);
}
///write an ActionForceWin to a stream
/**
 * @param s the stream to write to
 * @param a the ActionForceWin to write
 * @return true if i was written ok
 */
inline bool write(HypOStream& s,const ActionForceWin& a){
  return true;
}

///action to select or deselect portions of a string based on conditions
class ActionStringConditionSelect:public ActionCommon, protected PolymorphicHypIOImpl<ActionStringConditionSelect,7>{
  public:
    StringElementCondition change; ///<if the string element's selectedness should be changed
    StringElementCondition select; ///<if the string element should be changed to selected or deselected
  
    ///@copydoc ActionCommon::doCommon
    ///changes the selectedness of string elements and records that changes have been made in the response
    virtual void doCommon(ScriptResponse& r,String& s);
};
///Read an ActionStringConditionSelect from a stream
/**
 * @param s the stream to read from
 * @param a ActionStringConditionSelect variable to read the data into
 * @return an IOResult object that contains the status of the read
 */
IOResult read(HypIStream& s,ActionStringConditionSelect& a);
///write an ActionStringConditionSelect to a stream
/**
 * @param s the stream to write to
 * @param a the ActionStringConditionSelect to write
 * @return true if i was written ok
 */
bool write(HypOStream& s,const ActionStringConditionSelect& a);

///action to set the route that selected parts of the string follow
class ActionSetStringRoute:public ActionCommon, protected PolymorphicHypIOImpl<ActionSetStringRoute,9>{
  public:
    StringMatcher ranges;///<String matcher to select the ranges to set the route for
    int count;///<the number of elements in the new route
    SPA<Dirn> route;///<the directions the string goes in the new route
    bool all;///<set the route for all matches or just the first. if all then the changed string must eventually not match
  
    ///@copydoc ActionCommon::doCommon
    ///this calls the StringMatcher to find a match then edits the retuned match.
    ///if all is true this is repeated till the pattern fails to match. this can lead to infinite loops
    virtual void doCommon(ScriptResponse& r,String& s);
};
///Read an ActionSetStringRoute from a stream
/**
 * @param s the stream to read from
 * @param a ActionSetStringRoute variable to read the data into
 * @return an IOResult object that contains the status of the read
 */
IOResult read(HypIStream& s,ActionSetStringRoute& a);
///write an ActionSetStringRoute to a stream
/**
 * @param s the stream to write to
 * @param a the ActionSetStringRoute to write
 * @return true if i was written ok
 */
bool write(HypOStream& s,const ActionSetStringRoute& a);

#endif

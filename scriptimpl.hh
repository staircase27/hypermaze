#include "script.hh"

#ifndef SCRIPTIMPL_HH_INC
#define SCRIPTIMPL_HH_INC

class ConditionTrue;

///Utility class to implement polymorphic reading of a class
/**
 * Writes the ID for the class then delegates writing data for the object to a read function for that type
 * @tparam T the type this object should be output as. THIS MUST BE THE TYPE OF THE CLASS YOU ARE IMPLEMENTING
 * @tparam ID the id of this type. THIS MUST BE SAME AS IN THE SWITCH IN read(HypIStream,SP<Condition>)
 */
template <class T,int ID>
class PolymorphicHypIOImpl{
  protected:
    bool dowrite(HypOStream& s){
      if(!write(s,ID,0))
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
  StringElementCondition():selectionCondition(2),dirnsCondition(ALLDIRNSMASK),xrange_count(0),xrange(0),yrange_count(0),yrange(0),zrange_count(0),zrange(0){};
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
  PatternMatch():start(0),end(0),length(0){};
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
    virtual void process(SPA<pair<SP<POINTER>,SP<POINTER> > > groups)=0;
};

///a class to implement matching against a string and optionally allow use of the matches
/**
 * The pattern must match the entire string from start to end. A pattern that can match anywhere in the string
 * can be implemented by adding a pattern element that can match anything and any length. The match is done
 * using a recursive backtracking matcher function. The sections of the patter that we are interested in are
 * specified by groups property.
 */ 
class StringMatcher{
  private:
    int count;///<the number of elements to the pattern
    SPA<pair<PatternTag,StringElementCondition> > pattern;///<a List of the elements of the pattern
    int group_count;///<the number of groups to output
    SPA<pair<int,int> > groups;///<the indicies in the part of the start and end of each group to output
  public:
    ///default constructor that sets up with no pattern elements and no groups
	  StringMatcher():count(0),pattern(0),group_count(0),groups(0){};
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
	  bool match(const String& s,SPA<pair<SP<ConstStringPointer> > > groups=0);
    ///check a match against a non-constant String
    /**
     * can also return the groups if the optional paramiter groups is included. If included
     * it must have length groupCount()
     * @param s the string to match this pattern against
     * @param groups optional argument that will be filled with the groups matched by this pattern
     * @return true if there are any matches of this pattern to the string
     */
	  bool match(      String& s,SPA<pair<SP<     StringPointer> > > groups=0);
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
	      SPA<pair<SP<ConstStringPointer> > > groups=0);
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
	      SPA<pair<SP<     StringPointer> > > groups=0);
	  
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
	  bool match(STRING& s,SPA<pair<SP<POINTER> > > groups,StringMatcherCallback<POINTER>* cb);
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
	      SPA<pair<SP<POINTER> > > groups,StringMatcherCallback<POINTER>* cb);
};

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
IOResult read(HypIStream& s,ConditionTrue& c){return IOResult(true,false);}
///write a ConditionTrue to a stream
/**
 * this is actually a no-op as a ConditionTrue has no data
 * @param s the stream to write to
 * @param c the ConditionTrue that would be written if there was anything to write
 * @return true always as writing nothing always succeds
 */
bool write(HypOStream& s,const ConditionTrue& c){return true;}

///Condition that matches if any of the subconditions match
class ConditionOr: public Condition, protected PolymorphicHypIOImpl<ConditionOr,2>{
  SPA<SP<Condition>> conditions;
  int count;
  public:
    virtual bool is(int time,const Script& script,const String& s);
};
///Read a ConditionOr from a stream
/**
 * @param s the stream to read from
 * @param pt reference to a ConditionOr variable to store the read data in
 * @return an IOResult object that contains the status of the read
 */
IOResult read(HypIStream& s,ConditionOr& c);
///write a ConditionOr to a stream
/**
 * @param s the stream to write to
 * @param pt the ConditionOr to write
 * @return true if i was written ok
 */
bool write(HypOStream& s,const ConditionOr& c);

///Condition that matches if all of the subconditions match
class ConditionAnd: public Condition, protected PolymorphicHypIOImpl<ConditionAnd,3>{
  SPA<SP<Condition>> conditions;
  int count;
  public:
    virtual bool is(int time,const Script& script,const String& s);
};
///Read a ConditionAnd from a stream
/**
 * @param s the stream to read from
 * @param pt reference to a ConditionAnd variable to store the read data in
 * @return an IOResult object that contains the status of the read
 */
IOResult read(HypIStream& s,ConditionAnd& c);
///write a ConditionAnd to a stream
/**
 * @param s the stream to write to
 * @param pt the ConditionAnd to write
 * @return true if i was written ok
 */
bool write(HypOStream& s,const ConditionAnd& c);

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
  
  virtual void process(SPA<pair<SP<StringPointer>,SP<StringPointer> > > groups);
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

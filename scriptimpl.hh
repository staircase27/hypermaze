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

IOResult read(HypIStream& s,StringElementCondition& c);
bool write(HypOStream& s,const StringElementCondition& c);

class StringMatcher;

struct PatternTag{
  int min;
  int max;
  bool greedy;
  PatternTag():min(1),max(1),greedy(true){};
  private:
	  friend class StringMatcher;
};

IOResult read(HypIStream& s,PatternTag& pt);
bool write(HypOStream& s,const PatternTag& pt);

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
  SPA<pair<PatternTag,StringElementCondition> > pattern;
  int group_count;
  SPA<pair<int,int> > groups;
  public:
  
	  StringMatcher():count(0),pattern(0),group_count(0),groups(0){};
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

/**
 * @file string.hh
 * @brief The solution string for a maze and the classes to handle it
 */
#include "vector.hh"
#include "dirns.hh"
#include "maze.hh"
#include <list>

#ifdef IOSTREAM
#include <istream>
#endif
#ifndef STRING_HH_INC
#define STRING_HH_INC

// forward declarations for friend declarations
class StringPlay;
class StringEdit;

/// An element of the solution string
struct StringElement{
  Vector pos;///< The location of the start of this element
  Dirn d; ///< The direction this element points in
  bool selected; ///< If this element is selected
  /// Construct a new element
  /**
   * @param pos the location of the start of this element
   * @param d the direction this element points in
   * @param selected if this element is selected
   */
  StringElement(Vector pos,Dirn d,bool selected):pos(pos),d(d),selected(selected){};
};

/// A pointer to an element of the string
class StringPointer{
  private:
    std::list<StringElement>::iterator el;///< The iterator containing the actual element
  public:

    /// Access the actual element pointed to
    /**
     * @return the string element
     */
    const StringElement& operator *()const{
      return *el;
    }
    /// Access the actual element pointed to
    /**
     * @return the string element
     */
    const StringElement* operator ->() const{
      return el.operator->();
    }

    /// move this so it points to the next element
    /**
     * @return *this
     */
    StringPointer& operator++(){
      ++el;
      return *this;
    }
    /// move this so it points to the previous element
    /**
     * @return *this
     */
    StringPointer& operator--(){
      --el;
      return *this;
    }

    /// check if this string pointer points to a different element to another
    /**
     * @param other the element to compare to
     * @return true if the elements are different or else false
     */
    bool operator!=(const StringPointer& other) const{
      return el!=other.el;
    }
    /// check if this string pointer points to the same element as another
    /**
     * @param other the element to compare to
     * @return true if the elements are the same or else false
     */
    bool operator==(const StringPointer& other) const{
      return el==other.el;
    }

    /// update this pointer to point to the same element as another
    /**
     * @param other the element we should update to match
     * @return *this
     */
    StringPointer& operator=(StringPointer other){
      el=other.el;
      return *this;
    }

    ///Constructor for a StringPointer
    /**
     * This is usually called by the String as the route is hidden
     * @param el the element to point to
     */
    StringPointer(std::list<StringElement>::iterator el):el(el){};

    ///Declared friend so they can access the element for updates to the string
    friend class StringPlay;
    ///Declared friend so they can access the element for updates to the string
    friend class StringEdit;
};

/// A non editable pointer to an element of the string
class ConstStringPointer{
  private:
    std::list<StringElement>::const_iterator el;///< The iterator containing the actual element
  public:

    /// Access the actual element pointed to
    /**
     * @return the string element
     */
    const StringElement& operator *()const{
      return *el;
    }
    /// Access the actual element pointed to
    /**
     * @return the string element
     */
    const StringElement* operator ->()const{
      return el.operator->();
    }

    /// move this so it points to the next element
    /**
     * @return *this
     */
    ConstStringPointer& operator++(){
      ++el;
      return *this;
    }
    /// move this so it points to the previous element
    /**
     * @return *this
     */
    ConstStringPointer& operator--(){
      --el;
      return *this;
    }

    /// check if this string pointer points to a different element to another
    /**
     * @param other the element to compare to
     * @return true if the elements are different or else false
     */
    bool operator!=(const ConstStringPointer& other) const{
      return el!=other.el;
    }
    /// check if this string pointer points to the same element as another
    /**
     * @param other the element to compare to
     * @return true if the elements are the same or else false
     */
    bool operator==(const ConstStringPointer& other) const{
      return el==other.el;
    }

    /// update this pointer to point to the same element as another
    /**
     * @param other the element we should update to match
     * @return *this
     */
    ConstStringPointer& operator=(const ConstStringPointer& other){
      el=other.el;
      return *this;
    }

    ///Constructor for a StringPointer
    /**
     * This is usually called by the String as the route is hidden
     * @param el the element to point to
     */
    ConstStringPointer(std::list<StringElement>::const_iterator el):el(el){};
};

#ifdef IOSTREAM
/// write a representation of the string element to a steam
/**
 * @param o the stream to write to
 * @param e the element to write
 * @return the stream o
 */
inline std::ostream& operator<<(std::ostream& o,const StringElement& e){
  return o<<"<StringElement "<<e.pos<<" "<<e.d<<" "<<e.selected<<">";
}
/// write a representation of the string pointer to a steam
/**
 * @param o the stream to write to
 * @param e the element to write
 * @return the stream o
 */
inline std::ostream& operator<<(std::ostream& o,const StringPointer& e){
  return o<<"<Pointer to "<<*e<<">";
}
/// write a representation of the string pointer to a steam
/**
 * @param o the stream to write to
 * @param e the element to write
 * @return the stream o
 */
inline std::ostream& operator<<(std::ostream& o,const ConstStringPointer& e){
  return o<<"<Pointer to "<<*e<<">";
}
#endif

/// The solution string object
/**
 * \note we store information about the string and it's route in multiple ways so
 * changes can only be made via the StringPlay or StringEdit classes to ensure
 * consistency
 */
class String{
    std::list<StringElement> route;///< The actual route of the string
    Vector endPos;///< The end location for the string
  public:
    const Maze maze; ///< The maze this string is on
    const Dirn stringDir;///< the direction the string is in
    const Dirn targetDir;///< the direction to move the string

    ///Construct a new string for a maze
    /**
     * @param m the maze this string is for
     * @param stringDir the direction the string is in
     * @param targetDir the direction to move the string
     */
    String(Maze m,Dirn stringDir=LEFT,Dirn targetDir=FORWARD);

    ///Get the start location
    /**
     * @return a Vector that is the start of the string
     */
    inline const Vector& getStart() const{
      return route.front().pos;
    }
    ///Get the end location
    /**
     * @return a Vector that is the end of the string
     */
    inline const Vector& getEnd() const{
      return endPos;
    }

    ///Get the pointer to the first element of the string
    /**
     * @return a pointer to the first element of the string
     */
    inline StringPointer begin(){
      return StringPointer(route.begin());
    }
    ///Get the pointer to just after the last element of the string
    /**
     * @return a pointer to just after the last element of the string
     */
    inline StringPointer end(){
      return StringPointer(route.end());
    }
    ///Get the pointer to the first element of the string
    /**
     * @return a pointer to the first element of the string
     */
    inline ConstStringPointer begin() const{
      return ConstStringPointer(route.begin());
    }
    ///Get the pointer to just after the last element of the string
    /**
     * @return a pointer to just after the last element of the string
     */
    inline ConstStringPointer end() const{
      return ConstStringPointer(route.end());
    }

    ///Get the length of the string
    /**
     * @return the string's length
     */
    inline int length() const{
      return route.size();
    }

    ///check if the string is in a win location
    /**
     * @return true if they have won else false
     */
    bool hasWon() const;

  private:
    ///Copying isn't allowed
    String& operator=(const String& o);
    ///Copying isn't allowed
    String(const String& o);

    ///Friend to allow access to update the string
    friend class StringPlay;
    ///Friend to allow access to update the string
    friend class StringEdit;
    #ifdef IOSTREAM
    ///Friend to allow access to the strings internal data
    friend std::ostream& operator <<(std::ostream& o,SP<String> s);
    ///Friend to allow access to the strings internal data
    friend std::ostream& operator <<(std::ostream& o,const StringPlay& s);
    #endif
};

template <class T>
class LimitedStack;

class HistoryElement;

/// give access to update the string following the rules of the puzzle
class StringPlay{
  SP<String> s;///< The string we are working on

  int score; ///< The current score for the play so far

  LimitedStack<HistoryElement>* undohistory;///< the undo history
  bool inextendedmove; ///< are we in an extended move

  public:
    ///Create a new String Play
    /**
     * To reset to the start or make this as new with a different String use setString
     * @param s the initial String
     */
    StringPlay(SP<String> s);

    /// Get the current string
    /**
     * @return the string
     */
    inline SP<String> getString(){
      return s;
    }

    ///Set the string to a new string
    /**
     * This also resets everything to as new.
     * @param s the new string
     */
    void SetString(SP<String> s);

    /// Get the current score
    /**
     * @return the score
     */
    inline int getScore(){
      return score;
    }

    /// Starts an extended move
    /**
     * extended moves are used to ensure a script or keyboard edit breaks a drag
     */
    inline void startExtendedMove(){
      inextendedmove=true;
    }

    /// Signal to this StringPlay that an external edit has happened
    /**
     * This stops any extended move and clears the history
     */
    void externalEditHappened();

    /// Slide the selection in our out
    /**
     * @param moveEnd true for move the end or false for moving the start
     * @param out move the selection towards the centre or away from the centre
     * @return true if the selection actually changed else false
     */
    bool slide(bool moveEnd,bool out);

    /// Set the specified element to selected
    /**
     * @param p the element to set the selection state of
     * @param selected if it should be set to selected or unselected
     */
    void setSelected(StringPointer p,bool selected){
      inextendedmove=false;
      p.el->selected=selected;
    }

    ///Check if a move of the string in the specified direction is allowed
    /**
     * @param d the direction we want to move in
     * @return true if the move is allowed
     */
    bool canMove(Dirn d);

  private:
    ///Move the string in the specified direction
    /**
     * @note this will do the move even if it isn't valid. Always called via
     * tryMove or undo
     * @param d the direction to move in
     * @param undo if this move is an undo. This is used so we update the score
     * in the correct direction. Also if this move isn't an undo it adds the
     * move to the history
     */
    void doMove(Dirn d,bool undo);

  public:
    /// Undo a previous move
    /**
     * @param extendedmove if this undo is part of an extended move.
     * @return true if a move was undone false otherwise
     */
    bool undo(bool extendedmove=false);

    ///Try to move the string in the specified direction
    /**
     * This first checks if a move is allowed and if it does then it does it.
     * @param d the direction to move in.
     * @param extendedmove if this undo is part of an extended move.
     * @return true if a move was undone false otherwise
     */
    bool tryMove(Dirn d,bool extendedmove=false);

    #ifdef IOSTREAM
    friend std::ostream& operator <<(std::ostream& o,const StringPlay& s);
    #endif

    ///Destry the StringPlay and release any memory used
    ~StringPlay();
  private:
    ///Copying isn't allowed
    StringPlay(const StringPlay& o);
    ///Copying isn't allowed
    StringPlay& operator=(const StringPlay& o);
};

/// give access to update the string in arbitrary ways
class StringEdit{
  SP<String> s; ///< The string we are working on

  public:
    ///Create a new String Edit
    /**
     * @param s the initial String
     */
    StringEdit(SP<String> s):s(s){};

    /// Get the current string
    /**
     * @return the string
     */
    inline SP<String> getString(){
      return s;
    }

    /// Set the specified element to selected
    /**
     * Provided here for convenience
     * @note StringPlay::externalEditHappened() should be called for any
     * StringPlays using the same String
     * @param p the element to set the selection state of
     * @param selected if it should be set to selected or unselected
     */
    inline void setSelected(StringPointer p,bool selected){
      p.el->selected=selected;
    }

    /// Set the route for a section of the string to a new route
    /**
     * if the new route doesn't fill the gap String::stringDir will be used to
     * connect up. This also slides the segment after the move around to line up
     * with the new route.
     * @note StringPlay::externalEditHappened() should be called for any
     * StringPlays using the same String
     * @param sp the start of the range to replace with the new route
     * @param ep the element after range to replace with the new route
     * @param count the number of elements in the new route
     * @param newRoute the Dirns that make up the new wroute
     */
    void setStringSegment(StringPointer sp,StringPointer ep,int count,SPA<Dirn> newRoute);
};

#ifdef IOSTREAM
/// write a representation of the String to a stream
/**
 * @param o the stream to write to
 * @param s the string to write
 * @return the stream o
 */
inline std::ostream& operator<<(std::ostream& o,SP<String> s){
  o<<"<String ";
  for(std::list<StringElement>::iterator it=s->route.begin();it!=s->route.end();++it)
    o<<it->pos<<"-"<<(it->selected?"":"*")<<it->d<<(it->selected?"":"*")<<"-";
  return o<<s->endPos<<">";
}

/// write a representation of the StringPlay to a stream
/**
 * This basically writes the String that this contains out
 * @param o the stream to write to
 * @param s the string to write
 * @return the stream o
 */
inline std::ostream& operator<<(std::ostream& o,const StringPlay& s){
  o<<"<StringPlay ";
  for(std::list<StringElement>::iterator it=s.s->route.begin();it!=s.s->route.end();++it)
    o<<it->pos<<"-"<<(it->selected?"":"*")<<it->d<<(it->selected?"":"*")<<"-";
  return o<<s.s->endPos<<">";
}
#endif

#endif


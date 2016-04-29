/**
 * @file string.cc
 * @brief The implementation of string.hh
 */
#include "string.hh"

String::String(Maze m,Dirn stringDir,Dirn targetDir):maze(m),endPos(0,0,0),route(),stringDir(stringDir),targetDir(targetDir){
  Vector start=m.size().dotProduct(to_shift_vector(stringDir))*to_shift_vector(stringDir)+
      m.size().dotProduct(to_shift_vector(targetDir))*to_shift_vector(targetDir)+
      m.size().dotProduct(to_vector(perpendicular(stringDir,targetDir)))/2*to_vector(perpendicular(stringDir,targetDir));
  Vector pos=start;
  while(pos.dotProduct(to_vector(stringDir))!=(start+m.size()).dotProduct(to_vector(stringDir))){
    route.push_back(StringElement(pos,stringDir,true));
    pos+=to_vector(stringDir);
  }
  route.front().selected=false;
  endPos=pos;
};

bool String::hasWon() const{
  Vector d=to_vector(targetDir);
  int t=maze.size().dotProduct(-to_shift_vector(opposite(targetDir)));
  if(endPos.dotProduct(d)<t)
    return false;
  for(std::list<StringElement>::const_iterator it=route.begin();it!=route.end();++it)
    if(it->pos.dotProduct(d)<t)
      return false;
  return true;
}

///A fixed size stack
template <class T>
class LimitedStack{
  private:
    SPA<T> start;///< The start of the stacks storage
    SPA<T> end;///< The end of the stacks storage
    SPA<T> top;///< The top of the stack
    SPA<T> bottom;///< The bottom of the stack

  public:
    /// Create a new stack
    /**
     * @param len the length of the new stack
     */
    LimitedStack(int len):start(len),end(start+len-1),top(start),bottom(start){};

    /// Check if the stack is empty
    /**
     * @return true if the stack is empty else false
     */
    bool empty(){
      return top==bottom;
    }

    /// Get the value at the top of the stack
    /**
     * @return the top element on the stack
     */
    inline const T& getTop(){
      return *top;
    }

    /// remove and return the value at the top of the stack
    /**
     * @return the top element on the stack
     */
    const T& popTop(){
      const T& tmp=*top;
      if(top==start)
        top=end;
      else
        --top;
      return tmp;
    }

    /// Add an element to the top of the stack
    /**
     * This may replace elements at the bottom of the stack if space runs out
     * @param el the element to add
     */
    void pushTop(const T& el){
      if(top==end){
        top=start;
        if(top==bottom)
          ++bottom;
      }else{
        ++top;
        if(top==bottom)
          if(bottom==end)
             bottom=top;
          else
             ++bottom;
      }
      *top=el;
    }

    ///Clear the stack
    /**
     * Doesn't actually delete any elements but they won't be accessed again
     */
    void clear(){
      top=bottom=start;
    }
};

/// A Bitset
class Bitset{
  SPA<unsigned char> bits; ///< the actual bits in this bitset
  public:
    /// Construct an empty bitset.
    /**
     * All accesses to this bitset are invalid
     */
    Bitset():bits(){};
    /// Construct a bitset of the specified size
    /**
     * @param length the length to make the bitset.
     */
    explicit Bitset(const unsigned int& length):bits((length+CHAR_BIT-1)/CHAR_BIT){};
    /// Get a single bit from the bitset
    /**
     * @param i the index of the bit.
     * @return true if the bit is set else false
     */
    inline bool get(int i){
      return (bits[i/CHAR_BIT]&(1<<(i%CHAR_BIT)))!=0;
    }
    /// Set a single bit in the bitset
    /**
     * @param i the index of the bit to set
     */
    inline void set(int i){
      bits[i/CHAR_BIT]|=(1<<(i%CHAR_BIT));
    }
    /// Unset a single bit in the bitset
    /**
     * @param i the index of the bit to unset
     */
    inline void unset(int i){
      bits[i/CHAR_BIT]&=~(1<<(i%CHAR_BIT));
    }
};

///An element in the move history
class HistoryElement{
  public:
    int length; ///< The length of the string just after the move
    Bitset selected; ///< The selection state of the string just after the move
    /// was an element of the string selected
    /**
     * @param i the index of the element to check
     * @return true it was selected
     */
    bool isselected(int i){
      return selected.get(i);
    }
    Dirn d; ///< the direction the move was in
    std::list<Dirn> startcollapsed; ///< the route segments that where collapsed from the start of the string
    std::list<Dirn> endcollapsed; ///< the route segments that where collapsed from the end of the string

    /// Create a new history element
    /**
     * @param length The length of the string just after the move
     * @param selected The selection state of the string just after the move
     * @param d the direction the move was in
     * @param startcollapsed the route segments that where collapsed from the start of the string
     * @param endcollapsed the route segments that where collapsed from the end of the string
     */
    HistoryElement(int length,Bitset selected,Dirn d,std::list<Dirn> startcollapsed,std::list<Dirn> endcollapsed):
        length(length),selected(selected),d(d),startcollapsed(startcollapsed),endcollapsed(endcollapsed){};
    /// Create a new empty history element
    HistoryElement():length(0),selected(),d(),startcollapsed(),endcollapsed(){};
};

StringPlay::StringPlay(SP<String> s):s(s),score(0),undohistory(new LimitedStack<HistoryElement>(10+2*s->maze.size().dotProduct(Vector(1,1,1)))),inextendedmove(false){};

void StringPlay::SetString(SP<String> s){
  this->s=s;
  score=0;
  inextendedmove=false;
  delete undohistory;
  undohistory=new LimitedStack<HistoryElement>(10+2*s->maze.size().dotProduct(Vector(1,1,1)));
}

void StringPlay::externalEditHappened(){
  inextendedmove=false;
  undohistory->clear();
}
bool StringPlay::slide(bool moveEnd,bool out){
  if(moveEnd){
    std::list<StringElement>::reverse_iterator it;
    for(it=s->route.rbegin();it!=s->route.rend();++it)
      if(it->selected)
        break;
    if(out){
      if(it==s->route.rbegin())
        return false;
      --it;
      it->selected=true;
    }else{
      if(it==s->route.rend())
        return false;
      it->selected=false;
    }
  }else{
    std::list<StringElement>::iterator it;
    for(it=s->route.begin();it!=s->route.end();++it)
      if(it->selected)
        break;
    if(out){
      if(it==s->route.begin())
        return false;
      --it;
      it->selected=true;
    }else{
      if(it==s->route.end())
        return false;
      it->selected=false;
    }
  }
  inextendedmove=false;
  return true;
}

bool StringPlay::canMove(Dirn d){
  // Not allowed to try and drag the end elements out of the maze
  if(d==opposite(s->stringDir)){
    if(s->route.front().selected)
      return false;
  }else if(d==s->stringDir){
    if(s->route.back().selected)
      return false;
  }
  bool lastselected=s->route.front().selected;
  bool any=false;
  std::list<StringElement>::iterator previt=s->route.begin();
  std::list<StringElement>::iterator it=s->route.begin();
  for(;it!=s->route.end();previt=it,++it){
    if(!it->selected){
      if(lastselected){
        // element directly after the end of a selection
        // don't allow last element to move in the direction it points unless the element after it
        // (i.e. this element) also points the same direction (i.e. can be deleted)
        // i.e. avoiding moving _|- into _|_ where the string doubles back on itself
        if(previt->d == d && it->d != d)
          return false;
        lastselected=false;
      }
      // If not selected then no further checks needed as won't move
      continue;
    }
    if(!lastselected){
      lastselected=true;
      // First element of a selection
      // don't allow to move opposite to it's direction unless the previous element points
      // the same direction (i.e. can be deleted)
      // i.e. avoiding moving _|- into _|_ where the string doubles back on itself
      if(it->d == opposite(d) && previt->d != opposite(d))
        return false;

    }
    // We have at least one selected element
    any=true;
    // Check for moving out of bounds
    if(d==UP && it->pos.Y>=s->maze.size().Y-1)
      return false;
    if(d==DOWN && it->pos.Y<=1)
      return false;
    if(d==LEFT && it->pos.X>=s->maze.size().X+5)
      return false;
    if(d==RIGHT && it->pos.X<=-5)
      return false;
    if(d==FORWARD && it->pos.Z>=s->maze.size().Z+5)
      return false;
    if(d==BACK && it->pos.Z<=-5)
      return false;

    // Check if we will hit a wall
    if(it->d!=d && it->d!=opposite(d)){
      Vector wall=it->pos+to_shift_vector(it->d)+to_shift_vector(d);
      Dirn wallDirn=perpendicular(it->d,d);
      if(inCube(wall,Vector(0,0,0),s->maze.size())){
        if(((*s->maze[wall])&to_mask(wallDirn))!=0)
          return false;
      }
    }
  }
  return any;
}

std::pair<int,int> StringPlay::doMoveI(Dirn d){
  int length=0; // needed so we can store the selection state later
  int movescore=0;
  bool lastselected=false;

  //do the move
  for(std::list<StringElement>::iterator it=s->route.begin();it!=s->route.end();++it,++length){
    if(it->selected){
      if(!lastselected){
        // At start of selection so need to ensure the route connects up
        if(it!=s->route.begin()){
          std::list<StringElement>::iterator nit=it;
          --nit;
          if(nit->d==opposite(d)){
            // nit can't be selected as at start of selection
            s->route.erase(nit);
            --length;
          }else{
            s->route.insert(it,StringElement(it->pos,d,false));
            ++length;
          }
        }else if(d==s->stringDir){
          // start of string and dragging in to maze
          s->route.insert(it,StringElement(it->pos,d,false));
          ++length;
        }

      }
      // selected (may or may not be start of selection)
      it->pos+=to_vector(d);
      // only costs for score if element moves not in the direction it points (or the opposite direction)
      if(it->d!=d && it->d!=opposite(d))
        ++movescore;
    }else if(lastselected){
      // just after end of selection
      if(it->d==d){
        it=s->route.erase(it);
        --length;
        // "it" is now the next element so decrement and continue the loop so it gets processed
        lastselected=false;
        --it;
        continue;
      }else{
        s->route.insert(it,StringElement(it->pos+to_vector(d),opposite(d),false));
        ++length;
      }
    }
    lastselected=it->selected;
  }
  // fix up the end
  if(lastselected)
    if(d==opposite(s->stringDir)){
      s->route.insert(s->route.end(),StringElement(s->endPos+to_vector(d),opposite(d),false));
      ++length;
    }else
      s->endPos+=to_vector(d);
  return std::make_pair(movescore,length);
}

void StringPlay::doMove(Dirn d){
  std::pair<int,int> ret=doMoveI(d);
  score+=ret.first;

  // record the selection state to ensure it is correct before undo
  Bitset selection(ret.second);
  int i=0;
  for(std::list<StringElement>::iterator it=s->route.begin();it!=s->route.end();++it,++i)
    if(it->selected)
      selection.set(i);

  // collapse any lines along the edge (or slightly sticking out)
  std::list<Dirn> startcollapsed;
  int out=0;
  while(out!=0 || s->route.front().d!=s->stringDir){
    // checks we end at the same distance as we started at
    if(s->route.front().d == opposite(s->stringDir))
      out++;
    else if(s->route.front().d == s->stringDir)
      out--;
    startcollapsed.push_front(s->route.front().d);
    s->route.pop_front();
  }

  std::list<Dirn> endcollapsed;
  out=0;
  while(out!=0 || s->route.back().d!=s->stringDir){
    // checks we end at the same distance as we started at
    if(s->route.back().d == opposite(s->stringDir))
      out++;
    else if(s->route.back().d == s->stringDir)
      out--;
    endcollapsed.push_front(s->route.back().d);
    s->endPos=s->route.back().pos;
    s->route.pop_back();
  }

  undohistory->pushTop(HistoryElement(ret.second,selection,d,startcollapsed,endcollapsed));
}

bool StringPlay::undo(bool extendedmove){
  if(extendedmove && !inextendedmove)
    return false;
  inextendedmove=extendedmove;
  if(undohistory->empty())
    return false;
  HistoryElement histel=undohistory->popTop();

  // Add the ends back in
  for(std::list<Dirn>::iterator it=histel.startcollapsed.begin();it!=histel.startcollapsed.end();++it){
    s->route.push_front(StringElement(s->route.front().pos-to_vector(*it),*it,false));
  }
  for(std::list<Dirn>::iterator it=histel.endcollapsed.begin();it!=histel.endcollapsed.end();++it){
    s->route.push_back(StringElement(s->endPos,*it,false));
    s->endPos+=to_vector(*it);
  }

  // fix selection (both for end elements that have been added back in and
  // in case the user has changed the selection.
  int i=0;
  for(std::list<StringElement>::iterator it=s->route.begin();it!=s->route.end();++it,++i)
    it->selected=histel.isselected(i);

  // Undo the actual move
  score-=doMoveI(opposite(histel.d)).first;
  return true;
}

bool StringPlay::tryMove(Dirn d,bool extendedmove){
  if(extendedmove && !inextendedmove)
    return false;
  inextendedmove=extendedmove;
  if(canMove(d)){
    doMove(d);
    return true;
  }else{
    return false;
  }
}

StringPlay::~StringPlay(){delete undohistory; };

void StringEdit::setStringSegment(StringPointer sp,StringPointer ep,int count,SPA<Dirn> newRoute){
  std::list<StringElement>::iterator it=sp.el;
  Vector pos=it->pos;
  bool endSel=true;
  if(ep!=s->end())
    endSel=ep.el->selected;
  for(SPA<Dirn> d=newRoute;d<newRoute+count;++it,++d){
    //run out of bits of string to move so add a new one
    if(it==ep.el){
      --it;
      bool sel=it->selected&&endSel;
      ++it;
      it=s->route.insert(it,StringElement(pos,*d,sel));
    }else{
      it->pos=pos;
      it->d=*d;
    }
    pos+=to_vector(*d);
  }
  {
    //connect up to the right distance across
    Dirn d=s->stringDir;
    int dist=to_vector(d).dotProduct((ep==s->end()?s->endPos:ep.el->pos)-pos);
    if(dist<0){
      dist=-dist;
      d=opposite(d);
    }
    for(int i=0;i<dist;++i,++it){
      if(it==ep.el){
        --it;
        bool sel=it->selected&&endSel;
        ++it;
        it=s->route.insert(it,StringElement(pos,d,sel));
      }else{
        it->pos=pos;
        it->d=d;
      }
      pos+=to_vector(d);
    }
  }
  //delete any spares
  while(it!=ep.el)
    it=s->route.erase(it);

  //slide the rest of the string across to line up
  for(it=ep.el;it!=s->route.end();++it){
    it->pos=pos;
    pos+=to_vector(it->d);
  }
  s->endPos=pos;
}

void StringEdit::translateString(StringPointer sp,Vector newpos){
  Vector delta=newpos;
  if(sp==s->end())
    delta-=s->endPos;
  else
    delta-=sp->pos;
  // remove any component of delta in the direction of s->stringDir so we don't shift the ends in/out of the maze
  delta-=delta.dotProduct(to_vector(s->stringDir))*to_vector(s->stringDir);
  for(std::list<StringElement>::iterator it=s->route.begin();it!=s->route.end();++it)
    it->pos+=delta;
  s->endPos+=delta;
}

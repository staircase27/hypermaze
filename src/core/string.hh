#include "vector.hh"
#include "dirns.hh"
#include "maze.hh"
#include <list>

#ifdef IOSTREAM
#include <istream>
#endif
#ifndef STRING_HH_INC
#define STRING_HH_INC

class StringPlay;
class StringEdit;

struct StringElement{
  Vector pos;
  Dirn d;
  bool selected;
  StringElement(Vector pos,Dirn d,bool selected):pos(pos),d(d),selected(selected){};
};
class StringPointer{
  private:
    std::list<StringElement>::iterator el;
  public:

    const StringElement& operator *()const{
      return *el;
    }
    const StringElement* operator ->() const{
      return el.operator->();
    }

    StringPointer& operator++(){
      ++el;
      return *this;
    }
    StringPointer& operator--(){
      --el;
      return *this;
    }

    bool operator!=(const StringPointer& other) const{
      return el!=other.el;
    }
    bool operator==(const StringPointer& other) const{
      return el==other.el;
    }

    StringPointer& operator=(StringPointer other){
      el=other.el;
      return *this;
    }

    StringPointer(std::list<StringElement>::iterator el):el(el){};

    friend class StringPlay;
    friend class StringEdit;
};
class ConstStringPointer{
  private:
    std::list<StringElement>::const_iterator el;
  public:

    const StringElement& operator *()const{
      return *el;
    }
    const StringElement* operator ->()const{
      return el.operator->();
    }

    ConstStringPointer& operator++(){
      ++el;
      return *this;
    }
    ConstStringPointer& operator--(){
      --el;
      return *this;
    }

    bool operator!=(const ConstStringPointer& other) const{
      return el!=other.el;
    }
    bool operator==(const ConstStringPointer& other) const{
      return el==other.el;
    }

    ConstStringPointer& operator=(const ConstStringPointer& other){
      el=other.el;
      return *this;
    }

    ConstStringPointer(std::list<StringElement>::const_iterator el):el(el){};
};

#ifdef IOSTREAM
inline std::ostream& operator<<(std::ostream& o,const StringElement& e){
  return o<<"<StringElement "<<e.pos<<" "<<e.d<<" "<<e.selected<<">";
}
inline std::ostream& operator<<(std::ostream& o,const StringPointer& e){
  return o<<"<Pointer to "<<*e<<">";
}
inline std::ostream& operator<<(std::ostream& o,const ConstStringPointer& e){
  return o<<"<Pointer to "<<*e<<">";
}
#endif
class String{
    std::list<StringElement> route;
    Vector endPos;
  public:
    Maze maze;
    const Dirn stringDir;
    const Dirn targetDir;

    String(Maze m):maze(m),endPos(0,0,0),route(),stringDir(LEFT),targetDir(FORWARD){
      Vector start=m.size.dotProduct(to_shift_vector(stringDir))*to_shift_vector(stringDir)+
          m.size.dotProduct(to_shift_vector(targetDir))*to_shift_vector(targetDir)+
          m.size.dotProduct(to_vector(perpendicular(stringDir,targetDir)))/2*to_vector(perpendicular(stringDir,targetDir));
      Vector pos=start;
      while(pos.dotProduct(to_vector(stringDir))!=(start+m.size).dotProduct(to_vector(stringDir))){
        route.push_back(StringElement(pos,stringDir,true));
        pos+=to_vector(stringDir);
      }
      endPos=pos;
    };

    const Vector& getStart() const{
      return route.front().pos;
    }
    const Vector& getEnd() const{
      return endPos;
    }

    const std::list<StringElement>& getRoute() const{
      return route;
    }

    StringPointer begin(){
      return StringPointer(route.begin());
    }
    StringPointer end(){
      return StringPointer(route.end());
    }
    ConstStringPointer begin() const{
      return ConstStringPointer(route.begin());
    }
    ConstStringPointer end() const{
      return ConstStringPointer(route.end());
    }

    int length() const{
      return route.size();
    }

    bool hasWon() const{
      Vector d=to_vector(targetDir);
      int t=maze.size.dotProduct(-to_shift_vector(opposite(targetDir)));
      if(endPos.dotProduct(d)<t)
        return false;
      for(std::list<StringElement>::const_iterator it=route.begin();it!=route.end();++it)
        if(it->pos.dotProduct(d)<t)
          return false;
      return true;
    }

    String& operator=(const String& o){
      endPos=o.endPos;
      route=o.route;
      maze=o.maze;
      return *this;
    }

    friend class StringPlay;
    friend class StringEdit;
    #ifdef IOSTREAM
    friend std::ostream& operator <<(std::ostream& o,String s);
    friend std::ostream& operator <<(std::ostream& o,StringPlay s);
    #endif
};

template <class T>
class LimitedStack{
  private:
    SPA<T> start;
    SPA<T> end;
    SPA<T> top;
    SPA<T> bottom;

  public:

    LimitedStack(int len):start(len),end(start+len-1),top(start),bottom(start){};

    bool empty(){
      return top==bottom;
    }

    inline const T& getTop(){
      return *top;
    }

    const T& popTop(){
      const T& tmp=*top;
      if(top==start)
        top=end;
      else
        --top;
      return tmp;
    }

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

    void clear(){
      top=bottom=start;
    }
};

struct HistoryElement{
  int length;
  SPA<unsigned char> selected;
  bool isselected(int i){
    return (selected[i/CHAR_BIT]&(1<<(i%CHAR_BIT)))!=0;
  }
  Dirn d;
  std::list<Dirn> startcollapsed;
  std::list<Dirn> endcollapsed;
  HistoryElement(int length,SPA<unsigned char> selected,Dirn d,std::list<Dirn> startcollapsed,std::list<Dirn> endcollapsed):
      length(length),selected(selected),d(d),startcollapsed(startcollapsed),endcollapsed(endcollapsed){};
  HistoryElement():length(0),selected(),d(),startcollapsed(),endcollapsed(){};
};

class StringPlay{
  String& s;
  int score;

  LimitedStack<HistoryElement> undohistory;
  bool inextendedmove;

  public:
    StringPlay(String& s):s(s),score(0),undohistory(10+2*s.maze.size.dotProduct(Vector(1,1,1))),inextendedmove(false){};

    String& getString(){
      return s;
    }

    int getScore(){
      return score;
    }

    void startExtendedMove(){
      inextendedmove=true;
    }

    void externalEditHappened(){
      inextendedmove=false;
      undohistory.clear();
    }

    bool slide(bool moveEnd,bool out){
      if(moveEnd){
        std::list<StringElement>::reverse_iterator it;
        for(it=s.route.rbegin();it!=s.route.rend();++it)
          if(it->selected)
            break;
        if(out){
          if(it==s.route.rbegin())
            return false;
          --it;
          it->selected=true;
        }else{
          if(it==s.route.rend())
            return false;
          it->selected=false;
        }
      }else{
        std::list<StringElement>::iterator it;
        for(it=s.route.begin();it!=s.route.end();++it)
          if(it->selected)
            break;
        if(out){
          if(it==s.route.begin())
            return false;
          --it;
          it->selected=true;
        }else{
          if(it==s.route.end())
            return false;
          it->selected=false;
        }
      }
      inextendedmove=false;
      return true;
    }

    void setSelected(StringPointer p,bool selected){
      inextendedmove=false;
      p.el->selected=selected;
    }

    bool canMove(Dirn d){
      bool any=false;
      if((d==opposite(s.stringDir)&&s.route.front().selected)||(d==s.stringDir&&s.route.back().selected))
        return false;
      for(std::list<StringElement>::iterator it=s.route.begin();it!=s.route.end();++it){
        if(!it->selected)
          continue;
        any=true;
        if(d==UP && it->pos.Y>=s.maze.size.Y-1)
          return false;
        if(d==DOWN && it->pos.Y<=1)
          return false;
        if(d==LEFT && it->pos.X>=s.maze.size.X+5)
          return false;
        if(d==RIGHT && it->pos.X<=-5)
          return false;
        if(d==FORWARD && it->pos.Z>=s.maze.size.Z+5)
          return false;
        if(d==BACK && it->pos.Z<=-5)
          return false;
        if(it->d!=d && it->d!=opposite(d)){
          Vector wall=it->pos+to_shift_vector(it->d)+to_shift_vector(d);
          Dirn wallDirn=perpendicular(it->d,d);
          if(inCube(wall,Vector(0,0,0),s.maze.size)){
            if(((*s.maze[wall])&to_mask(wallDirn))!=0)
              return false;
          }
        }
      }
      return any;
    }

  private:
    void doMove(Dirn d,bool undo){
      int length=0;
      int movescore=0;
      bool lastselected=false;
      for(std::list<StringElement>::iterator it=s.route.begin();it!=s.route.end();++it){
        ++length;
        if(it->selected){
          if(!lastselected){
            if(it!=s.route.begin()){
              std::list<StringElement>::iterator nit=it;
              --nit;
              if(nit->d==opposite(d)){
                s.route.erase(nit);
                --length;
              }else{
                s.route.insert(it,StringElement(it->pos,d,false));
                ++length;
              }
            }else if(d==s.stringDir||d==opposite(s.stringDir))
              s.route.insert(it,StringElement(it->pos,d,false));
          }
          it->pos+=to_vector(d);
          if(it->d!=d && it->d!=opposite(d))
              movescore+=1;
        }else if(lastselected){
          if(it->d==d){
            it=s.route.erase(it);
            --length;
            if(it==s.route.end()){
              lastselected=false;
              break;
            }
          }else{
            s.route.insert(it,StringElement(it->pos+to_vector(d),opposite(d),false));
            ++length;
          }
        }
        lastselected=it->selected;
      }

      if(lastselected)
        if(d==s.stringDir||d==opposite(s.stringDir))
          s.route.insert(s.route.end(),StringElement(s.endPos+to_vector(d),opposite(d),false));
        else
          s.endPos+=to_vector(d);
      if(!undo){
        score+=movescore;
        SPA<unsigned char> selection((length+CHAR_BIT-1)/CHAR_BIT);
        int i=0;
        for(std::list<StringElement>::iterator it=s.route.begin();it!=s.route.end();++it,++i)
          if(it->selected)
            selection[i/CHAR_BIT]|=(1<<(i%CHAR_BIT));
        std::list<Dirn> startcollapsed;
        while(s.route.front().d!=s.stringDir){
          startcollapsed.push_back(s.route.front().d);
          s.route.pop_front();
        }
        std::list<Dirn> endcollapsed;
        while(s.route.back().d!=s.stringDir){
          endcollapsed.push_back(s.route.back().d);
          s.endPos=s.route.back().pos;
          s.route.pop_back();
        }
        undohistory.pushTop(HistoryElement(length,selection,d,startcollapsed,endcollapsed));
      }else
        score-=movescore;
    }

  public:
    bool undo(bool extendedmove=false){
      if(extendedmove && !inextendedmove)
        return false;
      inextendedmove=extendedmove;
      if(undohistory.empty())
        return false;
      HistoryElement el=undohistory.popTop();
      for(std::list<Dirn>::iterator it=el.startcollapsed.begin();it!=el.startcollapsed.end();++it){
        s.route.push_front(StringElement(s.route.front().pos-to_vector(*it),*it,false));
      }
      for(std::list<Dirn>::iterator it=el.endcollapsed.begin();it!=el.endcollapsed.end();++it){
        s.route.push_back(StringElement(s.endPos,*it,false));
        s.endPos+=to_vector(*it);
      }
      int i=0;
      for(std::list<StringElement>::iterator it=s.route.begin();it!=s.route.end();++it,++i)
        it->selected=el.isselected(i);
      doMove(opposite(el.d),true);
      return true;
    }

    bool tryMove(Dirn d,bool extendedmove=false){
      if(extendedmove && !inextendedmove)
        return false;
      inextendedmove=extendedmove;
      if(canMove(d)){
        doMove(d,false);
        return true;
      }else{
        return false;
      }
    }
    #ifdef IOSTREAM
    friend std::ostream& operator <<(std::ostream& o,StringPlay s);
    #endif

    StringPlay& operator=(const StringPlay& o){
      s=o.s;
      return *this;
    }

};

class StringEdit{
  String& s;

  public:
    StringEdit(String& s):s(s){};

    String& getString(){
      return s;
    }

    void setSelected(StringPointer p,bool selected){
      p.el->selected=selected;
    }
    void setStringSegment(StringPointer sp,StringPointer ep,int count,SPA<Dirn> newRoute){
      std::list<StringElement>::iterator it=sp.el;
      Vector pos=it->pos;
      bool endSel=true;
      if(ep!=s.end())
        endSel=ep.el->selected;
      for(SPA<Dirn> d=newRoute;d<newRoute+count;++it,++d){
        //run out of bits of string to move so add a new one
        if(it==ep.el){
          --it;
          bool sel=it->selected&&endSel;
          ++it;
          it=s.route.insert(it,StringElement(pos,*d,sel));
        }else{
          it->pos=pos;
          it->d=*d;
        }
        pos+=to_vector(*d);
      }
      {
		    //connect up to the right distance across
		    Dirn d=s.stringDir;
		    int dist=to_vector(d).dotProduct((ep==s.end()?s.endPos:ep.el->pos)-pos);
		    if(dist<0){
		      dist=-dist;
		      d=opposite(d);
		    }
		    for(int i=0;i<dist;++i,++it){
		      if(it==ep.el){
		        --it;
		        bool sel=it->selected&&endSel;
		        ++it;
		        it=s.route.insert(it,StringElement(pos,d,sel));
		      }else{
		        it->pos=pos;
		        it->d=d;
		      }
		      pos+=to_vector(d);
		    }
      }
      //delete any spares
      while(it!=ep.el)
	      it=s.route.erase(it);

      //slide the rest of the string across to line up
      for(it=ep.el;it!=s.route.end();++it){
        it->pos=pos;
        pos+=to_vector(it->d);
      }
      s.endPos=pos;
    }
};

#ifdef IOSTREAM
inline std::ostream& operator<<(std::ostream& o,String s){
  o<<"<String ";
  for(std::list<StringElement>::iterator it=s.route.begin();it!=s.route.end();++it)
    o<<it->pos<<"-"<<(it->selected?"":"*")<<it->d<<(it->selected?"":"*")<<"-";
  return o<<s.endPos<<">";
}

inline std::ostream& operator<<(std::ostream& o,StringPlay s){
  o<<"<StringPlay ";
  for(std::list<StringElement>::iterator it=s.s.route.begin();it!=s.s.route.end();++it)
    o<<it->pos<<"-"<<(it->selected?"":"*")<<it->d<<(it->selected?"":"*")<<"-";
  return o<<s.s.endPos<<">";
}
#endif

#endif

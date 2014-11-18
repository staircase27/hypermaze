#include "maze.hh"
#include "dirns.hh"
#include <list>
#ifdef IOSTREAM
#include <istream>
#endif
#ifndef STRING_HH_INC
#define STRING_HH_INC
using namespace std;

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
    list<StringElement>::iterator el;
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
    
    StringPointer(list<StringElement>::iterator el):el(el){};
    
    friend class StringPlay;
    friend class StringEdit;
};
class ConstStringPointer{
  private:
    list<StringElement>::const_iterator el;
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
    
    ConstStringPointer(list<StringElement>::const_iterator el):el(el){};
};

#ifdef IOSTREAM
inline ostream& operator<<(ostream& o,const StringElement& e){
  return o<<"<StringElement "<<e.pos<<" "<<e.d<<" "<<e.selected<<">";
}
inline ostream& operator<<(ostream& o,const StringPointer& e){
  return o<<"<Pointer to "<<*e<<">";
}
inline ostream& operator<<(ostream& o,const ConstStringPointer& e){
  return o<<"<Pointer to "<<*e<<">";
}
#endif
class String{
    list<StringElement> route;
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
    
    const list<StringElement>& getRoute() const{
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
      for(list<StringElement>::const_iterator it=route.begin();it!=route.end();++it)
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
    friend ostream& operator <<(ostream& o,String s);
    friend ostream& operator <<(ostream& o,StringPlay s);
    #endif
};

class StringPlay{
  String& s;
  int score;
 
  public:
    StringPlay(String& s):s(s),score(0){};
  
    String& getString(){
      return s;
    }
    
    int getScore(){
      return score;
    }
    
    bool slide(bool moveEnd,bool out){
      if(moveEnd){
        list<StringElement>::reverse_iterator it;
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
        list<StringElement>::iterator it;
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
      return true;
    }
    
    void setSelected(StringPointer p,bool selected){
      p.el->selected=selected;
    }
    
    bool canMove(Dirn d){
      bool any=false;
      if((d==s.stringDir||d==opposite(s.stringDir))&&(s.route.front().selected||s.route.back().selected))
        return false;
      for(list<StringElement>::iterator it=s.route.begin();it!=s.route.end();++it){
        if(!it->selected)
          continue;
        any=true;
        if(d==UP && it->pos.Y>=s.maze.size.Y-1)
          return false;
        if(d==DOWN && it->pos.Y<=1)
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
    
    void doMove(Dirn d){
      bool lastselected=false;
      for(list<StringElement>::iterator it=s.route.begin();it!=s.route.end();++it){
        if(it->selected){
          if(!lastselected){
            if(it!=s.route.begin()){
              list<StringElement>::iterator nit=it;
              --nit;
              if(nit->d==opposite(d))
                s.route.erase(nit);
              else
                s.route.insert(it,StringElement(it->pos,d,false));
            }
          }
          it->pos+=to_vector(d);
          if(it->d!=d && it->d!=opposite(d))
              score+=1;
        }else if(lastselected){
          if(it==s.route.end()){
            s.endPos+=to_vector(d);
          }else{
            if(it->d==d){
              it=s.route.erase(it);
              if(it==s.route.end()){
                lastselected=false;
                break;
              }
            }else{
              s.route.insert(it,StringElement(it->pos+to_vector(d),opposite(d),false));
            }
          }
        }
        lastselected=it->selected;
      }
      if(lastselected)
        s.endPos+=to_vector(d);
    }
    
    bool tryMove(Dirn d){
      if(canMove(d)){
        doMove(d);
        return true;
      }else{
        return false;
      }
    }
    #ifdef IOSTREAM
    friend ostream& operator <<(ostream& o,StringPlay s);
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
      list<StringElement>::iterator it=sp.el;
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
inline ostream& operator<<(ostream& o,String s){
  o<<"<String ";
  for(list<StringElement>::iterator it=s.route.begin();it!=s.route.end();++it)
    cout<<it->pos<<"-"<<(it->selected?"":"*")<<it->d<<(it->selected?"":"*")<<"-";
  return cout<<s.endPos<<">";
}

inline ostream& operator<<(ostream& o,StringPlay s){
  o<<"<StringPlay ";
  for(list<StringElement>::iterator it=s.s.route.begin();it!=s.s.route.end();++it)
    cout<<it->pos<<"-"<<(it->selected?"":"*")<<it->d<<(it->selected?"":"*")<<"-";
  return cout<<s.s.endPos<<">";
}
#endif

#endif

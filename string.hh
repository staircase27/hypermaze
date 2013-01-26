#include "maze.hh"
#include "dirns.hh"
#include <list>
#ifdef IOSTREAM
#include <istream>
#endif
#ifndef STRING_HH_INC
#define STRING_HH_INC
using namespace std;

class StringSlice;

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
    
    const StringElement& operator *(){
      return *el;
    }
    const StringElement* operator ->(){
      return &*el;
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
    
    friend class StringSlice;
};

#ifdef IOSTREAM
inline ostream& operator<<(ostream& o,const StringElement& e){
  return o<<"<StringElement "<<e.pos<<" "<<e.d<<" "<<e.selected<<">";
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
    
    int length(){
      return route.size();
    }
    
    bool hasWon(){
      Vector d=to_vector(targetDir);
      int t=maze.size.dotProduct(-to_shift_vector(opposite(targetDir)));
      if(endPos.dotProduct(d)<t)
        return false;
      for(list<StringElement>::iterator it=route.begin();it!=route.end();++it)
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
    
    friend class StringSlice;
    #ifdef IOSTREAM
    friend ostream& operator <<(ostream& o,String s);
    friend ostream& operator <<(ostream& o,StringSlice s);
    #endif
};

class StringSlice{
  String* s;

  public:
    StringSlice(String& s):s(&s){};
  
    String& getString(){
      return *s;
    }
    
    bool slide(bool moveEnd,bool out){
      if(moveEnd){
        list<StringElement>::reverse_iterator it;
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
        list<StringElement>::iterator it;
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
      return true;
    }
    
    void setSelected(StringPointer p,bool selected){
      p.el->selected=selected;
    }
    
    bool canMove(Dirn d){
      bool any=false;
      if((d==LEFT||d==opposite(LEFT))&&(s->route.front().selected||s->route.back().selected))
        return false;
      for(list<StringElement>::iterator it=s->route.begin();it!=s->route.end();++it){
        if(!it->selected)
          continue;
        any=true;
        if(d==UP && it->pos.Y>=s->maze.size.Y-1)
          return false;
        if(d==DOWN && it->pos.Y<=1)
          return false;
        if(it->d!=d && it->d!=opposite(d)){
          Vector wall=it->pos+to_shift_vector(it->d)+to_shift_vector(d);
          Dirn wallDirn=perpendicular(it->d,d);
          if(inCube(wall,Vector(0,0,0),s->maze.size)){
            if(((*s->maze[wall])&to_mask(wallDirn))!=0)
              return false;
          }
        }
      }
      return any;
    }
    
    void doMove(Dirn d){
      bool lastselected=false;
      for(list<StringElement>::iterator it=s->route.begin();it!=s->route.end();++it){
        if(it->selected){
          if(!lastselected){
            if(it!=s->route.begin()){
              list<StringElement>::iterator nit=it;
              --nit;
              if(nit->d==opposite(d))
                s->route.erase(nit);
              else
                s->route.insert(it,StringElement(it->pos,d,false));
            }
          }
          it->pos+=to_vector(d);
        }else if(lastselected){
          if(it==s->route.end()){
            s->endPos+=to_vector(d);
          }else{
            if(it->d==d){
              it=s->route.erase(it);
              if(it==s->route.end()){
                lastselected=false;
                break;
              }
            }else{
              s->route.insert(it,StringElement(it->pos+to_vector(d),opposite(d),false));
            }
          }
        }
        lastselected=it->selected;
      }
      if(lastselected)
        s->endPos+=to_vector(d);
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
    friend ostream& operator <<(ostream& o,StringSlice s);
    #endif

    StringSlice& operator=(const StringSlice& o){
      s=o.s;
      return *this;
    }

};

#ifdef IOSTREAM
inline ostream& operator<<(ostream& o,String s){
  o<<"<String ";
  for(list<StringElement>::iterator it=s.route.begin();it!=s.route.end();++it)
    cout<<it->pos<<"-"<<it->d<<"-";
  return cout<<s.endPos<<">";
}


inline ostream& operator<<(ostream& o,StringSlice s){
  o<<"<StringSlice ";
  for(list<StringElement>::iterator it=s.s.route.begin();it!=s.s.route.end();++it)
    cout<<it->pos<<"-"<<(it->selected?"":"*")<<it->d<<(it->selected?"":"*")<<"-";
  return cout<<s.s.endPos<<">";
}
#endif

#endif

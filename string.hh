#include "maze.hh"
#include "dirns.hh"
#include <list>
#include <istream>
#ifndef STRING_HH_INC
#define STRING_HH_INC
using namespace std;

class StringSlice;

ostream& operator<<(ostream& o,Dirn d){
  return o<<"<Dirn "<<(int)d<<": "<<to_vector(d)<<">";
}
class String{
  const Maze& maze;
  Vector start;
  Vector end;
  list<Dirn> route;
  
  public:
    String(const Maze& m):maze(m),start(Vector(0,2,0)),end(Vector(m.size.X,2,0)),route(m.size.X,LEFT){};
    
    friend class StringSlice;
    friend ostream& operator <<(ostream& o,String s);
};

ostream& operator<<(ostream& o,String s){
  o<<"<String ";
  Vector p=s.start;
  for(list<Dirn>::iterator it=s.route.begin();it!=s.route.end();++it){
    cout<<p<<"-"<<*it<<"-";
    p+=to_vector(*it);
  }
  return cout<<p<<"="<<s.end<<">";
}



class StringSlice{
  String& s;
  list<Dirn>::iterator start;
  list<Dirn>::iterator end;
  Vector startPoint;
  Vector endPoint;

  public:
    StringSlice(String& s):s(s),start(s.route.begin()),end(s.route.end()),startPoint(s.start),endPoint(s.end){};
  
    bool slide(bool moveEnd,bool towardsEnd){
      if(moveEnd)
        if(towardsEnd){
          if(end==s.route.end())
            return false;
          endPoint+=to_vector(*end);
          ++end;
        }else{
          if(end==start)
            return false;
          --end;
          endPoint-=to_vector(*end);
        }
      else
        if(towardsEnd){
          if(end==start)
            return false;
          startPoint+=to_vector(*start);
          ++start;
        }else{
          if(start==s.route.begin())
            return false;
          --start;
          startPoint-=to_vector(*start);
        }
      return true;
    }
    
    bool canMove(Dirn d){
      cout<<"can move in "<<d<<endl;
      Vector point=startPoint;
      list<Dirn>::iterator it=start;
      while(it!=end){
        if(*it!=d){
          Vector wall=point+to_shift_vector(*it)+to_shift_vector(d);
          Dirn wallDirn=perpendicular(*it,d);
          cout<<"check point"<<point<<" "<<*it<<" wall is "<<wall<<"-"<<wallDirn;
          if(inCube(wall,Vector(0,0,0),s.maze.size)){
            cout<<" and is "<<*s.maze[wall]<<" "<<to_mask(wallDirn)<<endl;
            if(((*s.maze[wall])&to_mask(wallDirn))!=0)
              return false;
          }else
            cout<<endl;
        }
        point+=to_vector(*it);
        ++it;
      }
      return true;
    }
    
    void doMove(Dirn d){
      startPoint+=to_vector(d);
      if(start==s.route.begin()){
        s.start=startPoint;
      }else{
        list<Dirn>::iterator it=start;
        --it;
        if(*it==opposite(d))
          s.route.erase(it);
        else
          s.route.insert(start,d);
      }
      endPoint+=to_vector(d);
      if(end==s.route.end()){
        s.end=endPoint;
      }else{
        if(*end==d){
          end=s.route.erase(end);
        }else{
          s.route.insert(end,opposite(d));
          --end;
        }
      }
    }
    
    bool tryMove(Dirn d){
      if(canMove(d)){
        doMove(d);
        return true;
      }else{
        return false;
      }
    }
    friend ostream& operator <<(ostream& o,StringSlice s);
};

ostream& operator<<(ostream& o,StringSlice s){
  o<<"<StringSlice ";
  Vector p=s.startPoint;
  for(list<Dirn>::iterator it=s.start;it!=s.end;++it){
    cout<<p<<"-"<<*it<<"-";
    p+=to_vector(*it);
  }
  return cout<<p<<"="<<s.endPoint<<">";
}


#endif

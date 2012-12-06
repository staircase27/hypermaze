#include "maze.hh"
#include "dirns.hh"
#include "vector.hh"
#include <set>
#include <cstdlib>

using namespace std;

#ifndef MAZEGEN_HH_INC
#define MAZEGEN_HH_INC

class MazeGenHalf{
  Maze& m;
  Vector p;
  bool down;
  Vector huntStart;
  Vector huntEnd;
  int mask;
  public:
    MazeGenHalf(Maze& m,bool down):m(m),p(-1,-1,-1),down(down),
        huntStart(0,0,0),huntEnd(m.size.X-1,m.size.Y-1,m.size.Z-1),
        mask(1<<10){
      if(down){
        huntStart=huntEnd;
        huntEnd=Vector(0,0,0);
        mask=mask<<1;
      }
      if(down){
        for(int x=0;x<m.size.X;++x)
          for(int z=0;z<m.size.Z;++z)
            *m[Vector(x,m.size.Y-1,z)]|=mask;
      }else{
        for(int x=0;x<m.size.X;++x)
          for(int z=0;z<m.size.Z;++z)
            *m[Vector(x,0,z)]|=mask;
      }
    };
    
    bool walk(){
      if(p.X==-1)
        return false;
      set<Dirn> available;
      for(int i=0;i<6;i++){
        Dirn d=from_id(i);
        if(inCube(p+to_vector(d),Vector(0,0,0),m.size)&&*m[p+to_vector(d)]==0)
          available.insert(d);
      }
      if(available.empty())
        return false;
      set<Dirn>::iterator dirn = available.begin();
      advance(dirn, rand() % available.size());
      #ifdef DEBUG
      cout<<"    walk in "<<*dirn<<" to "<<p+to_vector(*dirn)<<endl;
      #endif
      *m[p]|=to_mask(*dirn);
      p=p+to_vector(*dirn);
      *m[p]|=to_mask(opposite(*dirn))|mask;
      return true;
    }
    
    bool hunt(){
      #ifdef DEBUG
      cout<<"    hunt"<<endl;
      #endif
      while(huntStart!=huntEnd&&*m[huntStart]!=0){
        moveVector(huntStart);
        #ifdef DEBUG
        cout<<"    move stat"<<huntStart<<*m[huntStart]<<endl;
        #endif
      }
      p=huntStart;
      while(p!=huntEnd){
        #ifdef DEBUG
        cout<<"    move"<<p<<*m[p]<<endl;
        #endif
        if(*m[p]==0){
          set<Dirn> available;
          for(int i=0;i<6;i++){
            Dirn d=from_id(i);
            if(inCube(p+to_vector(d),Vector(0,0,0),m.size)&&((*m[p+to_vector(d)]&mask)!=0))
              available.insert(d);
          }
          if(!available.empty()){
            set<Dirn>::iterator dirn = available.begin();
            advance(dirn, rand() % available.size());
            *m[p]|=to_mask(*dirn)|mask;
            *m[p+to_vector(*dirn)]|=to_mask(opposite(*dirn));
            return true;
          }
        }
        moveVector(p);
      }
      huntStart=huntEnd;
      p.X=-1;
      return false;
    }
    
    void moveVector(Vector& v){
      v.Y+=down?-1:1;
      if(v.Y==m.size.Y){
        v.Y=0;
        v.Z++;
        if(v.Z==m.size.Z){
          v.Z=0;
          v.X++;
        }
      }else if(v.Y<0){
        v.Y=m.size.Y-1;
        v.Z--;
        if(v.Z<0){
          v.Z=m.size.Z-1;
          v.X--;
        }
      }
    }
        
    
    bool doStep(){
      #ifdef DEBUG
      cout<<"step "<<down<<endl;
      #endif
      if(!walk())
        if(!hunt())
          return true;
      return false;
    }

  friend Maze generate(Vector);
};


Maze generate(Vector size){
  #ifdef DEBUG
  cout<<"gen"<<endl;
  #endif
  Maze m(size);
  MazeGenHalf down(m,true);
  MazeGenHalf up(m,false);
  while(!(down.doStep()&up.doStep())){
    #ifdef DEBUG
    cout<<true<<" state "<<down.p<<" "<<down.huntStart<<" "<<down.huntEnd<<endl;
    cout<<false<<" state "<<up.p<<" "<<up.huntStart<<" "<<up.huntEnd<<endl;
    #endif
  };
  return m;
};

void solve(Maze& m){
  set<Dirn> dirns;
  dirns.insert(UP);
  dirns.insert(DOWN);
  dirns.insert(LEFT);
  dirns.insert(RIGHT);
  dirns.insert(FORWARD);
  dirns.insert(BACK);
  int mask=(1|(1<<1))<<10;
  for(int z=0;z<m.size.Z;++z)
    for(int y=m.size.Y-1;y>=0;--y)
        for(int x=m.size.X-1;x>=0;--x)
          for(set<Dirn>::iterator d=dirns.begin();d!=dirns.end();++d)
            if(inCube(Vector(x,y,z)+to_vector(*d),Vector(0,0,0),m.size)&&(*m[Vector(x,y,z)]&mask)==(*m[Vector(x,y,z)+to_vector(*d)]&mask))
              *m[Vector(x,y,z)]|=to_mask(*d);
}
#endif

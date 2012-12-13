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
  
  int* xtrans;
  int* ytrans;
  int* ztrans;
  int* xinvtrans;
  int* yinvtrans;
  int* zinvtrans;
  
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
      
      
      xtrans=new int[m.size.X];
      xinvtrans=new int[m.size.X];
      for(int i=0;i<(m.size.X+1)/2;++i){
        if(i%2==0){
          xtrans[i]=i;
          xtrans[m.size.X-1-i]=m.size.X-1-i;
        }else{
          xtrans[i]=m.size.X-1-i;
          xtrans[m.size.X-1-i]=i;
        }
        xinvtrans[xtrans[i]]=i;
        xinvtrans[xtrans[m.size.X-1-i]]=m.size.X-1-i;
      }
      
      makeTrans(m.size.X,xtrans,xinvtrans);
      makeTrans(m.size.Y,ytrans,yinvtrans);
      makeTrans(m.size.Z,ztrans,zinvtrans);

      huntStart.X=xtrans[huntStart.X];
      huntStart.Y=ytrans[huntStart.Y];
      huntStart.Z=ztrans[huntStart.Z];
      huntEnd.X=xtrans[huntEnd.X];
      huntEnd.Y=ytrans[huntEnd.Y];
      huntEnd.Z=ztrans[huntEnd.Z];
    };
    
    virtual void makeTrans(int size,int* &trans,int* &invtrans){
      trans=new int[size];
      invtrans=new int[size];
    
      for(int i=0;i<size;++i)
        invtrans[i]=-1;
      for(int i=0;i<size;++i){
        int j=rand()%(size-i);
        int k=-1;
        while(j>=0){
          ++k;
          --j;
          while(invtrans[k]!=-1){
           ++k;
          }
        }
        cout<<i<<"->"<<k<<",";
        trans[i]=k;
        invtrans[k]=i;
      }
      cout<<endl;
      /*    
      for(int i=0;i<(size+1)/2;++i){
        if(i%2==0){
          trans[i]=i;
          trans[size-1-i]=size-1-i;
        }else{
          trans[i]=size-1-i;
          trans[size-1-i]=i;
        }
        invtrans[trans[i]]=i;
        invtrans[trans[size-1-i]]=size-1-i;
      }      
      */
      /*
      for(int i=0;i<size;++i){
        if(i%2==0)
          trans[i]=i/2;
        else
          trans[i]=size-(i+1)/2;
        invtrans[trans[i]]=i;
      }
      */
    }
    
    
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
      while(huntStart!=huntEnd&&*m[huntStart]!=0){
        moveVector(huntStart);
      }
      p=huntStart;
      while(p!=huntEnd){
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
      v.Y=yinvtrans[v.Y];
      v.Y+=down?-1:1;
      if(v.Y==m.size.Y){
        v.Y=0;
        v.Z=zinvtrans[v.Z];
        v.Z++;
        if(v.Z==m.size.Z){
          v.Z=0;
          v.X=xinvtrans[v.X];
          v.X++;
          v.X=xtrans[v.X];
        }
        v.Z=ztrans[v.Z];
      }else if(v.Y<0){
        v.Y=m.size.Y-1;
        v.Z=zinvtrans[v.Z];
        v.Z--;
        if(v.Z<0){
          v.Z=m.size.Z-1;
          v.X=xinvtrans[v.X];
          v.X--;
          v.X=xtrans[v.X];
        }
        v.Z=ztrans[v.Z];
      }
      v.Y=ytrans[v.Y];
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

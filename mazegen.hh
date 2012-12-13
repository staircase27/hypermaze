#include "maze.hh"
#include "dirns.hh"
#include "vector.hh"
#include <set>
#include <cstdlib>

using namespace std;

#define DEBUG

#ifndef MAZEGEN_HH_INC
#define MAZEGEN_HH_INC

class Walker{
  protected:
    Maze& m;
  public:
    Walker(Maze& m):m(m){};
    
    virtual void init(){};
    
    virtual Vector getStart(){
      return Vector(0,0,0);
    }
    virtual Vector getEnd(){
      return Vector(m.size.X-1,m.size.Y-1,m.size.Z-1);
    }
    
    virtual void moveVector(Vector& v,bool forward){
      v.Z+=forward?-1:1;
      if(v.Z==m.size.Z){
        v.Z=0;
        v.X++;
        if(v.X==m.size.X){
          v.X=0;
          v.Y++;
        }
      }else if(v.Z<0){
        v.Z=m.size.Z-1;
        v.X--;
        if(v.X<0){
          v.X=m.size.X-1;
          v.Y--;
        }
      }
    }
  template <class MGH>
  friend Maze generate(Vector size);
};
  
template <class W>
class Hunter{
  protected:  
    Maze& m;
    int& mask;
    bool down;
    Vector& p;
    Walker* w;
    Vector huntStart;
    Vector huntEnd;
  public:
    Hunter(Maze& m,Vector& p,bool down,int& mask):m(m),mask(mask),down(down),
        p(p),w(new W(m)),huntStart(w->getStart()),huntEnd(w->getEnd()){
      if(down){
        Vector tmp=huntStart;
        huntStart=huntEnd;
        huntEnd=tmp;
      }
    }
    virtual bool hunt(){
      return doHunt(huntStart,huntEnd,down);
    }
    virtual void init(){
      w->init();
    };
  protected:
    virtual bool doHunt(Vector huntStart,Vector huntEnd,bool down){
      while(huntStart!=huntEnd&&*m[huntStart]!=0){
        w->moveVector(huntStart,down);
      }
      
      for(p=huntStart;p!=huntEnd;w->moveVector(p,down)){
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
      }
      huntStart=huntEnd;
      p.X=-1;
      return false;
    }
    
  template <class MGH>
  friend Maze generate(Vector size);
};

template <class H,class W>
class MazeGenHalf{
  protected:
    Maze& m;
    int mask;
    Vector p;

    Hunter<W>* h;
  public:
    MazeGenHalf(Maze& m,bool down):m(m),p(-2,-2,-2),mask(1<<10),h(new H(m,p,down,mask)){
      if(down)
        mask=mask<<1;
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
    
    virtual void init(){
      h->init();
    };
    bool walk(){
      if(p.X<0)
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
    
    virtual bool forceHunt(){
      return false;
    }
    bool doStep(){
      if(p.X==-2)
        init();
      #ifdef DEBUG
      cout<<"step "<<mask<<endl;
      #endif
      if(forceHunt()||!walk()){
        cout<<"hunting"<<endl;
        if(!h->hunt()){
          cout<<"hunting failed"<<endl;
          return true;
        }
      }
      return false;
    }

  template <class MGH>
  friend Maze generate(Vector size);
};

  

class ReorderWalker:public Walker{
  protected:
    virtual void translate(Vector& v){
      if(v.X%2==0)
        v.X=v.X/2;
      else
        v.X=m.size.X-(v.X+1)/2;
      if(v.Y%2==0)
        v.Y=v.Y/2;
      else
        v.Y=m.size.Y-(v.Y+1)/2;
      if(v.Z%2==0)
        v.Z=v.Z/2;
      else
        v.Z=m.size.Z-(v.Z+1)/2;
    }
    virtual void invtranslate(Vector& v){
      if(2*v.X<m.size.X)
        v.X=2*v.X;
      else
        v.X=2*(m.size.X-v.X)-1;
      if(2*v.Y<m.size.Y)
        v.Y=2*v.Y;
      else
        v.Y=2*(m.size.Y-v.Y)-1;
      if(2*v.Z<m.size.Z)
        v.Z=2*v.Z;
      else
        v.Z=2*(m.size.Z-v.Z)-1;
    }
    virtual Vector getEnd(){
      Vector end(m.size.X-1,m.size.Y-1,m.size.Z-1);
      translate(end);
      return end;
    }
    virtual void moveVector(Vector& v,bool down){
      invtranslate(v);
      Walker::moveVector(v,down);
      translate(v);
    }
  public:
    ReorderWalker(Maze& m):Walker(m){}
};
 
class RandOrderWalker:public ReorderWalker{
  protected:
    int* xtrans;
    int* ytrans;
    int* ztrans;
    int* xinvtrans;
    int* yinvtrans;
    int* zinvtrans;
    
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
    }
    
    virtual Vector getEnd(){
      return Vector(xtrans[m.size.X-1],ytrans[m.size.Y-1],ztrans[m.size.Z-1]);
    }
    virtual Vector getStart(){
      return Vector(xtrans[0],ytrans[0],ztrans[0]);
    }
    virtual void translate(Vector& v){
      v.X=xtrans[v.X];
      v.Z=ztrans[v.Z];
      v.Y=ytrans[v.Y];
    }
    virtual void invtranslate(Vector& v){
      v.Y=yinvtrans[v.Y];
      v.Z=zinvtrans[v.Z];
      v.X=xinvtrans[v.X];
    }
  public:
    RandOrderWalker(Maze& m):ReorderWalker(m){
      makeTrans(m.size.X,xtrans,xinvtrans);
      makeTrans(m.size.Y,ytrans,yinvtrans);
      makeTrans(m.size.Z,ztrans,zinvtrans);
    };
};


template <class MGH>
Maze generate(Vector size){
  #ifdef DEBUG
  cout<<"gen"<<endl;
  #endif
  Maze m(size);
  MGH* down=new MGH(m,true);
  MGH* up=new MGH(m,false);
  #ifdef DEBUG
  cout<<true<<" state "<<down->p<<" "<<down->h->huntStart<<" "<<down->h->huntEnd<<endl;
  cout<<false<<" state "<<up->p<<" "<<up->h->huntStart<<" "<<up->h->huntEnd<<endl;
  #endif
  while(!(down->doStep()&up->doStep())){
    #ifdef DEBUG
    cout<<true<<" state "<<down->p<<" "<<down->h->huntStart<<" "<<down->h->huntEnd<<endl;
    cout<<false<<" state "<<up->p<<" "<<up->h->huntStart<<" "<<up->h->huntEnd<<endl;
    #endif
  };
  delete up;
  delete down;
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

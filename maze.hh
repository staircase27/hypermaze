#include "dirns.hh"
#include "vector.hh"
#include "string.h"

#ifndef MAZE_HH_INC
#define MAZE_HH_INC

class Point;
class ConstPoint;

class Maze{
  private:
    int* maze;
  public:
    Vector size;

    Maze(Vector size);
    Point operator [](Vector p);
    ConstPoint operator [](Vector p) const;
    friend ostream& operator<<(ostream&,Maze);
    friend istream& operator>>(istream&,Maze&);
};

class Point{
  private:
    Maze& maze;
    int* point;
  public:
    Point(Maze& maze,int* point):maze(maze),point(point){};
    Point(const Point& p):maze(p.maze),point(p.point){};
    Point& operator=(const Point& p){
      maze=p.maze;
      point=p.point;
      return *this;
    }
    inline int& operator*(){return *point;}
    inline const int& operator*()const {return *point;}
    Point operator+(Vector d){
      return Point(maze,point+(d.X+maze.size.X*(d.Y+maze.size.Y*d.Z)));
    }
    friend class ConstPoint;
};

class ConstPoint{
  private:
    const Maze& maze;
    int* point;
  public:
    ConstPoint(const Maze& maze,int* point):maze(maze),point(point){};
    ConstPoint(const ConstPoint& p):maze(p.maze),point(p.point){};
    ConstPoint(const Point& p):maze(p.maze),point(p.point){};
    inline const int& operator*()const {return *point;}
    ConstPoint operator+(Vector d){
      return ConstPoint(maze,point+(d.X+maze.size.X*(d.Y+maze.size.Y*d.Z)));
    }
};


Point Maze::operator [](Vector p){
  return Point(*this,maze+(p.X+size.X*(p.Y+size.Y*p.Z)));
};
ConstPoint Maze::operator [](Vector p) const{
  return ConstPoint(*this,maze+(p.X+size.X*(p.Y+size.Y*p.Z)));
};

Maze::Maze(Vector size):maze(new int[size.X*size.Y*size.Z]),size(size){
  int defmask=ALLDIRNSMASK&~to_mask(UP)&~to_mask(DOWN);
  memset(maze,0,size.X*size.Y*size.Z*sizeof(int));
  for(int x=0;x<size.X;++x){
    int mask=defmask;
    if(x==0)
      mask&=~to_mask(RIGHT);
    else if(x==size.X-1)
      mask&=~to_mask(LEFT);
    for(int z=0;z<size.Z;++z){
      *(*this)[Vector(x,0,z)]=mask;
      *(*this)[Vector(x,size.Y-1,z)]=mask;
    }
    *(*this)[Vector(x,0,0)]&=~to_mask(BACK);
    *(*this)[Vector(x,size.Y-1,0)]&=~to_mask(BACK);
    *(*this)[Vector(x,0,size.Z-1)]&=~to_mask(FORWARD);
    *(*this)[Vector(x,size.Y-1,size.Z-1)]&=~to_mask(FORWARD);
  }
};

ostream& operator<<(ostream& o,Maze m){
  o<<m.size.X<<" "<<m.size.Y<<" "<<m.size.Z<<hex<<endl;
  int* p=m.maze;
  for(int i=0;i<m.size.X*m.size.Y*m.size.Z;++i){
    o<<*p<<" ";
    #ifdef DEBUG
    if((i+1)%m.size.X==0)
      o<<endl;
    if((i+1)%(m.size.X*m.size.Y)==0)
      o<<endl;
    #endif
    ++p;
  }
  o<<dec<<endl;
  return o;
}

istream& operator>>(istream& o,Maze& m){
  o>>m.size.X>>m.size.Y>>m.size.Z>>hex;
  delete[] m.maze;
  m.maze=new int[m.size.X*m.size.Y*m.size.Z];
  int* p=m.maze;
  for(int i=0;i<m.size.X*m.size.Y*m.size.Z;++i){
    o>>*p;
    ++p;
  }
  return o>>dec;
}


void prettyPrint(ostream& o,Maze m,int w=150){
  w/=(m.size.X*4+2);
  o<<m.size.X<<" "<<m.size.Y<<" "<<m.size.Z<<" :"<<w<<endl;
  for(int Z=0;Z<m.size.Z;Z+=w){
    for(int y=m.size.Y-1;y>=0;--y){
      for(int z=Z;z<m.size.Z&&z<Z+w;++z){
        for(int x=m.size.X-1;x>=0;--x){
          o<<(((*m[Vector(x,y,z)]&1<<10)==0)?"#":"X")<<(((*m[Vector(x,y,z)]&to_mask(RIGHT))==0)?" ":"=");
        }
        o<<" ";
        for(int x=m.size.X-1;x>=0;--x){
          o<<(((*m[Vector(x,y,z)]&to_mask(FORWARD))==0)?" ":"+")<<" ";
        }
        o<<" ";
      }
      o<<endl;
      for(int z=Z;z<m.size.Z&&z<Z+w;++z){
        for(int x=m.size.X-1;x>=0;--x){
          o<<(((*m[Vector(x,y,z)]&to_mask(DOWN))==0)?" ":"|")<<" ";
        }
        o<<" ";
        for(int x=m.size.X-1;x>=0;--x){
          o<<"  ";
        }
        o<<" ";
      }
      o<<endl;
    }
    o<<endl;
  }
}


#endif

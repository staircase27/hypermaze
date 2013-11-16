#include "dirns.hh"
#include "vector.hh"

#include "SmartPointer.hh"

#include "hypio.hh"

#ifdef IRRLICHT
#include "irrio.hh"
#endif
#ifdef IOSTREAM
#include <iostream>
#endif

#ifndef MAZE_HH_INC
#define MAZE_HH_INC

class Point;
class ConstPoint;
#ifdef IRRLICHT
class MazeParser;
#endif

class Maze
{
  private:
    SPA<int> maze;
  public:
    Vector size;

    Maze(Vector size);
    Maze(Maze& m);
    Maze(const Maze& m);
    
    ~Maze();
    
    Maze& operator=(const Maze& m);
    
    inline void makeThisACopy(){
      (*this)=Maze((const Maze)(*this));
    }
    
    Point operator [](Vector p);
    ConstPoint operator [](Vector p) const;
    
    #ifdef IOSTREAM
    friend ostream& operator<<(ostream&,const Maze&);
    friend istream& operator>>(istream&,Maze&);
    #endif
    
    #ifdef IRRLICHT
    friend class MazeParser;
    void load(irr::IReadFile* in);
    void save(irr::IWriteFile*);
    #endif
    
    friend IOResult read(HypIStream&,Maze& m);
    
};

IOResult read(HypIStream&,Maze&);

class Point{
  private:
		Vector size;
    SPA<int> point;
  public:
    Point(Vector size,SPA<int> point):size(size),point(point){};
    Point(const Point& p):size(p.size),point(p.point){};
    Point& operator=(const Point& p){
      size=p.size;
      point=p.point;
      return *this;
    }
    inline int& operator*()const {return *point;}
    inline Point operator+(Vector d){
      return Point(size,point+(d.X+size.X*(d.Y+size.Y*d.Z)));
    }
    friend class ConstPoint;
};

class ConstPoint{
  private:
    Vector size;
    SPA<int> point;
  public:
    ConstPoint(Vector size,SPA<int> point):size(size),point(point){};
    ConstPoint(const ConstPoint& p):size(p.size),point(p.point){};
    ConstPoint(const Point& p):size(p.size),point(p.point){};
    inline const int& operator*() const {return *point;}
    inline ConstPoint operator+(Vector d){
      return ConstPoint(size,point+(d.X+size.X*(d.Y+size.Y*d.Z)));
    }
};

#ifdef IOSTREAM
inline ostream& operator<<(ostream& o,const Maze& m){
  o<<m.size.X<<" "<<m.size.Y<<" "<<m.size.Z<<hex<<endl;
  const int* p=&*m.maze;
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

inline istream& operator>>(istream& o,Maze& m){
  o>>m.size.X>>m.size.Y>>m.size.Z>>hex;
  m=Maze(m.size);
  int* p=&*m.maze;
  for(int i=0;i<m.size.X*m.size.Y*m.size.Z;++i){
    o>>*p;
    ++p;
  }
  return o>>dec;
}

void prettyPrint(ostream& o,Maze m,int w=150);
#endif

#endif

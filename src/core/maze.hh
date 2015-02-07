#include "dirns.hh"
#include "vector.hh"

#include "SmartPointer.hh"

#include "hypio.hh"

#ifdef IOSTREAM
#include <iostream>
#endif

#ifndef MAZE_HH_INC
#define MAZE_HH_INC

class Point;
class ConstPoint;

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
    friend std::ostream& operator<<(std::ostream&,const Maze&);
    friend std::istream& operator>>(std::istream&,Maze&);
    #endif

    friend IOResult read(HypIStream&,Maze& m);
    friend bool write(HypOStream& s,const Maze& m);

};

IOResult read(HypIStream&,Maze&);
bool write(HypOStream& s,const Maze& m);

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
inline std::ostream& operator<<(std::ostream& o,const Maze& m){
  o<<m.size.X<<" "<<m.size.Y<<" "<<m.size.Z<<std::hex<<std::endl;
  SPA<int> p=m.maze;
  for(int i=0;i<m.size.X*m.size.Y*m.size.Z;++i,++p){
    o<<*p<<" ";
    #ifdef DEBUG
    if((i+1)%m.size.X==0)
      o<<std::endl;
    if((i+1)%(m.size.X*m.size.Y)==0)
      o<<std::endl;
    #endif
  }
  o<<std::dec<<std::endl;
  return o;
}

inline std::istream& operator>>(std::istream& o,Maze& m){
  o>>m.size.X>>m.size.Y>>m.size.Z>>std::hex;
  m=Maze(m.size);
  SPA<int> p=m.maze;
  for(int i=0;i<m.size.X*m.size.Y*m.size.Z;++i,++p){
    o>>*p;
  }
  return o>>std::dec;
}

void prettyPrint(std::ostream& o,Maze m,int w=150);
#endif

#endif

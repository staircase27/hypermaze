#include "dirns.hh"
#include "vector.hh"

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
    int* maze;
  public:
    Vector size;

    Maze(Vector size);
    Point operator [](Vector p);
    ConstPoint operator [](Vector p) const;
    
    #ifdef IOSTREAM
    friend ostream& operator<<(ostream&,Maze);
    friend istream& operator>>(istream&,Maze&);
    #endif
    
    #ifdef IRRLICHT
    friend class MazeParser;
    void load(irr::IReadFile* in);
    void save(irr::IWriteFile*);
    #endif
    
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
    inline Point operator+(Vector d){
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
    inline ConstPoint operator+(Vector d){
      return ConstPoint(maze,point+(d.X+maze.size.X*(d.Y+maze.size.Y*d.Z)));
    }
};

#ifdef IOSTREAM
inline ostream& operator<<(ostream& o,Maze m){
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

inline istream& operator>>(istream& o,Maze& m){
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

void prettyPrint(ostream& o,Maze m,int w=150);
#endif

#endif

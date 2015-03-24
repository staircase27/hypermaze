/**
 * @file maze.cc
 * @brief Implementation of maze.hh
 */
#include "maze.hh"
#include <cstring>

using namespace std;

Point Maze::operator [](Vector p){
  return Point(thesize,maze+(p.X+thesize.X*(p.Y+thesize.Y*p.Z)));
};
ConstPoint Maze::operator [](Vector p) const{
  return ConstPoint(thesize,maze+(p.X+thesize.X*(p.Y+thesize.Y*p.Z)));
};

Maze::Maze(Vector thesize):maze(thesize.X*thesize.Y*thesize.Z),thesize(thesize){
  int defmask=ALLDIRNSMASK&~to_mask(UP)&~to_mask(DOWN);
  for(int x=0;x<thesize.X;++x){
    int mask=defmask;
    if(x==0)
      mask&=~to_mask(RIGHT);
    else if(x==thesize.X-1)
      mask&=~to_mask(LEFT);
    for(int z=0;z<thesize.Z;++z){
      *(*this)[Vector(x,0,z)]=mask;
      *(*this)[Vector(x,thesize.Y-1,z)]=mask;
    }
    *(*this)[Vector(x,0,0)]&=~to_mask(BACK);
    *(*this)[Vector(x,thesize.Y-1,0)]&=~to_mask(BACK);
    *(*this)[Vector(x,0,thesize.Z-1)]&=~to_mask(FORWARD);
    *(*this)[Vector(x,thesize.Y-1,thesize.Z-1)]&=~to_mask(FORWARD);
  }
};

Maze::Maze(Maze& m):maze(m.maze),thesize(m.thesize){};

Maze::Maze(const Maze& m):maze(m.thesize.X*m.thesize.Y*m.thesize.Z),thesize(m.thesize){
  memcopy(maze,(SPA<const int>)m.maze,m.thesize.X*m.thesize.Y*m.thesize.Z);
}
Maze::~Maze(){}

Maze& Maze::operator=(const Maze& m){
  thesize=m.thesize;
  maze=m.maze;
  return *this;
}

#ifdef IOSTREAM
void prettyPrint(std::ostream& o,Maze m,int w){
  w/=(m.thesize.X*4+2);
  o<<m.thesize.X<<" "<<m.thesize.Y<<" "<<m.thesize.Z<<" :"<<w<<endl;
  for(int Z=0;Z<m.thesize.Z;Z+=w){
    for(int y=m.thesize.Y-1;y>=0;--y){
      for(int z=Z;z<m.thesize.Z&&z<Z+w;++z){
        for(int x=m.thesize.X-1;x>=0;--x){
          o<<(((*m[Vector(x,y,z)]&1<<10)==0)?"#":"X")<<(((*m[Vector(x,y,z)]&to_mask(RIGHT))==0)?" ":"=");
        }
        o<<" ";
        for(int x=m.thesize.X-1;x>=0;--x){
          o<<(((*m[Vector(x,y,z)]&to_mask(FORWARD))==0)?" ":"+")<<" ";
        }
        o<<" ";
      }
      o<<endl;
      for(int z=Z;z<m.thesize.Z&&z<Z+w;++z){
        for(int x=m.thesize.X-1;x>=0;--x){
          o<<(((*m[Vector(x,y,z)]&to_mask(DOWN))==0)?" ":"|")<<" ";
        }
        o<<" ";
        for(int x=m.thesize.X-1;x>=0;--x){
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

IOResult read(HypIStream& s,Maze& m){
  Vector thesize;
  IOResult r;
  if(!((r=read(s,thesize.X,10)).ok&&
       (r=read(s,thesize.Y,10)).ok&&
       (r=read(s,thesize.Z,10)).ok))
    return IOResult(false,r.eof);
  m=Maze(thesize);
  int pos=-1;//start at -1 so first increment doesn't sift us of the maze
  while(++pos<m.thesize.X*m.thesize.Y*m.thesize.Z){
    if(!(r=read(s,m.maze[pos],16)).ok)
      return IOResult(false,r.eof);
  }
  return IOResult(true,r.eof);
}

bool write(HypOStream& s,const Maze& m){
  bool status=write(s,m.thesize.X);
  status&=write(s,m.thesize.Y);
  status&=write(s,m.thesize.Z);
  s.setNextSpace("\n");
  SPA<int> p=m.maze;
  SPA<int> end=m.maze+m.thesize.X*m.thesize.Y*m.thesize.Z;
  while(p!=end){
    status&=write(s,*p,16);
    ++p;
  }
  return status;
}


#include "maze.hh"
#include <cstring>

Point Maze::operator [](Vector p){
  return Point(size,maze+(p.X+size.X*(p.Y+size.Y*p.Z)));
};
ConstPoint Maze::operator [](Vector p) const{
  return ConstPoint(size,maze+(p.X+size.X*(p.Y+size.Y*p.Z)));
};

Maze::Maze(Vector size):maze(new int[size.X*size.Y*size.Z]),size(size){
  int defmask=ALLDIRNSMASK&~to_mask(UP)&~to_mask(DOWN);
  memset(&*maze,0,size.X*size.Y*size.Z*sizeof(int));
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

Maze::Maze(Maze& m):maze(m.maze),size(m.size){
};
Maze::Maze(const Maze& m):maze(new int[m.size.X*m.size.Y*m.size.Z]),size(m.size){
  memcpy(&*maze,&*m.maze,sizeof(int)*m.size.X*m.size.Y*m.size.Z);
}
Maze::~Maze(){}

Maze& Maze::operator=(const Maze& m){
  size=m.size;
  maze=m.maze;
  return *this;
}

#ifdef IOSTREAM
void prettyPrint(ostream& o,Maze m,int w){
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

IOResult read(HypIStream& s,Maze& m){
  Vector size;
  IOResult r;
  if(!((r=read(s,size.X,10)).ok&&
       (r=read(s,size.Y,10)).ok&&
       (r=read(s,size.Z,10)).ok))
    return IOResult(false,r.eof);
  m=Maze(size);
  int pos=-1;//start at -1 so first increment doesn't sift us of the maze
  while(++pos<m.size.X*m.size.Y*m.size.Z){
    if(!(r=read(s,m.maze[pos],16)).ok)
      return IOResult(false,r.eof);
  }
  return IOResult(true,r.eof);
}

bool write(HypOStream& s,const Maze& m){
  write(s,m.size.X);
  write(s,m.size.Y);
  write(s,m.size.Z);
  s.setNextSpace("\n");
  SPA<int> p=m.maze;
  SPA<int> end=m.maze+m.size.X*m.size.Y*m.size.Z;
  while(p!=end){
    write(s,*p,16);
    ++p;
  }
}

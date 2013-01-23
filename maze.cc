#include "maze.hh"
#include <cstring>

Point Maze::operator [](Vector p){
  return Point(*this,maze+(p.X+size.X*(p.Y+size.Y*p.Z)));
};
ConstPoint Maze::operator [](Vector p) const{
  return ConstPoint(*this,maze+(p.X+size.X*(p.Y+size.Y*p.Z)));
};

Maze::Maze(Vector size):maze(new int[1+size.X*size.Y*size.Z]+1),size(size){
  *(maze-1)=1;
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

Maze::Maze(Maze& m):maze(m.maze),size(m.size){
  *(maze-1)+=1;
};
Maze::Maze(const Maze& m):maze(new int[1+m.size.X*m.size.Y*m.size.Z]+1),size(m.size){
  *(maze-1)=1;
  memcpy(maze,m.maze,sizeof(int)*m.size.X*m.size.Y*m.size.Z);
}
Maze::~Maze(){
  *(maze-1)-=1;
  if(*(maze-1)<=0)
    delete[] (maze-1);
}

Maze& Maze::operator=(Maze& m){
  *(maze-1)-=1;
  if(*(maze-1)<=0)
    delete[] (maze-1);
  maze=m.maze;
  *(maze-1)+=1;
  size=m.size;
}
Maze& Maze::operator=(const Maze& m){
  *(maze-1)-=1;
  if(*(maze-1)<=0)
    delete[] (maze-1);
  maze=new int[1+m.size.X*m.size.Y*m.size.Z]+1;
  *(maze-1)=1;
  memcpy(maze,m.maze,sizeof(int)*m.size.X*m.size.Y*m.size.Z);
  size=m.size;
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

#ifdef IRRLICHT
class MazeParser:public InputParser{
  Maze* m;
  int pos;
  public:
    MazeParser(Maze* m):m(m),pos(-1){};
    
    Used parse(char* data,irr::u32 length,bool eof){
      char* start=data;
      char* end=data+length;
      if(!eof)
        end-=1;
      if(pos==-1){
        Vector size;
        size.X=strtol(data,&data,10);
        if(data>=end) return Used(0,false);

        size.Y=strtol(data,&data,10);
        if(data>=end) return Used(0,false);

        size.Z=strtol(data,&data,10);
        if(data>=end) return Used(0,false);
        *m=Maze(size);
        pos=0;
      }
      
      char* tmp;
      while(pos<m->size.X*m->size.Y*m->size.Z){
        m->maze[pos]=strtol(data,&tmp,16);
        if(tmp>=end) return Used(data-start,false);
        ++pos;
        data=tmp;
      }
      return Used(data-start,true);
    }
};
void Maze::load(irr::IReadFile* in){
  MazeParser mp(this);
  ::parse(in,&mp);
}
irr::stringc tostr16(int number){
	// store if negative and make positive
	bool negative = false;
	if (number < 0)
	{
		number *= -1;
		negative = true;
	}

	// temporary buffer for 16 numbers
	irr::c8 tmpbuf[16]={0};
	irr::u32 idx = 15;

	// special case '0'

	if (!number)
	{
		tmpbuf[14] = '0';
		return irr::stringc(&tmpbuf[14]);
	}

	// add numbers
	while(number && idx)
	{
		--idx;
		if((number % 16)>9)
  		tmpbuf[idx] = (irr::c8)('a' - 10 + (number % 16));
    else
  		tmpbuf[idx] = (irr::c8)('0' + (number % 16));
		number /= 16;
	}

	// add sign
	if (negative)
	{
		--idx;
		tmpbuf[idx] = '-';
	}

	return irr::stringc(&tmpbuf[idx]);
}
void Maze::save(irr::IWriteFile* out){
  irr::stringc str;
  str+=size.X;
  str+=" ";
  str+=size.Y;
  str+=" ";
  str+=size.Z;
  str+="\n";
  int* p=maze;
  int* end=maze+size.X*size.Y*size.Z;
  while(p!=end){
    str+=tostr16(*p++);
    str+=" ";
    if(str.size()>256){
      out->write(str.c_str(),str.size());
      str=irr::stringc();
    }
  }
  str+="\n";
  out->write(str.c_str(),str.size());
}
#endif


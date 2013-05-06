#include <iostream>
#include <fstream>
#include "maze.hh"
#include "string.hh"
#include "mazegen.hh"
#include <cstdlib>
#include <ctime>


using namespace std;


void addWall(Maze m,Vector v,Dirn wall){
  Point p=m[v];
  *p|=to_mask(wall);
  p=p+to_vector(wall);
  v=v+to_vector(wall);
  if(v.X<0||v.X==m.size.X||v.Y<0||v.Y==m.size.Y||v.Z<0||v.Z==m.size.Z)
    return;
  *p|=to_mask(opposite(wall));
}
void removeWall(Maze m,Vector v,Dirn wall){
  Point p=m[v];
  *p&=~to_mask(wall);
  p=p+to_vector(wall);
  v=v+to_vector(wall);
  if(v.X<0||v.X==m.size.X||v.Y<0||v.Y==m.size.Y||v.Z<0||v.Z==m.size.Z)
    return;
  *p&=~to_mask(opposite(wall));
}

int main(){
  Dirn d=UP;
  cout<<to_id(d)<<endl;
  cout<<to_id(opposite(d))<<endl;
  cout<<to_id(opposite(from_id(5)))<<endl;
  cout<<to_mask(d)<<endl;
  int c=0;
  c|=to_mask(d);
  c|=to_mask(opposite(d));
  cout<<"c"<<c<<endl;
  
  Maze m(Vector(5,5,5));
  cout<<m;
  for(int x=0;x<m.size.X;++x)
    for(int z=0;z<m.size.Z;++z){
      for(int y=0;y<m.size.Y-1;++y)
        addWall(m,Vector(x,y,z),UP);
      if((x+z)%2==0)
        removeWall(m,Vector(x,0,z),UP);
      else
        removeWall(m,Vector(x,m.size.Y-1,z),DOWN);
    }
  cout<<m;
  String s(m);
  StringPlay l(s);
  cout<<s<<endl;
  cout<<l.canMove(DOWN)<<endl;
  l.doMove(DOWN);
  cout<<s<<endl;  
  cout<<l.canMove(FORWARD)<<endl;
  cout<<l<<endl;
  l.slide(false,true);
  while(l.slide(true,false)){};
  l.slide(true,true);
  cout<<l<<endl;
  cout<<l.tryMove(FORWARD)<<endl;
  cout<<s<<endl;
  l.slide(true,true);
  l.slide(false,true);
  cout<<l<<endl;
  cout<<l.tryMove(FORWARD)<<endl;
  cout<<s<<endl;
  l.slide(true,true);
  l.slide(false,true);
  cout<<l<<endl;
  cout<<l.tryMove(FORWARD)<<endl;
  cout<<s<<endl;
  l.slide(true,true);
  l.slide(false,true);
  cout<<l<<endl;
  
  ifstream inf("mazetest.hmo");
  inf>>m;
  inf.close();
  cout<<"loaded maze"<<endl;
  prettyPrint(cout,m);

  srand(time(NULL));

  m=generate<RandLimitMazeGenHalf<Hunter<RandOrderWalker<DiagonalWalker> > > >(Vector(10,10,10));

  prettyPrint(cout,m);
  ofstream of("mazetest.hmo");
  of<<m;
  of.close();
  
  cout<<"solved"<<endl;
  solve(m);
  prettyPrint(cout,m);
  
  
}

#include "../shared/cpphypioimp.hh"
#include <iostream>
#include <fstream>
#include "../core/maze.hh"
#include "../core/script.hh"
#include "../core/mazegen.hh"
#include <string>


using namespace std;


void addWall(Maze m,Vector v,Dirn wall){
  if(v.X<0||v.X>=m.size().X||v.Y<0||v.Y>=m.size().Y||v.Z<0||v.Z>=m.size().Z)
    return;
  Point p=m[v];
  cout<<"Updating "<<v<<" from "<<*p<<" to ";
  *p|=to_mask(wall);
  cout<<*p<<" by adding "<<to_mask(wall)<<endl;
  p=p+to_vector(wall);
  v=v+to_vector(wall);
  if(v.X<0||v.X>=m.size().X||v.Y<0||v.Y>=m.size().Y||v.Z<0||v.Z>=m.size().Z)
    return;
  cout<<"Updating "<<v<<" from "<<*p<<" to ";
  *p|=to_mask(opposite(wall));
  cout<<*p<<" by adding "<<to_mask(opposite(wall))<<endl;
}
void removeWall(Maze& m,Vector v,Dirn wall){
  if(v.X<0||v.X>=m.size().X||v.Y<0||v.Y>=m.size().Y||v.Z<0||v.Z>=m.size().Z)
    return;
  Point p=m[v];
  cout<<"Updating "<<v<<" from "<<*p<<" to ";
  *p&=~to_mask(wall);
  cout<<*p<<" by removing "<<to_mask(wall)<<endl;
  p=p+to_vector(wall);
  v=v+to_vector(wall);
  if(v.X<0||v.X>=m.size().X||v.Y<0||v.Y>=m.size().Y||v.Z<0||v.Z>=m.size().Z)
    return;
  cout<<"Updating "<<v<<" from "<<*p<<" to ";
  *p&=~to_mask(opposite(wall));
  cout<<*p<<" by removing "<<to_mask(wall)<<endl;
}

int main(){
  char* filename=new char[256];
  Maze m(Vector(5,5,5));
  Script s;
  bool loaded=false;

  while(true){
    cout<<"Please enter the filename for the level: ";
    cin.getline(filename,256);
    ifstream is(filename);
    if(is.is_open()){
      CPPHypIStream ihs(is);
      read(ihs,m);
      read(ihs,s);
      loaded=true;
      break;
    }else{
      ofstream os(filename);
      if(!os.is_open()){
        cout<<"error opening file"<<endl;
        continue;
      }
      CPPHypOStream ohs(os);
      write(ohs,m);
      ohs.setNextSpace("\n");
      write(ohs,s);
      break;
    }
  }

  if(!loaded){
    Vector size(0,0,0);
    while(size.X<3 || size.Y<3 || size.Z<3){
      cout<<"Please enter the size of the maze in the order width, height, depth with each component separated by spaces"<<endl<<": ";
      cin>>size.X>>size.Y>>size.Z;
    }
    m=Maze(size);
    ofstream os(filename);
    if(!os.is_open()){
      cout<<"error opening file"<<endl;
    }
    CPPHypOStream ohs(os);
    write(ohs,m);
    ohs.setNextSpace("\n");
  }
  while(true){
    cout<<m<<endl;
    char c='\0';
    cout<<"Do you want to add, remove or check a wall? a/r/c: ";
    cin>>c;
    if(c!='a' && c!='r' && c!='c')
      continue;

    if(c=='c'){
      Vector v(0,0,0);
      cout<<"Please enter the position to check the walls for: ";
      cin>>v.X>>v.Y>>v.Z;
      if(v.X<0||v.X>=m.size().X||v.Y<0||v.Y==m.size().Y||v.Z<0||v.Z==m.size().Z){
        cout<<"Invalid position"<<endl;
        continue;
      }
      Point p=m[v];
      cout<<v<<" has walls "<<*p<<" = ";
#define MAKETERM(D,N) \
      if((*p & to_mask(D))!=0){\
        if(hasdirn)\
          cout<<", ";\
        cout<<N;\
        hasdirn=true;\
      }
      bool hasdirn=false;
      MAKETERM(UP,"up")
      MAKETERM(DOWN,"down")
      MAKETERM(LEFT,"left")
      MAKETERM(RIGHT,"right")
      MAKETERM(FORWARD,"forward")
      MAKETERM(BACK,"back")
      if(!hasdirn)
        cout<<"none";
      cout<<endl;
#undef MAKETERM
      continue;

    }else if(c=='a'){
      Vector v(0,0,0);
      cout<<"Please enter the start of the wall to add: ";
      cin>>v.X>>v.Y>>v.Z;
      if(v.X<0||v.X>=m.size().X||v.Y<0||v.Y>=m.size().Y||v.Z<0||v.Z>=m.size().Z){
        cout<<"Invalid position"<<endl;
        continue;
      }
      cout<<"What dirn should the wall be added? u (up), d (down), l (left), r (right), f (foward) and b (backwards)?"<<endl<<": ";
      cin>>c;
#define MAKETERM(D,C) \
      case C:\
        d=D;\
        break;
      bool valid=true;
      Dirn d=UP;
      switch(c){
        MAKETERM(UP,'u')
        MAKETERM(DOWN,'d')
        MAKETERM(LEFT,'l')
        MAKETERM(RIGHT,'r')
        MAKETERM(FORWARD,'f')
        MAKETERM(BACK,'b')
        default:
          cout<<"Unknown direction letter"<<endl;
          valid=false;
          break;
      }
#undef MAKETERM
      if(!valid)
        continue;
      addWall(m,v,d);

    }else{
      Vector v(0,0,0);
      cout<<"Please enter the start of the wall to remove: ";
      cin>>v.X>>v.Y>>v.Z;
      if(v.X<0||v.X>=m.size().X||v.Y<0||v.Y>=m.size().Y||v.Z<0||v.Z>=m.size().Z){
        cout<<"Invalid position"<<endl;
        continue;
      }
      cout<<"What dirn is the wall to remove? u (up), d (down), l (left), r (right), f (foward) and b (backwards)?"<<endl<<": ";
      cin>>c;
#define MAKETERM(D,C) \
      case C:\
        d=D;\
        break;
      bool valid=true;
      Dirn d=UP;
      switch(c){
        MAKETERM(UP,'u')
        MAKETERM(DOWN,'d')
        MAKETERM(LEFT,'l')
        MAKETERM(RIGHT,'r')
        MAKETERM(FORWARD,'f')
        MAKETERM(BACK,'b')
        default:
          cout<<"Unknown direction letter"<<endl;
          valid=false;
          break;
      }
#undef MAKETERM
      if(!valid)
        continue;
      removeWall(m,v,d);
    }
    ofstream os(filename);
    if(!os.is_open()){
      cout<<"error opening file"<<endl;
    }
    CPPHypOStream ohs(os);
    write(ohs,m);
    ohs.setNextSpace("\n");
    write(ohs,s);
    cout<<"written"<<endl;
  }
}

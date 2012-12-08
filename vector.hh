#include <iostream>
#ifndef VECTOR_HH_INC
#define VECTOR_HH_INC
using namespace std;

#ifdef IRRLICHT
  #include "irrlicht.h"
  typedef irr::core::vector3di Vector;
#else
  class Vector{
    public:
      int X,Y,Z;
      Vector(int x,int y,int z):X(x),Y(y),Z(z){};
      Vector operator+(Vector o){
        return Vector(X+o.X,Y+o.Y,Z+o.Z);
      }
      Vector& operator+=(Vector o){
        X+=o.X;Y+=o.Y;Z+=o.Z;
        return *this;
      }
      Vector operator-(Vector o){
        return Vector(X-o.X,Y-o.Y,Z-o.Z);
      }
      Vector& operator-=(Vector o){
        X-=o.X;Y-=o.Y;Z-=o.Z;
        return *this;
      }
      bool operator !=(Vector o){
        return X!=o.X||Y!=o.Y||Z!=o.Z;
      }
  };
#endif

bool inCube(Vector p,Vector start,Vector end){
  return !(p.X<start.X||p.Y<start.Y||p.Z<start.Z||
      p.X>=end.X||p.Y>=end.Y||p.Z>=end.Z);
}

ostream& operator<<(ostream& o,Vector v){
  return o<<"<Vector: "<<v.X<<","<<v.Y<<","<<v.Z<<">";
}


#endif

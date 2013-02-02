#ifdef IOSTREAM
#include <iostream>
#endif
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
      Vector():X(0),Y(0),Z(0){};
      Vector operator+(const Vector& o) const{
        return Vector(X+o.X,Y+o.Y,Z+o.Z);
      }
      Vector& operator+=(const Vector& o){
        X+=o.X;Y+=o.Y;Z+=o.Z;
        return *this;
      }
      Vector operator-(const Vector& o) const{
        return Vector(X-o.X,Y-o.Y,Z-o.Z);
      }
      Vector operator-() const{
        return Vector(-X,-Y,-Z);
      }
      Vector& operator-=(const Vector& o){
        X-=o.X;Y-=o.Y;Z-=o.Z;
        return *this;
      }
      Vector operator*(int i) const {
        return Vector(X*i,Y*i,Z*i);
      }
      bool operator !=(const Vector& o)const {
        return X!=o.X||Y!=o.Y||Z!=o.Z;
      }
      int dotProduct(const Vector& o) const{
        return X*o.X+Y*o.Y+Z*o.Z;
      }
  };
#endif
inline Vector operator*(int i,Vector v){
  return v*i;
}
inline bool inCube(Vector p,Vector start,Vector end){
  return !(p.X<start.X||p.Y<start.Y||p.Z<start.Z||
      p.X>=end.X||p.Y>=end.Y||p.Z>=end.Z);
}

#ifdef IOSTREAM
inline ostream& operator<<(ostream& o,Vector v){
  return o<<"<Vector: "<<v.X<<","<<v.Y<<","<<v.Z<<">";
}
#endif

#endif

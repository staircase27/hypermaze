/**
 * @file vector.hh
 * @brief This file provides a vector class and a few functions for use with it.
 *
 * If we are using irrlicht then the Vector class is just an alias for irr::code::vector3di
 * otherwise we have to implement one ourselves.
 */

#ifdef IOSTREAM
#include <iostream>
#endif

#ifndef VECTOR_HH_INC
#define VECTOR_HH_INC

#include "hypio.hh"

#ifdef IRRLICHT
  #include "vector3d.h"
  /// A vector for locations in the maze
  /**
   * We use the irrlicht vector when we are linking with irrlicht
   */
  typedef irr::core::vector3di Vector;
#else
  /// A vector for locations in the maze
  /**
   * The manual implementation for when we don't have irrlicht
   */
  class Vector{
    public:
      int X,///< @brief the x coordinates of the point
          Y,///< @brief the y coordinates of the point
          Z;///< @brief the z coordinates of the point
      /// Construct a vector for a point
      /**
       * @param x the x coordinate of the new Vector
       * @param y the y coordinate of the new Vector
       * @param z the z coordinate of the new Vector
       */
      Vector(int x,int y,int z):X(x),Y(y),Z(z){};
      /// Construct a new vector for the orign
      Vector():X(0),Y(0),Z(0){};
      /// return a vector that is the sum of this vector and another
      /**
       * @param o the vector to add to this one
       * @return a new vector containing the result of the addition
       */
      Vector operator+(const Vector& o) const{
        return Vector(X+o.X,Y+o.Y,Z+o.Z);
      }
      /// Add a vector to this vector
      /**
       * @param o the vector to add to this vector
       * @return *this
       */
      Vector& operator+=(const Vector& o){
        X+=o.X;Y+=o.Y;Z+=o.Z;
        return *this;
      }
      /// return a vector that is the difference between this vector and another
      /**
       * @param o the vector to subtract from this one
       * @return a new vector containing the result of the subtraction
       */
      Vector operator-(const Vector& o) const{
        return Vector(X-o.X,Y-o.Y,Z-o.Z);
      }
      /// return the opposite of this vector
      /**
       * @return a new vector pointing in the opposite direction with the same length
       */
      Vector operator-() const{
        return Vector(-X,-Y,-Z);
      }
      /// subtract a vector from this vector
      /**
       * @param o the vector to subtract
       * @return *this
       */
      Vector& operator-=(const Vector& o){
        X-=o.X;Y-=o.Y;Z-=o.Z;
        return *this;
      }
      /// return a vector that is the result of multiplying this vector by a scalar
      /**
       * @param i the scalar to multiply by
       * @return a new vector containing the result of the multiplication
       */
      Vector operator*(int i) const {
        return Vector(X*i,Y*i,Z*i);
      }
      /// check if vectors are different.
      /**
       * This is a component wise comparison
       * @param o the vector to compare against
       * @return true if they are different and false otherwise
       */
      bool operator !=(const Vector& o)const {
        return X!=o.X||Y!=o.Y||Z!=o.Z;
      }
      /// check if vectors are the same.
      /**
       * This is a component wise comparison
       * @param o the vector to compare against
       * @return true if they are the same and false otherwise
       */
      int dotProduct(const Vector& o) const{
        return X*o.X+Y*o.Y+Z*o.Z;
      }
  };
#endif
/// Extra multiplication operator to make multiplication of a scalar and a vector symmetric
/**
 * This uses Vector::operator*(int i)
 * @param i the scalar to multiply by
 * @param v the vector to multiply
 * @return the result of the multiplication
 */
inline Vector operator*(int i,Vector v){
  return v*i;
}
/// Check if a vector is in an axis alligned cube defined by it's two diagonally opposite corners
/**
 * @param p the point to check
 * @param start the corner of the cube with all coordinates smallest
 * @param end the corner of the cube with all coordinates largest
 * @return true of p is in the cube else false
 */
inline bool inCube(Vector p,Vector start,Vector end){
  return !(p.X<start.X||p.Y<start.Y||p.Z<start.Z||
      p.X>=end.X||p.Y>=end.Y||p.Z>=end.Z);
}

#ifdef IOSTREAM
///Write a representation of the vector to a stream
/**
 * @param o the stream to write to
 * @param v the vector to write
 * @return the stream o
 */
inline std::ostream& operator<<(std::ostream& o,Vector v){
  return o<<"<Vector: "<<v.X<<","<<v.Y<<","<<v.Z<<">";
}
#endif

inline IOResult read(HypIStream& s,Vector& v){
  IOResult r;
  (r=read(s,v.X,10)).ok&&(r=read(s,v.Y,10)).ok&&(r=read(s,v.Z,10)).ok;
  return r;
}

inline bool write(HypOStream& s,const Vector& v){
  return write(s,v.X)&&write(s,v.Y)&&write(s,v.Z);
}

#endif

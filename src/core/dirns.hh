/**
 * @file dirns.hh
 * A 3D Direction type and various functions to convert between dirns, bitmasks, and vectors
 */
#ifndef DIRNS_HH_INC
#define DIRNS_HH_INC

#include "vector.hh"

#ifdef IOSTREAM
#include <iostream>
#endif

///A direction type
/**
 * An enum containing values for each direction
 */
 enum Dirn{
  UP=0,
  LEFT,
  FORWARD,
  DOWN,
  RIGHT,
  BACK
};

/// A list of all six directions
/**
 * used when a pointer to dirn is needed and to iterate over all directions
 */
static Dirn allDirns[6]={UP,LEFT,FORWARD,DOWN,RIGHT,BACK};

/// The bitmask corresponding to all directions selected
static const int ALLDIRNSMASK = (1<<6)-1;

/// Convert a Dirn to an integer id
/**
 * This just does a safe cast to an integer of the enum value
 * @param d the Dirn to convert
 * @return the id for d
 */
inline unsigned int to_id(Dirn d){
  return static_cast<unsigned int>(d);
}

/// Convert an integer id to a Dirn
/**
 * This just does a safe cast to a Dirn of the integer value.
 * No checks are made that the integer is valid
 * @param d the integer to convert
 * @return the Dirn for d
 */
inline Dirn from_id(unsigned int d){
  return static_cast<Dirn>(d);
}

/// Get the opposite Dirn to the given Dirn
/**
 * @param d the dirn to get the opposite of
 * @return the dirn pointing in the opposite direction
 */
inline Dirn opposite(Dirn d){
  return from_id((to_id(d)+3)%6);
}

/// Convert a Dirn to a bitmask
/**
 * The return value has the bit corresponding to the Dirn's id set and no others.
 * The reverse operation is not defined as a bitmask may in general store multiple dirns.
 * @param d the Dirn to convert
 * @return the bitmask for d
 */
inline int to_mask(Dirn d){
  return 1<<to_id(d);
}

/// Convert a Dirn to a Vector
/**
 * Please see the code for the convention used. Please note that this uses a right handed
 * coordinate system but many graphics renderers use a left handed system.
 * The reverse operation is not defined as a Vector may be a mixture of different amounts
 * of different directions.
 * @param d the Dirn to convert
 * @return the Vector for d
 */
inline Vector to_vector(Dirn d){
  switch(d){
    case UP:
      return Vector(0,1,0);
    case DOWN:
      return Vector(0,-1,0);
    case LEFT:
      return Vector(1,0,0);
    case RIGHT:
      return Vector(-1,0,0);
    case FORWARD:
      return Vector(0,0,1);
    case BACK:
      return Vector(0,0,-1);
    default:
      return Vector(0,0,0);
  }
}

/// Convert a Dirn to a shift Vector
/**
 * A shift vector has all components less than or equal to 0 and is related to the fact
 * that we treat one corner of the space as the origin. The reverse operation is not defined
 * @param d the Dirn to convert
 * @return the shift Vector for d
 */
inline Vector to_shift_vector(Dirn d){
  switch(d){
    case DOWN:
      return Vector(0,-1,0);
    case RIGHT:
      return Vector(-1,0,0);
    case BACK:
      return Vector(0,0,-1);
    default:
      return Vector(0,0,0);
  }
}

/// Get a vector perpendicular to the two provided Dirns
/**
 * The returned vector is always one of BACK, RIGHT or DOWN
 * (i.e. the ones that have a non-zero shift vector)
 * @param d a Dirn
 * @param e the other Dirn
 * @return a vector perpendicular to both d and e
 */
inline Dirn perpendicular(Dirn d,Dirn e){
  switch(d){
    case UP:
    case DOWN:
      switch(e){
        case FORWARD:
        case BACK:
          return RIGHT;
        case LEFT:
        case RIGHT:
          return BACK;
        default:
          return d;
      }
    case FORWARD:
    case BACK:
      switch(e){
        case UP:
        case DOWN:
          return RIGHT;
        case LEFT:
        case RIGHT:
          return DOWN;
        default:
          return d;
      }
    case LEFT:
    case RIGHT:
      switch(e){
        case UP:
        case DOWN:
          return BACK;
        case FORWARD:
        case BACK:
          return DOWN;
        default:
          return d;
      }
    default:
      return d;
  }
}

#ifdef IOSTREAM
///Print a representation of a direction
/**
 * Prints the id and vector form of the Dirn
 * @param o the stream to print the Dirn to
 * @param d the Dirn to print.
 * @return the stream o
 */
inline std::ostream& operator<<(std::ostream& o,Dirn d){
  return o<<"<Dirn "<<(int)d<<": "<<to_vector(d)<<">";
}
#endif

#endif

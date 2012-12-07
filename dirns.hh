#ifndef DIRNS_HH_INC
#define DIRNS_HH_INC

#include "vector.hh"

enum Dirn{
	UP=0,
	LEFT,
	FORWARD,
	DOWN,
	RIGHT,
	BACK
};

static const int ALLDIRNSMASK = (1<<6)-1;

unsigned int to_id(Dirn d){
	return static_cast<unsigned int>(d);
}
Dirn from_id(unsigned int d){
	return static_cast<Dirn>(d);
}

Dirn opposite(Dirn d){
  return from_id((to_id(d)+3)%6);
}
int to_mask(Dirn d){
	return 1<<to_id(d);
}

Vector to_vector(Dirn d){
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

Vector to_shift_vector(Dirn d){
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

Dirn perpendicular(Dirn d,Dirn e){
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
#endif
/**
 * @file maze.hh
 * @brief A Hypermaze class and classes to access the maze data
 */

#include "dirns.hh"
#include "vector.hh"
#include "SmartPointer.hh"
#include "hypio.hh"

#ifdef IOSTREAM
#include <iostream>
#endif

#ifndef MAZE_HH_INC
#define MAZE_HH_INC

class Point;
class ConstPoint;

/// A hypermaze
/**
 * This is the data for a hypermaze.
 */
class Maze
{
  private:
    /// The actual data stored as a flat array
    /**
     * The size of the maze is used to flatten a 3d coordinate to a 1d index.
     * Each value is a bitmask of Dirn. Any spare bits are free for other uses
     * (Currently the generator uses themwhile generating) but aren't written out
     * if the maze is saved
     */
    SPA<int> maze;
    /// The size of the maze
    Vector thesize;
  public:

    ///Create a new blank maze
    /**
     * This sets up the maze with a top and bottom but leaves the bulk empty
     * @param size the size of the maze to create
     */
    Maze(Vector size);
    /// Make a new maze that references the same data
    /**
     * Changes to the data in new maze will change the original.
     * @param m the maze to link with
     */
    Maze(Maze& m);
    ///Create a copy of the specified maze
    /**
     * The new maze will have a copy of the data in the orignal.
     * @param m the maze to copy
     */
    Maze(const Maze& m);

    /// Destroy this maze. Data is only freed if no other mazes are using it.
    ~Maze();

    /// Link this maze to the specified other maze
    /**
     * Changes to the data in new maze will change the original.
     * @param m the maze to link with
     * @return *this
     */
    Maze& operator=(const Maze& m);

    /// Make this maze a copy of the maze it currently contains
    inline void makeThisACopy(){
      (*this)=Maze((const Maze)(*this));
    }

    /// Get the size of this maze
    /**
     * @return the size of this maze
     */
    inline const Vector& size() const{
      return thesize;
    }

    ///Get pointer to the data for a point on the maze
    /**
     * @param p the point to get the data for
     * @return a Point object pointing to the data for the specified point
     */
    Point operator [](Vector p);
    ///Get a read only pointer to the data for a point on the maze
    /**
     * @param p the point to get the data for
     * @return a ConstPoint object pointing to the data for the specified point
     */
    ConstPoint operator [](Vector p) const;

    #ifdef IOSTREAM
    friend std::ostream& operator<<(std::ostream&,const Maze&);
    friend std::istream& operator>>(std::istream&,Maze&);
    #endif

    friend IOResult read(HypIStream& s,Maze& m);
    friend bool write(HypOStream& s,const Maze& m);

};

///Read a maze from an input stream
/**
 * @param s the stream to read from
 * @param m the maze to read into
 * @return the result of the read
 */
IOResult read(HypIStream& s,Maze& m);
///Write a maze to an output stream
/**
 * @param s the stream to write to
 * @param m the maze to write
 * @return the result of the write
 */
bool write(HypOStream& s,const Maze& m);

/// A pointer to a point in the maze
/**
 * This allows access and editing of the value in this point and navigation round the data.
 * No checks are made for moving out of the data or wrapping round dimensions.
 */
class Point{
  private:
    Vector size; ///< The size of the maze for moving the point around the maze
    SPA<int> point; ///< The current point this points to
  public:
    /// Create a new point.
    /**
     * This constructor is only called by the maze.
     * @param size the size of the maze
     * @param point the pointer to the point
     */
    Point(Vector size,SPA<int> point):size(size),point(point){};
    /// New point that is a copy of the orignal point.
    /**
     * @param p the point to copy
     */
    Point(const Point& p):size(p.size),point(p.point){};
    /// Make this point a copy of another point
    /**
     * @param p the point to copy
     * @return *this
     */
    Point& operator=(const Point& p){
      size=p.size;
      point=p.point;
      return *this;
    }
    /// Access the data for the point this points to
    /**
     * @return the value stored in this Point's target
     */
    inline int& operator*()const {return *point;}
    /// Get a new point that is shifted by d relative to this one.
    /**
     * No checks are made for moving out of the data or wrapping round dimensions.
     * @param d the distance to shift by
     * @return a new Point pointing to the new point
     */
    inline Point operator+(Vector d){
      return Point(size,point+(d.X+size.X*(d.Y+size.Y*d.Z)));
    }
    friend class ConstPoint;
};

/// A read only pointer to a point in the maze
/**
 * This allows access of the value in this point and navigation round the data.
 * No checks are made for moving out of the data or wrapping round dimensions.
 */
class ConstPoint{
  private:
    Vector size; ///< The size of the maze for moving the point around the maze
    SPA<int> point; ///< The current point this points to
  public:
    /// Create a new point.
    /**
     * This constructor is only called by the maze.
     * @param size the size of the maze
     * @param point the pointer to the point
     */
    ConstPoint(Vector size,SPA<int> point):size(size),point(point){};
    /// New point that is a copy of the orignal point.
    /**
     * @param p the point to copy
     */
    ConstPoint(const ConstPoint& p):size(p.size),point(p.point){};
    /// New point that is a copy of the orignal point.
    /**
     * @param p the point to copy
     */
    ConstPoint(const Point& p):size(p.size),point(p.point){};
    /// Make this point a copy of another point
    /**
     * @param p the point to copy
     * @return *this
     */
    ConstPoint& operator=(const ConstPoint& p){
      size=p.size;
      point=p.point;
      return *this;
    }
    /// Access the data for the point this points to
    /**
     * @return the value stored in this Point's target
     */
    inline const int& operator*() const {return *point;}
    /// Get a new point that is shifted by d relative to this one.
    /**
     * No checks are made for moving out of the data or wrapping round dimensions.
     * @param d the distance to shift by
     * @return a new Point pointing to the new point
     */
    inline ConstPoint operator+(Vector d){
      return ConstPoint(size,point+(d.X+size.X*(d.Y+size.Y*d.Z)));
    }
};

#ifdef IOSTREAM
///Print a representation of a maze
/**
 * Prints the size and data for a maze
 * @param o the stream to print the Maze to
 * @param m the Maze to print.
 * @return the stream o
 */
inline std::ostream& operator<<(std::ostream& o,const Maze& m){
  o<<m.size().X<<" "<<m.size().Y<<" "<<m.size().Z<<std::hex<<std::endl;
  SPA<int> p=m.maze;
  for(int i=0;i<m.size().X*m.size().Y*m.size().Z;++i,++p){
    o<<*p<<" ";
    #ifdef DEBUG
    if((i+1)%m.size().X==0)
      o<<std::endl;
    if((i+1)%(m.size().X*m.size().Y)==0)
      o<<std::endl;
    #endif
  }
  o<<std::dec<<std::endl;
  return o;
}

///Read a representation of a maze
/**
 * Read the size and data for a maze. The format is as output by operator<<(std::ostream&,const Maze&)
 * @param o the stream to read the Maze from
 * @param m the Maze to read the data into.
 * @return the stream o
 */
inline std::istream& operator>>(std::istream& o,Maze& m){
  Vector size;
  o>>size.X>>size.Y>>size.Z>>std::hex;
  m=Maze(size);
  SPA<int> p=m.maze;
  for(int i=0;i<m.thesize.X*m.thesize.Y*m.size().Z;++i,++p){
    o>>*p;
  }
  return o>>std::dec;
}

///Print a human reasable representation of a maze
/**
 * Prints the size and data for a maze
 * @param o the stream to print the Maze to
 * @param m the Maze to print.
 * @param w the maximum width of the screen to use
 */
void prettyPrint(std::ostream& o,Maze m,int w=150);
#endif

#endif

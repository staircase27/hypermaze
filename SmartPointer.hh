/**
 * @file
 * This file provides two smart pointer classes.
 */  


#ifndef SMARTPOINTER_HH_INC
#define SMARTPOINTER_HH_INC

#ifdef IOSTREAM
#include <iostream>
using namespace std;
#endif

///A Smart pointer class
/**
 * This class provides a basic smart pointer class that reference
 * tracks it's argument and deletes when no pointers are left
 * @tparam T the type of the data stored in the smart pointer
 */
template <class T>
class SP{
  protected:
    T* p;///< The data pointer
    int* c;///< The reference count
  public:
    ///Construct a new smart pointer pointing to null
    SP<T>():c(new int(1)),p(0){};
    ///Construct a smart pointer pointing to the provided data
    /**
     * The provided pointer should be a freshly created pointer that nothing else
     * should try to delete or access unless directly unless you ensure that a smart pointer is also kept
     * @param p the pointer to the data to be managed
     */
    explicit SP<T>(T* p):c(new int(1)),p(p){};

    ///Copy Constructor
    /**
     * Creates a new copy of the smart pointer refering to the same data and reference count and
     * increments the reference count
     * @param op the smart pointer to copy
     */
    SP<T>(const SP<T>& op):p(op.p),c(op.c){++*c;}

    ///Copy Constructor
    /**
     * Creates a new copy of the smart pointer refering to the same data and reference count and
     * increments the reference count
     * @param op the smart pointer to copy
     * @tparam U the type of the pointer we are copying
     */
    template <class U>
    SP<T>(const SP<U>& op):p(op.p),c(op.c){++*c;}
    
    template <class U>
    friend class SP;
    
    ///Destructor for pointer
    /**
     * Destroys the pointer and updates the reference count. If this is the last pointer then delete the
     * data we point to as well
     */
    ~SP<T>(){
      if(--*c==0){
        delete p;
        delete c;
      }
    }
    
    ///Assignment operator
    /**
     * Updates the reference count for the current data and deletes if needed and then replaces with
     * value pointed to by other pointer
     * @param op the pointer to replace this one with
     * @return a reference to this pointer
     */
    SP<T>& operator=(const SP<T>& op){
      if(p==op.p)
        return *this;
      if(--*c==0){
        delete p;
        delete c;
      }
      p=op.p;
      c=op.c;
      ++*c;
      
      return *this;
    }
    
    ///Dereference the pointer 
    /**
     * @return a reference to the element we point to
     */
    T& operator* () const{
      return *p;
    }

    ///Dereference the pointer to a member 
    /**
     * @return A pointer that can be used to access the member requested
     */
    T* operator-> () const{
      return p;
    }
    
    ///Compare this pointer to another pointer
    /**
     * Returns true if the other pointer points to the same memory location as this pointer
     * @param op the other pointer to compare to this pointer
     * @return true if they point to the same memory location
     */
    const bool operator==(const  SP<T>& op)const{
      return p==op.p;
    }
    
    ///Check if this is a null pointer
    /**
     * @return true if this is a null pointer
     */
    const bool isnull() const{
      return p==0;
    }

    #ifdef IOSTREAM
    template<class U>
    friend ostream& operator<<(ostream&,const SP<U>&);
    #endif

};

///A Smart array pointer class
/**
 * This class provides a more advanced smart pointer class that reference
 * tracks a c style array acting as a pointer to a location in the array
 * and deletes the entire array when no pointers are left
 * @tparam T the type of the data stored in the smart pointer
 */
template <class T>
class SPA{
  protected:
    T* p;///< The data pointer
    T* h;///< The head of the array
    int* c;///< The reference count
    
  public:
    ///Construct a new smart pointer pointing to null
    SPA<T>():c(new int(1)),p(0),h(0){};
    ///Construct a smart pointer pointing to the provided data
    /**
     * The provided pointer should be a freshly created pointer to a c style array that nothing else
     * should try to delete or access unless directly unless you ensure that a smart pointer is also kept
     * @param p the pointer to the data to be managed
     */
    explicit SPA<T>(T* p):c(new int(1)),p(p),h(p){};
    ///Copy Constructor
    /**
     * Creates a new copy of the smart pointer refering to the same data and reference count and
     * increments the reference count
     * @param op the smart pointer to copy
     */
    SPA<T>(const SPA<T>& op):p(op.p),c(op.c),h(op.h){++*c;}
    ///Copy Constructor
    /**
     * Creates a new copy of the smart pointer refering to the same data and reference count and
     * increments the reference count
     * @param op the smart pointer to copy
     * @tparam U the type of the pointer we are copying
     */
    template <class U>
    SPA<T>(const SPA<U>& op):p(op.p),c(op.c),h(op.h){++*c;}

    template <class U>
    friend class SPA;
    
    ///Destructor for pointer
    /**
     * Destroys the pointer and updates the reference count. If this is the last pointer then delete the
     * data we point to as well
     */
    ~SPA<T>(){
      if(--*c==0){
        delete[] h;
        delete c;
      }
    }
    
    ///Assignment operator
    /**
     * Updates the reference count for the current data and deletes if needed and then replaces with
     * value pointed to by other pointer
     * @param op the pointer to replace this one with
     * @return a reference to this pointer
     */
    SPA<T>& operator=(const SPA<T>& op){
      if(p==op.p)
        return *this;
      if(--*c==0){
        delete[] h;
        delete c;
      }
      p=op.p;
      c=op.c;
      h=op.h;
      ++*c;
      return *this;
    }

    ///Dereference the pointer 
    /**
     * @return a reference to the element we point to
     */
    T& operator* () const{
      return *p;
    }

    ///Dereference the pointer to a member 
    /**
     * @return A pointer that can be used to access the member requested
     */
    T* operator-> () const{
      return p;
    }
    ///Derefence the array pointer to an element relative to element we point to
    /**
     * @param i the position relative to the element this points to to return
     * @return a reference to the element
     */
    T& operator[](const int i) const{
      return p[i];
    }
    ///Get a new pointer to the element i after the one this points to
    /**
     * @param i the position relative to this pointers element we want to return a pointer to
     * @return a new pointer that points i elements after this one
     */
    SPA<T> operator+(const int i) const{
      return SPA<T>(*this)+=i;
    }
    ///Move this pointer to the element i after the one it currently points to
    /**
     * Updates the pointer it's called on and returns a reference to itself
     * @param i how many positions to move this pointer forwards by
     * @return a reference to this pointer
     */
    SPA<T>& operator+=(const int i){
      p+=i;
      return *this;
    }
    ///Get a new pointer to the element i before the one this points to
    /**
     * @param i the position relative to this pointers element we want to return a pointer to
     * @return a new pointer that points i elements before this one
     */
    SPA<T> operator-(const int i) const{
      return SPA<T>(*this)-=i;
    }
    ///Move this pointer to the element i before the one it currently points to
    /**
     * Updates the pointer it's called on and returns a reference to itself
     * @param i how many positions to move this pointer backwards by
     * @return a reference to this pointer
     */
    SPA<T>& operator-=(const int i){
      p-=i;
      return *this;
    }
    ///Increment this pointer by one
    /**
     * Updates this pointer and returns a reference to itself
     * @return a reference to this pointer
     */
    SPA<T>& operator++(){
      ++p;
      return *this;
    }
    ///Decrement this pointer by one
    /**
     * Updates this pointer and returns a reference to itself
     * @return a reference to this pointer
     */
    SPA<T>& operator--(){
      --p;
      return *this;
    }
    ///Increment this pointer by one and return a copy of the old value
    /**
     * Updates this pointer and returns a reference to itself
     * @return a reference to this pointer
     */
    SPA<T> operator++(int){
      SPA<T> tmp(*this);
      ++p;
      return tmp;
    }
    ///Decrement this pointer by one and return a copy of the old value
    /**
     * Updates this pointer and returns a reference to itself
     * @return a reference to this pointer
     */
    SPA<T> operator--(int){
      SPA<T> tmp(*this);
      --p;
      return tmp;
    }
    ///Compare this pointer to another pointer
    /**
     * Returns true if the other pointer points to the same memory location as this pointer
     * @param op the other pointer to compare to this pointer
     * @return true if they point to the same memory location
     */
    const bool operator==(const  SPA<T>& op)const{
      return h==op.h && p==op.p;
    }
    ///Compare this pointer to another pointer
    /**
     * Returns false if the other pointer points to the same memory location as this pointer
     * @param op the other pointer to compare to this pointer
     * @return true if they point to different memory location
     */
    const bool operator!=(const  SPA<T>& op)const{
      return h!=op.h || p!=op.p;
    }

    ///Check if this is a null pointer
    /**
     * @return true if this is a null pointer
     */
    const bool isnull() const{
      return p==0;
    }

    #ifdef IOSTREAM
    template<class U>
    friend ostream& operator<<(ostream&,const SPA<U>&);
    #endif

};

#ifdef IOSTREAM
template <class T>
ostream& operator<<(ostream& o,const SP<T>& p){
  return o<<"<SP: "<<p.p<<" ("<<*p.c<<")>";
}
template <class T>
ostream& operator<<(ostream& o,const SPA<T>& p){
  return o<<"<SPA: "<<p.p<<" from "<<(void*)p.h<<" ("<<*p.c<<")>";
}
#endif

#endif

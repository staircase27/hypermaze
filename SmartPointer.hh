/**
 * @file
 * This file provides two smart pointer classes.
 */  


#ifndef SMARTPOINTER_HH_INC
#define SMARTPOINTER_HH_INC


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
    SP<T>(T* p):c(new int(1)),p(p){};
    ///Copy Constructor
    /**
     * Creates a new copy of the smart pointer refering to the same data and reference count and
     * increments the reference count
     * @param op the smart pointer to copy
     * @tparam U the type of the pointer we are copying
     */
    template <class U>
    SP<T>(const SP<U>& op):p(op.p),c(op.c){++*c;}
    
    friend class SP<const T>;
    
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
     * @param the pointer to replace this one with
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
    T& operator* () const{
      return *p;
    }

    ///Dereference the pointer to a member 
    T* operator-> () const{
      return p;
    }
    
    ///Compare this pointer to another pointer
    /**
     * Returns true if the other pointer points to the same memory location as this pointer
     * @param op the other pointer to compare to this pointer
     */
    const bool operator==(const  SP<T>& op)const{
      return p==op.p;
    }
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
    SPA<T>(T* p):c(new int(1)),p(p),h(p){};
    ///Copy Constructor
    /**
     * Creates a new copy of the smart pointer refering to the same data and reference count and
     * increments the reference count
     * @param op the smart pointer to copy
     * @tparam U the type of the pointer we are copying
     */
    SPA<T>(const SPA<T>& op):p(op.p),c(op.c),h(op.h){++*c;}

    friend class SPA<const T>;
    
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
     * @param the pointer to replace this one with
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
    virtual T& operator* () const{
      return *p;
    }

    ///Dereference the pointer to a member 
    virtual T* operator-> () const{
      return p;
    }
    T& operator[](const int i){
      return p[i];
    }
    SPA<T> operator+(const int i) const{
      return SPA<T>(*this)+=i;
    }
    SPA<T>& operator+=(const int i){
      p+=i;
      return *this;
    }
    SPA<T> operator-(const int i) const{
      return SPA<T>(*this)-=i;
    }
    SPA<T>& operator-=(const int i){
      p-=i;
      return *this;
    }
    SPA<T>& operator++(){
      ++p;
      return *this;
    }
    SPA<T>& operator--(){
      --p;
      return *this;
    }
    ///Compare this pointer to another pointer
    /**
     * Returns true if the other pointer points to the same memory location as this pointer
     * @param op the other pointer to compare to this pointer
     */
    const bool operator==(const  SPA<T>& op)const{
      return h==op.h && p==op.p;
    }
    ///Compare this pointer to another pointer
    /**
     * Returns false if the other pointer points to the same memory location as this pointer
     * @param op the other pointer to compare to this pointer
     */
    const bool operator!=(const  SPA<T>& op)const{
      return h!=op.h && p!=op.p;
    }
};
#endif
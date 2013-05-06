#ifndef SMARTPOINTER_HH_INC
#define SMARTPOINTER_HH_INC

template <class T>
class SP{
  protected:
    T* p;
    int* c;
  public:
    SP<T>(T* p):c(new int(1)),p(p){ };
    template <class U>
    SP<T>(const SP<U>& op):p(op.p),c(op.c){++*c;}
    friend class SP<const T>;
    
    ~SP<T>(){
      if(--*c==0){
        delete p;
        delete c;
      }
    }
    
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
        
    T& operator* () const{
      return *p;
    }

    T* operator-> () const{
      return p;
    }
    
    const bool operator==(const  SP<T>& op)const{
      return p==op.p;
    }
};

template <class T>
class SPA{
  protected:
    T* p;
    T* h;
    int* c;
    
  public:
    SPA<T>(T* p):c(new int(1)),p(p),h(p){};
    SPA<T>(const SPA<T>& op):p(op.p),c(op.c),h(op.h){++*c;}
    friend class SPA<const T>;
    
    ~SPA<T>(){
      if(--*c==0){
        delete[] h;
        delete c;
      }
    }
    
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

    virtual T& operator* () const{
      return *p;
    }

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
    const bool operator==(const  SPA<T>& op)const{
      return h==op.h && p==op.p;
    }
    const bool operator!=(const  SPA<T>& op)const{
      return h!=op.h && p!=op.p;
    }
};
#endif
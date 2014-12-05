/* 
 * File:   sortedmap.hh
 * Author: armstrong
 *
 * Created on December 5, 2014, 12:18 PM
 */

#ifndef SORTEDMAP_HH
#define	SORTEDMAP_HH

template<class K,class V>
class Node{
  private:
    K k;
    V v;
  public:
    K getKey(){
      return k;
    }
    V getValue(){
      return v;
    }
    void setValue(V o){
      v=o;
    }
    Node(K k,V v):k(k),v(v){};
};

template<class k,class V>
class sortedmap {
private:
  class Node: ::Node<K,V>{
    Node* left;
    Node* right;
  };
public:
  sortedmap();
  sortedmap(const sortedmap& orig);
  virtual ~sortedmap();
private:

};

#endif	/* SORTEDMAP_HH */


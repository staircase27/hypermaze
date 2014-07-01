#include "script.hh"
#include "scriptimpl.hh"
#include <iostream>

using namespace std;

ostream& operator<<(ostream& s,SP<Condition> e);

ostream& operator<<(ostream& s,SP<ConditionTrue> e){
  return s<<"True";
}

ostream& operator<<(ostream& s,SP<ConditionNot> e){
  return s<<"Not "<<e->condition;
}

ostream& operator<<(ostream& s,SP<ConditionOr> e){
  s<<"Any of [";
  for(int i=0;i<e->count;++i){
    if(i!=0)
      s<<", ";
    s<<e->conditions[i];
  }
  return s<<"]";
}
ostream& operator<<(ostream& s,SP<ConditionAnd> e){
  s<<"All of [";
  for(int i=0;i<e->count;++i){
    if(i!=0)
      s<<", ";
    s<<e->conditions[i];
  }
  return s<<"]";
}
ostream& operator<<(ostream& s,SP<ConditionAfter> e){
  return s<<"At least "<<e->delay<<" after event "<<e->event;
}
ostream& operator<<(ostream& s,SP<ConditionBefore> e){
  return s<<"Before event "<<e->event;
}
ostream& operator<<(ostream& s,Pair<PatternTag,StringElementCondition>& p){
  s<<p.a.min<<"-"<<p.a.max<<"elements ";
  if(p.a.greedy)
    s<<"greedy ";
  s<<"matching ";
  if((p.b.selectionCondition&2)==0)
    if((p.b.selectionCondition&1)==0)
      s<<"not selected ";
    else
      s<<"not selected ";
  else
    s<<"any selection ";
  s<<"dirns:"<<p.b.dirnsCondition<<" ";
  s<<"located in (";
  if(p.b.xrange_count==0)
    s<<"any";
  else{
    if(p.b.xrange_count>1)
      s<<"{";
    for(int i=0;i<p.b.xrange_count;++i){
      if(i!=0)
        s<<", ";
      s<<p.b.xrange[i].start<<"-"<<p.b.xrange[i].end;
    }
    if(p.b.xrange_count>1)
      s<<"}";
  }
  s<<", ";
  if(p.b.yrange_count==0)
    s<<"any";
  else{
    if(p.b.yrange_count>1)
      s<<"{";
    for(int i=0;i<p.b.yrange_count;++i){
      if(i!=0)
        s<<", ";
      s<<p.b.yrange[i].start<<"-"<<p.b.yrange[i].end;
    }
    if(p.b.yrange_count>1)
      s<<"}";
  }
  s<<", ";
  if(p.b.zrange_count==0)
    s<<"any";
  else{
    if(p.b.zrange_count>1)
      s<<"{";
    for(int i=0;i<p.b.zrange_count;++i){
      if(i!=0)
        s<<", ";
      s<<p.b.zrange[i].start<<"-"<<p.b.zrange[i].end;
    }
    if(p.b.zrange_count>1)
      s<<"}";
  }
  return s<<")";
}
ostream& operator<<(ostream& s,SP<ConditionStringPattern> e){
  s<<"String Pattern [";
  for(int i=0;i<e->sm.count;++i){
    if(i!=0)
      s<<", ";
    s<<e->sm.pattern[i];
  }
  return s<<"]";
}


#define MAKECASE1(T) case T::id:\
                       s<<(SP<T>)e;\
                       break;
ostream& operator<<(ostream& s,SP<Condition> e){
  if(e.isnull())
    return s<<"A Condition NULL Pointer";
  switch(e->getid()){
    MAKECASE1(ConditionTrue)
    MAKECASE1(ConditionOr)
    MAKECASE1(ConditionAnd)
    MAKECASE1(ConditionNot)
    MAKECASE1(ConditionAfter)
    MAKECASE1(ConditionBefore)
    MAKECASE1(ConditionStringPattern)
    default:
      s<<"unknown condition";
  }
  return s;
}

using namespace std;

int main(){
  Script s;
  cout<<&s<<endl;
  ConditionOr* cor=new ConditionOr();
  SP<Condition> test(cor);
  cor->conditions=SPA<SP<Condition> >(2);
  cor->count=2;
  cor->conditions[0]=SP<Condition>(new ConditionAfter());
  ConditionNot* cn=new ConditionNot();
  cn->condition=SP<Condition>(new ConditionBefore());
  cor->conditions[1]=SP<Condition>(cn);
  cout<<test<<endl;
}



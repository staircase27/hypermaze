#define IOSTREAM
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

ostream& operator<<(ostream& s,const StringElementCondition& b){
  if((b.selectionCondition&2)==0)
    if((b.selectionCondition&1)==0)
      s<<"not selected ";
    else
      s<<"not selected ";
  else
    s<<"any selection ";
  s<<"dirns:"<<b.dirnsCondition<<" ";
  s<<"located in (";
  if(b.xrange_count==0)
    s<<"any";
  else{
    if(b.xrange_count>1)
      s<<"{";
    for(int i=0;i<b.xrange_count;++i){
      if(i!=0)
        s<<", ";
      s<<b.xrange[i].start<<"-"<<b.xrange[i].end;
    }
    if(b.xrange_count>1)
      s<<"}";
  }
  s<<", ";
  if(b.yrange_count==0)
    s<<"any";
  else{
    if(b.yrange_count>1)
      s<<"{";
    for(int i=0;i<b.yrange_count;++i){
      if(i!=0)
        s<<", ";
      s<<b.yrange[i].start<<"-"<<b.yrange[i].end;
    }
    if(b.yrange_count>1)
      s<<"}";
  }
  s<<", ";
  if(b.zrange_count==0)
    s<<"any";
  else{
    if(b.zrange_count>1)
      s<<"{";
    for(int i=0;i<b.zrange_count;++i){
      if(i!=0)
        s<<", ";
      s<<b.zrange[i].start<<"-"<<b.zrange[i].end;
    }
    if(b.zrange_count>1)
      s<<"}";
  }
  return s<<")";
}

ostream& operator<<(ostream& s,Pair<PatternTag,StringElementCondition>& p){
  s<<p.a.min<<"-"<<p.a.max<<"elements ";
  if(p.a.greedy)
    s<<"greedy ";
  return s<<"matching "<<p.b;
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

#define MAKECASE(T) case T::id:\
                       s<<(SP<T>)e;\
                       break;
ostream& operator<<(ostream& s,SP<Condition> e){
  if(e.isnull())
    return s<<"A Condition NULL Pointer";
  switch(e->getid()){
    MAKECASE(ConditionTrue)
    MAKECASE(ConditionOr)
    MAKECASE(ConditionAnd)
    MAKECASE(ConditionNot)
    MAKECASE(ConditionAfter)
    MAKECASE(ConditionBefore)
    MAKECASE(ConditionStringPattern)
    default:
      s<<"unknown condition";
  }
  return s;
}
#undef MAKECASE


ostream& operator<<(ostream& s,SP<Action> e);

ostream& operator<<(ostream& s,SP<ActionMulti> e){
  s<<"Do all of [";
  for(int i=0;i<e->num;++i){
    if(i!=0)
      s<<", ";
    s<<e->actions[i];
  }
  return s<<"]";
}

ostream& operator<<(ostream& s,SP<ActionNothing> e){
  return s<<"Nothing";
}

ostream& operator<<(ostream& s,const Message& m){
  s<<"[";
  for(int i=0;i<m.count;++i){
    if(i!=0)
      s<<", \n";
    s<<m.paragraphs[i].a<<": \""<<m.paragraphs[i].b<<"\"";
  }
  return s<<"]";
}

ostream& operator<<(ostream& s,SP<ActionMessage> e){
  return s<<"Show message: "<<e->m;
}

ostream& operator<<(ostream& s,SP<ActionBlockWin> e){
  return s<<"Block Win";
}

ostream& operator<<(ostream& s,SP<ActionWinMessage> e){
  return s<<"Show win message: "<<e->m;
}

ostream& operator<<(ostream& s,SP<ActionWinNextLevel> e){
  return s<<"Next Level of \""<<e->nextLevel.a<<"\" called \""<<e->nextLevel.b<<"\"";
}

ostream& operator<<(ostream& s,SP<ActionForceWin> e){
  return s<<"Force Win";
}

ostream& operator<<(ostream& s,SP<ActionStringConditionSelect> e){
  return s<<"Set selection status for string matching "<<e->change<<" to "<<e->select;
}

ostream& operator<<(ostream& s,SP<ActionSetStringRoute> e){
  return s<<"Set route for string matching pattern [";
  for(int i=0;i<e->ranges.count;++i){
    if(i!=0)
      s<<", ";
    s<<e->ranges.pattern[i];
  }
  s<<"] element";
  if(e->ranges.group_count!=1)
    s<<"s [";
  else
    s<<" ";
  for(int i=0;i<e->ranges.group_count;++i){
    if(i!=0)
      s<<", ";
    s<<e->ranges.groups[i].a<<"-"<<e->ranges.groups[i].b;
  }
  if(e->ranges.group_count!=1)
    s<<"]";
  s<<" to [";
  for(int i=0;i<e->count;++i){
    if(i!=0)
      s<<", ";
    s<<e->route[i];
  }
  s<<"]";
  if(e->all)
    s<<" repeatedly";
  return s;
}

#define MAKECASE(T) case T::id:\
                       s<<(SP<T>)e;\
                       break;
ostream& operator<<(ostream& s,SP<Action> e){
  if(e.isnull())
    return s<<"An Action NULL Pointer";
  switch(e->getid()){
    MAKECASE(ActionMulti)
    MAKECASE(ActionNothing)
    MAKECASE(ActionMessage)
    MAKECASE(ActionBlockWin)
    MAKECASE(ActionWinMessage)
    MAKECASE(ActionWinNextLevel)
    MAKECASE(ActionForceWin)
    MAKECASE(ActionStringConditionSelect)
    MAKECASE(ActionSetStringRoute)
    default:
      s<<"unknown action";
  }
  return s;
}
#undef MAKECASE

ostream& operator<<(ostream& s,const Event& e){
  return s<<"Event triggered on "<<e.trigger<<" if "<<e.condition<<" to do "<<e.action;
}

ostream& operator<<(ostream& s,const Script& e){
  s<<"Script: [";
  for(int i=0;i<e.geteventcount();++i){
    if(i!=0)
      s<<", ";
    s<<e.getevents()[i];
  }
  return s<<"]";
}


int main(){
  Script s;
  cout<<s<<endl;
  ConditionOr* cor=new ConditionOr();
  SP<Condition> test(cor);
  cor->conditions=SPA<SP<Condition> >(2);
  cor->count=2;
  cor->conditions[0]=SP<Condition>(new ConditionAfter());
  ConditionNot* cn=new ConditionNot();
  cn->condition=SP<Condition>(new ConditionBefore());
  cor->conditions[1]=SP<Condition>(cn);
  cout<<test<<endl;
  SPA<Event> c(1);
  c[0].condition=test;
  s=Script(1,c);
  cout<<s;
}



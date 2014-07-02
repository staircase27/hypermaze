#define IOSTREAM
#include "script.hh"
#include "scriptimpl.hh"
#include "hypioimp.hh"
#include <iostream>
#include <fstream>

using namespace std;

ostream& operator<<(ostream& s,SP<Condition> e);

ostream& operator<<(ostream& s,const ConditionTrue& e){
  return s<<"True";
}

ostream& operator<<(ostream& s,const ConditionNot& e){
  return s<<"Not "<<e.condition;
}

ostream& operator<<(ostream& s,const ConditionOr& e){
  s<<"Any of [";
  for(int i=0;i<e.count;++i){
    if(i!=0)
      s<<", ";
    s<<e.conditions[i];
  }
  return s<<"]";
}
ostream& operator<<(ostream& s,const ConditionAnd& e){
  s<<"All of [";
  for(int i=0;i<e.count;++i){
    if(i!=0)
      s<<", ";
    s<<e.conditions[i];
  }
  return s<<"]";
}
ostream& operator<<(ostream& s,const ConditionAfter& e){
  return s<<"At least "<<e.delay<<" after event "<<e.event;
}
ostream& operator<<(ostream& s,const ConditionBefore& e){
  return s<<"Before event "<<e.event;
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

ostream& operator<<(ostream& s,const ConditionStringPattern& e){
  s<<"String Pattern [";
  for(int i=0;i<e.sm.count;++i){
    if(i!=0)
      s<<", ";
    s<<e.sm.pattern[i];
  }
  return s<<"]";
}

#define MAKECASE(T) case T::id:\
                       s<<(T&)*e;\
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

ostream& operator<<(ostream& s,const ActionMulti& e){
  s<<"Do all of [";
  for(int i=0;i<e.num;++i){
    if(i!=0)
      s<<", ";
    s<<e.actions[i];
  }
  return s<<"]";
}

ostream& operator<<(ostream& s,const ActionNothing& e){
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

ostream& operator<<(ostream& s,const ActionMessage& e){
  return s<<"Show message: "<<e.m;
}

ostream& operator<<(ostream& s,const ActionBlockWin& e){
  return s<<"Block Win";
}

ostream& operator<<(ostream& s,const ActionWinMessage& e){
  return s<<"Show win message: "<<e.m;
}

ostream& operator<<(ostream& s,const ActionWinNextLevel& e){
  return s<<"Next Level of \""<<e.nextLevel.a<<"\" called \""<<e.nextLevel.b<<"\"";
}

ostream& operator<<(ostream& s,const ActionForceWin& e){
  return s<<"Force Win";
}

ostream& operator<<(ostream& s,const ActionStringConditionSelect& e){
  return s<<"Set selection status for string matching "<<e.change<<" to "<<e.select;
}

ostream& operator<<(ostream& s,const ActionSetStringRoute& e){
  s<<"Set route for string matching pattern [";
  for(int i=0;i<e.ranges.count;++i){
    if(i!=0)
      s<<", ";
    s<<e.ranges.pattern[i];
  }
  s<<"] element";
  if(e.ranges.group_count!=1)
    s<<"s [";
  else
    s<<" ";
  for(int i=0;i<e.ranges.group_count;++i){
    if(i!=0)
      s<<", ";
    s<<e.ranges.groups[i].a<<"-"<<e.ranges.groups[i].b;
  }
  if(e.ranges.group_count!=1)
    s<<"]";
  s<<" to [";
  for(int i=0;i<e.count;++i){
    if(i!=0)
      s<<", ";
    s<<e.route[i];
  }
  s<<"]";
  if(e.all)
    s<<" repeatedly";
  return s;
}

#define MAKECASE(T) case T::id:\
                       s<<(T&)*e;\
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

/*
bool edit(Event& e){
  char c='p';
  bool changed=false;
  while(c!='d'){
    switch(c){
      case 'p':
        cout<<e<<endl;
        break;
    }
    cout<<"Please select an action:"<<endl<<"p) Print"<<endl<<"r) Resize"<<endl<<"e) Edit an event"<<endl<<"d) Done with this item"<<endl<<": ";
    cin>>c;
  }
  return changed;
}
*/

bool edit(SP<Action>&);

bool edit(ActionMulti& a){
  char c='p';
  bool changed=false;
  while(c!='d'){
    switch(c){
      case 'p':
        cout<<a<<endl;
        break;
      case 'r':{
        cout<<"currently there are "<<a.num<<" Actions's"<<endl<<"how many would you like there to be? ";
        int newlen=-1;
        c=' ';
        cin>>newlen;
        while(newlen<0){
          cout<<"must be positive"<<endl<<": ";
          cin>>newlen;
        }
        while(c!='y'&&c!='n'){
          cout<<"are you sure you want to change the length to "<<newlen<<"? y/n: ";
          cin>>c;
        }
        if(c=='n')
          break;
        if(newlen==a.num)
          break;
        SPA<SP<Action> > newactions(newlen);
        for(int i=0;i<newlen&&i<a.num;++i)
          newactions[i]=a.actions[i];
        if(newlen>a.num)
          for(int i=a.num;i<newlen;++i)
            newactions[i]=Action::defaultvalue;
        a.num=newlen;
        a.actions=newactions;
        changed=true;
        break;
      }
      case 'e':{
        cout<<"which action do you wish to edit? ";
        int n;
        cin>>n;
        while(n>=a.num||n<0){
          cout<<"no such element: ";
          cin>>n;
        }
        changed|=edit(a.actions[n]);
        break;
      }
    }
    cout<<"Please select an action to use on this Multi Action:"<<endl<<"p) Print"<<endl<<"r) Resize action list"<<endl<<"e) Edit an Action"<<endl<<"d) Done with this item"<<endl<<": ";
    cin>>c;
  }
  return changed;
}
bool edit(ActionNothing& e){
  cout<<"nothing to edit for a nothing action"<<endl;
  return false;
}
bool edit(ActionMessage& e){
  char c='p';
  bool changed=false;
  while(c!='d'){
    switch(c){
      case 'p':
        cout<<e<<endl;
        break;
    }
    cout<<"Please select an action:"<<endl<<"p) Print"<<endl<<"r) Resize"<<endl<<"e) Edit an event"<<endl<<"d) Done with this item"<<endl<<": ";
    cin>>c;
  }
  return changed;
}
bool edit(ActionBlockWin& e){
  cout<<"nothing to edit for a block win action"<<endl;
  return false;
}
bool edit(ActionWinMessage& e){
  char c='p';
  bool changed=false;
  while(c!='d'){
    switch(c){
      case 'p':
        cout<<e<<endl;
        break;
    }
    cout<<"Please select an action:"<<endl<<"p) Print"<<endl<<"r) Resize"<<endl<<"e) Edit an event"<<endl<<"d) Done with this item"<<endl<<": ";
    cin>>c;
  }
  return changed;
}
bool edit(ActionWinNextLevel& e){
  char c='p';
  bool changed=false;
  while(c!='d'){
    switch(c){
      case 'p':
        cout<<e<<endl;
        break;
    }
    cout<<"Please select an action:"<<endl<<"p) Print"<<endl<<"r) Resize"<<endl<<"e) Edit an event"<<endl<<"d) Done with this item"<<endl<<": ";
    cin>>c;
  }
  return changed;
}
bool edit(ActionForceWin& e){
  cout<<"nothing to edit for a force win action"<<endl;
  return false;
}
bool edit(ActionStringConditionSelect& e){
  char c='p';
  bool changed=false;
  while(c!='d'){
    switch(c){
      case 'p':
        cout<<e<<endl;
        break;
    }
    cout<<"Please select an action:"<<endl<<"p) Print"<<endl<<"r) Resize"<<endl<<"e) Edit an event"<<endl<<"d) Done with this item"<<endl<<": ";
    cin>>c;
  }
  return changed;
}
bool edit(ActionSetStringRoute& a){
  char c='p';
  bool changed=false;
  while(c!='d'){
    switch(c){
      case 'p':
        cout<<a<<endl;
        break;
    }
    cout<<"Please select an action:"<<endl<<"p) Print"<<endl<<"r) Resize"<<endl<<"e) Edit an event"<<endl<<"d) Done with this item"<<endl<<": ";
    cin>>c;
  }
  return changed;
}
#define PRINTIDNAME(T) cout<<T::id<<") "<<#T<<endl;
#define MAKEREPCASE(T) case T::id:\
                         p=SP<T>(new T());\
                         break;
#define MAKEEDITCASE(T) case T::id:\
                          changed!=edit((T&)*p);\
                          break;
bool edit(SP<Action>& p){
  char c='p';
  bool changed=false;
  while(c!='d'){
    switch(c){
      case 'p':
        cout<<p<<endl;
        break;
      case 'r':{
        cout<<"choose a new action"<<endl;
        PRINTIDNAME(ActionMulti)
        PRINTIDNAME(ActionNothing)
        PRINTIDNAME(ActionMessage)
        PRINTIDNAME(ActionBlockWin)
        PRINTIDNAME(ActionWinMessage)
        PRINTIDNAME(ActionWinNextLevel)
        PRINTIDNAME(ActionForceWin)
        PRINTIDNAME(ActionStringConditionSelect)
        PRINTIDNAME(ActionSetStringRoute)
        int i;
        bool done=false;
        while(!done){
          cout<<": ";
          cin>>i;
          done=true;
          switch(i){
            MAKEREPCASE(ActionMulti)
            MAKEREPCASE(ActionNothing)
            MAKEREPCASE(ActionMessage)
            MAKEREPCASE(ActionBlockWin)
            MAKEREPCASE(ActionWinMessage)
            MAKEREPCASE(ActionWinNextLevel)
            MAKEREPCASE(ActionForceWin)
            MAKEREPCASE(ActionStringConditionSelect)
            MAKEREPCASE(ActionSetStringRoute)
            default:
              done=false;
              cout<<"Unknown Action"<<endl;
          }
        }
        changed=true;
        break;
      }
      case 'e':
        switch(p->getid()){
          MAKEEDITCASE(ActionMulti)
          MAKEEDITCASE(ActionNothing)
          MAKEEDITCASE(ActionMessage)
          MAKEEDITCASE(ActionBlockWin)
          MAKEEDITCASE(ActionWinMessage)
          MAKEEDITCASE(ActionWinNextLevel)
          MAKEEDITCASE(ActionForceWin)
          MAKEEDITCASE(ActionStringConditionSelect)
          MAKEEDITCASE(ActionSetStringRoute)
          default:
            cout<<"unknown action can't edit"<<endl;
        }
        break;
    }
    cout<<"Please select an action to use on this Action:"<<endl<<"p) Print"<<endl<<"r) Replace"<<endl<<"e) Edit"<<endl<<"d) Done with this item"<<endl<<": ";
    cin>>c;
  }
  return changed;
}
bool edit(SP<Condition>& p){
  char c='p';
  bool changed=false;
  while(c!='d'){
    switch(c){
      case 'p':
        cout<<p<<endl;
        break;
    }
    cout<<"Please select an action to use on this Condition:"<<endl<<"p) Print"<<endl<<"r) Replace"<<endl<<"e) Edit"<<endl<<"d) Done with this item"<<endl<<": ";
    cin>>c;
  }
  return changed;
}
bool edit(Event& e){
  char c='p';
  bool changed=false;
  while(c!='d'){
    switch(c){
      case 'p':
        cout<<e<<endl;
        break;
      case 't':{
        cout<<"please input new trigger: ";
        cin>>e.trigger;
        changed=true;
        break;
      }
      case 'c':
        changed|=edit(e.condition);
        break;
      case 'a':
        changed|=edit(e.action);
        break;
    }
    cout<<"Please select an action to use on this Event:"<<endl<<"p) Print"<<endl<<"t) Change trigger"<<endl<<"c) Edit the condition"<<endl<<"a) Edit the action"<<endl<<"d) Done with this item"<<endl<<": ";
    cin>>c;
  }
  return changed;
}

bool edit(Script& s){
  char c='p';
  bool changed=false;
  while(c!='d'){
    switch(c){
      case 'p':
        cout<<s<<endl;
        break;
      case 'r':{
        cout<<"currently there are "<<s.geteventcount()<<" event's"<<endl<<"how many would you like there to be? ";
        int newlen=-1;
        c=' ';
        cin>>newlen;
        while(newlen<0){
          cout<<"must be positive"<<endl<<": ";
          cin>>newlen;
        }
        while(c!='y'&&c!='n'){
          cout<<"are you sure you want to change the length to "<<newlen<<"? y/n: ";
          cin>>c;
        }
        if(c=='n')
          break;
        if(newlen==s.geteventcount())
          break;
        SPA<Event> newevents(newlen);
        for(int i=0;i<newlen&&i<s.geteventcount();++i)
          newevents[i]=s.getevents()[i];
        s=Script(newlen,newevents);
        changed=true;
        break;
      }
      case 'e':{
        cout<<"which event do you wish to edit? ";
        int n;
        cin>>n;
        while(n>=s.geteventcount()||n<0){
          cout<<"no such element: ";
          cin>>n;
        }
        changed|=edit(s.getevents()[n]);
        break;
      }
    }
    cout<<"Please select an action to use on this Script:"<<endl<<"p) Print"<<endl<<"r) Resize"<<endl<<"e) Edit an event"<<endl<<"d) Done with this item"<<endl<<": ";
    cin>>c;
  }
  return changed;
}

void edit(){
  char c='p';
  Script s;
  bool changed;
  char* fname=new char[256];
  while(true){
    switch(c){
      case 'p':
        cout<<"Current script is: "<<s<<endl;
        break;
      case 'l':{
        if(changed){
          cout<<"The current script has unsaved changes. Are you sure you want to load a new one? (y/n) ";
          cin>>c;
          while(c!='y' && c!='n'){
            cout<<"invalid response. ";
            cin>>c;
          }
          if(c=='n')
            break;
        }
        cout<<"please enter the filename: ";
        cin>>ws;
        cin.getline(fname,256);
        cout<<"opening "<<fname<<endl;
        ifstream is(fname);
        if(!is.is_open()){
          cout<<"error opening file"<<endl;
          break;
        }
        CPPHypIStream ihs(is);
        read(ihs,s);
        cout<<"new script loaded: "<<s<<endl;
        break;
      }
      case 'n':
        if(changed){
          cout<<"The current script has unsaved changes. Are you sure you want to create a new one? (y/n) ";
          cin>>c;
          while(c!='y' && c!='n'){
            cout<<"invalid response. ";
            cin>>c;
          }
          if(c=='n')
            break;
        }
        s=Script();
        break;
      case 's':{
        char* tmp=new char[256];
        cout<<"Current filename is "<<fname<<"please enter the filename or leave blank to use same name: ";
        cin>>ws;
        cin.getline(tmp,256);
        if(strlen(tmp)!=0){
          delete[] fname;
          fname=tmp;
        }else{
          delete[] tmp;
        }
        cout<<"saving to "<<fname<<endl;
        ofstream os(fname);
        if(!os.is_open()){
          cout<<"error opening file"<<endl;
          break;
        }
        CPPHypOStream ohs(os);
        write(ohs,s);
        cout<<"script saved"<<endl;
        changed=false;
        break;
      }
      case 'e':
        changed|=edit(s);
        break;
      case 'd':
        if(changed){
          cout<<"The current script has unsaved changes. Are you sure you want to create a new one? (y/n) ";
          cin>>c;
          while(c!='y' && c!='n'){
            cout<<"invalid response. ";
            cin>>c;
          }
          if(c=='n')
            break;
        }
        return;
    }
    cout<<"What would you like to do?"<<endl<<"p) Print the current Script"<<endl<<"l) Load a script from file"<<endl<<"n) Create a new empty script"<<endl<<"s) Save the current script"<<endl<<"e) Edit the current script"<<endl<<"d) Done with editing"<<endl<<": ";
    cin>>c;
  }      
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
  cout<<s<<endl;
  cout<<"STARTING EDIT"<<endl;
  cout<<edit(s);
  edit();
}



#include "../core/script.hh"
#include "../core/scriptimpl.hh"
#include "../shared/cpphypioimp.hh"
#include "../core/maze.hh"
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

ostream& operator<<(ostream& s,const Range& r){
  if(r.start==INT_MAX)
    s<<'*';
  else
    s<<r.start;
  s<<'-';
  if(r.end==INT_MAX)
    s<<'*';
  else
    s<<r.end;
  return s;
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
      s<<b.xrange[i];
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
      s<<b.yrange[i];
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
      s<<b.zrange[i];
    }
    if(b.zrange_count>1)
      s<<"}";
  }
  return s<<")";
}

ostream& operator<<(ostream& s,Pair<PatternTag,StringElementCondition>& p){
  s<<p.a.min<<"-"<<p.a.max<<" elements ";
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
  s<<"Event triggered on ";
  bool hastrigger=false;
  if((e.trigger & TRIGGER_START)!=0){
    if(hastrigger)
      s<<", ";
    s<<"start";
    hastrigger=true;
  }
  if((e.trigger & TRIGGER_WIN)!=0){
    if(hastrigger)
      s<<", ";
    s<<"win";
    hastrigger=true;
  }
  if((e.trigger & TRIGGER_MOVE)!=0){
    if(hastrigger)
      s<<", ";
    s<<"move";
    hastrigger=true;
  }
  if((e.trigger & TRIGGER_SELECT)!=0){
    if(hastrigger)
      s<<", ";
    s<<"select";
    hastrigger=true;
  }
  if(!hastrigger)
    s<<"never";
  return s<<" if "<<e.condition<<" to do "<<e.action;
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

bool edit(Range& r){
  cout<<"Enter the start and end seperated by spaces (* means no limit): ";
  while(true){
    cin>>ws;
    if(cin.peek()=='*'){
      cin.ignore(1);
      r.start=INT_MAX;
    }else{
      cin>>r.start;
    }
    cin>>ws;
    if(cin.peek()=='*'){
      cin.ignore(1);
       r.end=INT_MAX;
    }else{
      cin>>r.end;
    }
    if(!cin.fail())
      return true;
    cin.clear();
    cin.ignore(100,'\n');
    cout<<"invalid input"<<endl<<": ";
  }
}

bool edit(SPA<Range>& r,int& num){
  char c='p';
  bool changed=false;
  while(c!='d'){
    switch(c){
      case 'p':
        cout<<"[";
        for(int i=0;i<num;++i)
          cout<<r[i];
        cout<<"]"<<endl;
        break;
      case 'r':{
        cout<<"currently there are "<<num<<" Ranges"<<endl<<"how many would you like there to be? ";
        int newlen=-1;
        c=' ';
        cin>>newlen;
        while(newlen<0||!cin.good()){
          cout<<"must be positive integer"<<endl<<": ";
          cin.clear();
          cin.ignore(100,'\n');
          cin>>newlen;
        }
        while(c&&c!='y'&&c!='n'){
          cout<<"are you sure you want to change the length to "<<newlen<<"? y/n: ";
          cin>>c;
        }
        if(c=='n')
          break;
        if(newlen==num)
          break;
        SPA<Range> newr(newlen);
        for(int i=0;i<newlen&&i<num;++i)
          newr[i]=r[i];
        num=newlen;
        r=newr;
        changed=true;
        break;
      }
      case 'e':{
        if(c<=0){
          cout<<"none to edit"<<endl;
          break;
        }
        cout<<"which range do you wish to edit? ";
        int n;
        cin>>n;
        while(n>=num||n<0){
          cout<<"no such element: ";
          cin>>n;
        }
        changed|=edit(r[n]);
        break;
      }
      default: cout<<"invalid input \""<<c<<"\""<<endl;
    }
    cout<<"Please select an action to use on this list of ranges:"<<endl<<"p) Print"<<endl<<"r) Resize list of ranges"<<endl<<"e) Edit a range"<<endl<<"d) Done with this item"<<endl<<": ";
    cin>>c;
    if(cin.eof())
      break;
  }
  return changed;
}

bool edit(StringElementCondition& e){
  char c='p';
  bool changed=false;
  while(c!='d'){
    switch(c){
      case 'p':
        cout<<e<<endl;
        break;
      case 's':
        cout<<"what selection status condition do you want?"<<endl<<"0) Not selected"<<endl<<"1) Selected"<<endl<<"2) Don't care"<<endl<<": ";
        cin>>e.selectionCondition;
        while(cin.fail()){
          cout<<"invalid input: ";
          cin.clear();
          cin.ignore(100,'\n');
          cin>>e.selectionCondition;
        }
        changed=true;
        break;
      case 'd':
        cout<<"what dirns do you want?"<<endl<<": ";
        cin>>e.dirnsCondition;
        while(cin.fail()){
          cout<<"invalid input: ";
          cin.clear();
          cin.ignore(100,'\n');
          cin>>e.dirnsCondition;
        }
        changed=true;
        break;
      case 'x':
        changed|=edit(e.xrange,e.xrange_count);
        break;
      case 'y':
        changed|=edit(e.yrange,e.yrange_count);
        break;
      case 'z':
        changed|=edit(e.zrange,e.zrange_count);
        break;
      default: cout<<"invalid input \""<<c<<"\""<<endl;
    }
    cout<<"Please select an action:"<<endl<<"p) Print"<<endl<<"s) Edit Selection status condition"<<endl<<"d) Edit the Dirn's mask"<<endl<<"x/y/z) Edit the range of locations in the x/y/z direction"<<endl<<"d) Done with this item"<<endl<<": ";
    cin>>c;
    if(cin.eof())
      break;
  }
  return changed;
}

bool edit(PatternTag& r){
  cout<<"Enter the min and max repeates seperated by spaces: ";
  while(true){
    cin>>r.min;
    cin>>r.max;
    if(!cin.fail())
      break;
    cin.clear();
    cin.ignore(100,'\n');
    cout<<"invalid input"<<endl<<": ";
  }
  cout<<"Is this pattern element greedy? (y/n): ";
  char c;
  cin>>c;
  while(c&&c!='n'&&c!='y'){
    cout<<"enter y or n: ";
    cin>>c;
  }
  r.greedy=(c=='y');
  return true;
}

bool edit(SP<Condition>&);

bool edit(ConditionTrue&){
  cout<<"nothing to edit for a always true condition"<<endl;
  return false;
}

bool edit(ConditionOr& a){
  char c='p';
  bool changed=false;
  while(c!='d'){
    switch(c){
      case 'p':
        cout<<a<<endl;
        break;
      case 'r':{
        cout<<"currently there are "<<a.count<<" Conditions's"<<endl<<"how many would you like there to be? ";
        int newlen=-1;
        c=' ';
        cin>>newlen;
        while(newlen<0||!cin.good()){
          cout<<"must be positive integer"<<endl<<": ";
          cin.clear();
          cin.ignore(100,'\n');
          cin>>newlen;
        }
        while(c&&c!='y'&&c!='n'){
          cout<<"are you sure you want to change the length to "<<newlen<<"? y/n: ";
          cin>>c;
        }
        if(c=='n')
          break;
        if(newlen==a.count)
          break;
        SPA<SP<Condition> > newcondition(newlen);
        for(int i=0;i<newlen&&i<a.count;++i)
          newcondition[i]=a.conditions[i];
        if(newlen>a.count)
          for(int i=a.count;i<newlen;++i)
            newcondition[i]=Condition::defaultvalue;
        a.count=newlen;
        a.conditions=newcondition;
        changed=true;
        break;
      }
      case 'e':{
        if(a.count<=0){
          cout<<"none to edit"<<endl;
          break;
        }
        cout<<"which condition do you wish to edit? ";
        int n;
        cin>>n;
        while(n>=a.count||n<0){
          cout<<"no such element: ";
          cin>>n;
        }
        changed|=edit(a.conditions[n]);
        break;
      }
      default: cout<<"invalid input \""<<c<<"\""<<endl;
    }
    cout<<"Please select an action to use on this Or Condition:"<<endl<<"p) Print"<<endl<<"r) Resize action list"<<endl<<"e) Edit an Condition"<<endl<<"d) Done with this item"<<endl<<": ";
    cin>>c;
    if(cin.eof())
      break;
  }
  return changed;
}

bool edit(ConditionAnd& a){
  char c='p';
  bool changed=false;
  while(c!='d'){
    switch(c){
      case 'p':
        cout<<a<<endl;
        break;
      case 'r':{
        cout<<"currently there are "<<a.count<<" Conditions's"<<endl<<"how many would you like there to be? ";
        int newlen=-1;
        c=' ';
        cin>>newlen;
        while(newlen<0||!cin.good()){
          cout<<"must be positive integer"<<endl<<": ";
          cin.clear();
          cin.ignore(100,'\n');
          cin>>newlen;
        }
        while(c&&c!='y'&&c!='n'){
          cout<<"are you sure you want to change the length to "<<newlen<<"? y/n: ";
          cin>>c;
        }
        if(c=='n')
          break;
        if(newlen==a.count)
          break;
        SPA<SP<Condition> > newcondition(newlen);
        for(int i=0;i<newlen&&i<a.count;++i)
          newcondition[i]=a.conditions[i];
        if(newlen>a.count)
          for(int i=a.count;i<newlen;++i)
            newcondition[i]=Condition::defaultvalue;
        a.count=newlen;
        a.conditions=newcondition;
        changed=true;
        break;
      }
      case 'e':{
        if(a.count<=0){
          cout<<"none to edit"<<endl;
          break;
        }
        cout<<"which condition do you wish to edit? ";
        int n;
        cin>>n;
        while(n>=a.count||n<0){
          cout<<"no such element: ";
          cin>>n;
        }
        changed|=edit(a.conditions[n]);
        break;
      }
      default: cout<<"invalid input \""<<c<<"\""<<endl;
    }
    cout<<"Please select an action to use on this And Condition:"<<endl<<"p) Print"<<endl<<"r) Resize action list"<<endl<<"e) Edit an Condition"<<endl<<"d) Done with this item"<<endl<<": ";
    cin>>c;
    if(cin.eof())
      break;
  }
  return changed;
}

bool edit(ConditionNot& e){
  cout<<"editing the condition this is the inverse of"<<endl;
  return edit(e.condition);
}

bool edit(ConditionAfter& e){
  char c='p';
  bool changed=false;
  while(c!='d'){
    switch(c){
      case 'p':
        cout<<e<<endl;
        break;
      case 'e':
        cout<<"which event should this be after: ";
        cin>>e.event;
        while(e.event<0||cin.fail()){
          cout<<"invalid input: ";
          cin.clear();
          cin.ignore(100,'\n');
          cin>>e.event;
        }
        changed=true;
        break;
      case 't':
        cout<<"what minimum delay do you want?"<<endl<<": ";
        cin>>e.delay;
        while(e.delay<0||cin.fail()){
          cout<<"invalid input: ";
          cin.clear();
          cin.ignore(100,'\n');
          cin>>e.delay;
        }
        changed=true;
        break;
      default: cout<<"invalid input \""<<c<<"\""<<endl;
    }
    cout<<"Please select an action to use on this after condition:"<<endl<<"p) Print"<<endl<<"e) Edit the event to be after"<<endl<<"t) Edit the required delay"<<endl<<"d) Done with this item"<<endl<<": ";
    cin>>c;
    if(cin.eof())
      break;
  }
  return changed;
}

bool edit(ConditionBefore& e){
  char c='p';
  bool changed=false;
  while(c!='d'){
    switch(c){
      case 'p':
        cout<<e<<endl;
        break;
      case 'e':
        cout<<"which event should this be before?: ";
        cin>>e.event;
        while(e.event<0||cin.fail()){
          cout<<"invalid input: ";
          cin.clear();
          cin.ignore(100,'\n');
          cin>>e.event;
        }
        changed=true;
        break;
      default: cout<<"invalid input \""<<c<<"\""<<endl;
    }
    cout<<"Please select an action to use on this before condition:"<<endl<<"p) Print"<<endl<<"e) Edit the event to be after"<<endl<<"d) Done with this item"<<endl<<": ";
    cin>>c;
    if(cin.eof())
      break;
  }
  return changed;
}

bool edit(ConditionStringPattern& a){
  char c='p';
  bool changed=false;
  while(c!='d'){
    switch(c){
      case 'p':
        cout<<a<<endl;
        break;
      case 'r':{
        cout<<"currently there are "<<a.sm.count<<" elements in the pattern"<<endl<<"how many would you like there to be? ";
        int newlen=-1;
        c=' ';
        cin>>newlen;
        while(newlen<0||!cin.good()){
          cout<<"must be positive integer"<<endl<<": ";
          cin.clear();
          cin.ignore(100,'\n');
          cin>>newlen;
        }
        while(c&&c!='y'&&c!='n'){
          cout<<"are you sure you want to change the length to "<<newlen<<"? y/n: ";
          cin>>c;
        }
        if(c=='n')
          break;
        if(newlen==a.sm.count)
          break;
        SPA<Pair<PatternTag,StringElementCondition> > newpattern(newlen);
        for(int i=0;i<newlen&&i<a.sm.count;++i)
          newpattern[i]=a.sm.pattern[i];
        a.sm.count=newlen;
        a.sm.pattern=newpattern;
        changed=true;
        break;
      }
      case 'e':{
        if(a.sm.count<=0){
          cout<<"none to edit"<<endl;
          break;
        }
        cout<<"which pattern element do you wish to edit? ";
        int n;
        cin>>n;
        while(n>=a.sm.count||n<0){
          cout<<"no such element: ";
          cin>>n;
        }
        changed|=edit(a.sm.pattern[n].b);
        break;
      }
      case 't':{
        if(a.sm.count<=0){
          cout<<"none to edit"<<endl;
          break;
        }
        cout<<"which pattern element's taf do you wish to edit? ";
        int n;
        cin>>n;
        while(n>=a.sm.count||n<0){
          cout<<"no such element: ";
          cin>>n;
        }
        cout<<"editing tag for pattern element :"<<a.sm.pattern[n].b<<endl;
        changed|=edit(a.sm.pattern[n].a);
        break;
      }
      default: cout<<"invalid input \""<<c<<"\""<<endl;
    }
    cout<<"Please select an action to use on this String pattern Condition:"<<endl<<"p) Print"<<endl<<"r) Resize pattern list"<<endl<<"e) Edit a pattern element"<<endl<<"t) Edit the tag for a pattern element"<<endl<<"d) Done with this item"<<endl<<": ";
    cin>>c;
    if(cin.eof())
      break;
  }
  return changed;
}

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
        while(newlen<0||!cin.good()){
          cout<<"must be positive integer"<<endl<<": ";
          cin.clear();
          cin.ignore(100,'\n');
          cin>>newlen;
        }
        while(c&&c!='y'&&c!='n'){
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
        if(a.num<=0){
          cout<<"none to edit"<<endl;
          break;
        }
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
      default: cout<<"invalid input \""<<c<<"\""<<endl;
    }
    cout<<"Please select an action to use on this Multi Action:"<<endl<<"p) Print"<<endl<<"r) Resize action list"<<endl<<"e) Edit an Action"<<endl<<"d) Done with this item"<<endl<<": ";
    cin>>c;
    if(cin.eof())
      break;
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
      case 'r':{
        cout<<"currently there are "<<e.m.count<<" paragraphs's"<<endl<<"how many would you like there to be? ";
        int newlen=-1;
        c=' ';
        cin>>newlen;
        while(newlen<0||!cin.good()){
          cout<<"must be positive integer"<<endl<<": ";
          cin.clear();
          cin.ignore(100,'\n');
          cin>>newlen;
        }
        while(c&&c!='y'&&c!='n'){
          cout<<"are you sure you want to change the length to "<<newlen<<"? y/n: ";
          cin>>c;
        }
        if(c=='n')
          break;
        if(newlen==e.m.count)
          break;
        SPA<Pair<SPA<const char> > > newpars(newlen);
        for(int i=0;i<newlen&&i<e.m.count;++i)
          newpars[i]=e.m.paragraphs[i];
        if(newlen>e.m.count)
          for(int i=e.m.count;i<newlen;++i)
            newpars[i]=Pair<SPA<const char> >(SPA<const char>(1),SPA<const char>(1));
        e.m.count=newlen;
        e.m.paragraphs=newpars;
        changed=true;
        break;
      }
      case 't':{
        if(e.m.count<=0){
          cout<<"none to edit"<<endl;
          break;
        }
        cout<<"which paragraph to change the tag for: ";
        int i;
        cin>>i;
        while(i<0 || i>=e.m.count || cin.fail()){
          cout<<"invalid index"<<endl<<": ";
          cin.clear();
          cin.ignore(100,'\n');
          cin>>i;
        }
        cout<<"please enter the new tag for the paragraph \""<<e.m.paragraphs[i].b<<"\": ";
        string s;
	cin.ignore(100,'\n');
        getline(cin,s);
        cout<<"\""<<s<<"\""<<endl;
        if(s.length()<=0)
          break;
        SPA<char> tmp(s.length()+1);
        memcopy(tmp,s.c_str(),s.length());
        tmp[s.length()]='\0';
        e.m.paragraphs[i].a=tmp;
        changed=true;
        break;
      }
      case 'e':{
        if(e.m.count<=0){
          cout<<"none to edit"<<endl;
          break;
        }
        cout<<"which paragraph to change the text for: ";
        int i;
        cin>>i;
        while(i<0 || i>=e.m.count || cin.fail()){
          cout<<"invalid index"<<endl<<": ";
          cin.clear();
          cin.ignore(100,'\n');
          cin>>i;
        }
        cout<<"please enter new text for the paragraph: ";
        string s;
	cin.ignore(100,'\n');
        getline(cin,s);
        if(s.length()<=0)
          break;
        SPA<char> tmp(s.length()+1);
        memcopy(tmp,s.c_str(),s.length());
        tmp[s.length()]='\0';
        e.m.paragraphs[i].b=tmp;
        changed=true;
        break;
      }
      default: cout<<"invalid input \""<<c<<"\""<<endl;
    }
    cout<<"Please select an action:"<<endl<<"p) Print"<<endl<<"r) Resize the message"<<endl<<"t) Edit a paragraph tag"<<endl<<"e) Edit a paragraph text"<<endl<<"d) Done with this item"<<endl<<": ";
    cin>>c;
    if(cin.eof())
      break;
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
      case 'r':{
        cout<<"currently there are "<<e.m.count<<" paragraphs's"<<endl<<"how many would you like there to be? ";
        int newlen=-1;
        c=' ';
        cin>>newlen;
        while(newlen<0||!cin.good()){
          cout<<"must be positive integer"<<endl<<": ";
          cin.clear();
          cin.ignore(100,'\n');
          cin>>newlen;
        }
        while(c&&c!='y'&&c!='n'){
          cout<<"are you sure you want to change the length to "<<newlen<<"? y/n: ";
          cin>>c;
        }
        if(c=='n')
          break;
        if(newlen==e.m.count)
          break;
        SPA<Pair<SPA<const char> > > newpars(newlen);
        for(int i=0;i<newlen&&i<e.m.count;++i)
          newpars[i]=e.m.paragraphs[i];
        if(newlen>e.m.count)
          for(int i=e.m.count;i<newlen;++i)
            newpars[i]=Pair<SPA<const char> >(SPA<const char>(1),SPA<const char>(1));
        e.m.count=newlen;
        e.m.paragraphs=newpars;
        changed=true;
        break;
      }
      case 't':{
        if(e.m.count<=0){
          cout<<"none to edit"<<endl;
          break;
        }
        cout<<"which paragraph to change the tag for: ";
        int i;
        cin>>i;
        while(i<0 || i>=e.m.count || cin.fail()){
          cout<<"invalid index"<<endl<<": ";
          cin.clear();
          cin.ignore(100,'\n');
          cin>>i;
        }
        cout<<"please enter the new tag for the paragraph \""<<e.m.paragraphs[i].b<<"\": ";
        string s;
	cin.ignore(100,'\n');
        getline(cin,s);
        cout<<"\""<<s<<"\""<<endl;
        if(s.length()<=0)
          break;
        SPA<char> tmp(s.length()+1);
        memcopy(tmp,s.c_str(),s.length());
        tmp[s.length()]='\0';
        e.m.paragraphs[i].a=tmp;
        changed=true;
        break;
      }
      case 'e':{
        if(e.m.count<=0){
          cout<<"none to edit"<<endl;
          break;
        }
        cout<<"which paragraph to change the text for: ";
        int i;
        cin>>i;
        while(i<0 || i>=e.m.count || cin.fail()){
          cout<<"invalid index"<<endl<<": ";
          cin.clear();
          cin.ignore(100,'\n');
          cin>>i;
        }
        cout<<"please enter new text for the paragraph: ";
        string s;
	cin.ignore(100,'\n');
        getline(cin,s);
        if(s.length()<=0)
          break;
        SPA<char> tmp(s.length()+1);
        memcopy(tmp,s.c_str(),s.length());
        tmp[s.length()]='\0';
        e.m.paragraphs[i].b=tmp;
        changed=true;
        break;
      }
      default: cout<<"invalid input \""<<c<<"\""<<endl;
    }
    cout<<"Please select an action:"<<endl<<"p) Print"<<endl<<"r) Resize the message"<<endl<<"t) Edit a paragraph tag"<<endl<<"e) Edit a paragraph text"<<endl<<"d) Done with this item"<<endl<<": ";
    cin>>c;
    if(cin.eof())
      break;
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
      case 'u':{
        cout<<"please enter the new url: ";
        string s;
	cin.ignore(100,'\n');
        getline(cin,s);
        if(s.length()<=0)
          break;
        SPA<char> tmp(s.length()+1);
        memcopy(tmp,s.c_str(),s.length());
        tmp[s.length()]='\0';
        e.nextLevel.a=tmp;
        changed=true;
        break;
      }
      case 'n':{
        cout<<"please enter the new name: ";
        string s;
	cin.ignore(100,'\n');
        getline(cin,s);
        if(s.length()<=0)
          break;
        SPA<char> tmp(s.length()+1);
        memcopy(tmp,s.c_str(),s.length());
        tmp[s.length()]='\0';
        e.nextLevel.b=tmp;
        changed=true;
        break;
      }
      default: cout<<"invalid input \""<<c<<"\""<<endl;
    }
    cout<<"Please select an action:"<<endl<<"p) Print"<<endl<<"u) Change the path/url"<<endl<<"n) Change the name to show"<<endl<<"d) Done with this item"<<endl<<": ";
    cin>>c;
    if(cin.eof())
      break;
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
      case 'c':
        changed|=edit(e.change);
        break;
      case 't':
        changed|=edit(e.select);
        break;
      default: cout<<"invalid input \""<<c<<"\""<<endl;
    }
    cout<<"Please select an action:"<<endl<<"p) Print"<<endl<<"c) Edit the change condition"<<endl<<"t) Edit the target selection status condition"<<endl<<"d) Done with this item"<<endl<<": ";
    cin>>c;
    if(cin.eof())
      break;
  }
  return changed;
}


bool edit(ActionSetStringRoute& a){
  cout<<"Not Implemented Yet"<<endl;
  return false;
  ///TODO IMPLEMENT
  char c='p';
  bool changed=false;
  while(c!='d'){
    switch(c){
      case 'p':
        cout<<a<<endl;
        break;
      default: cout<<"invalid input \""<<c<<"\""<<endl;
    }
    cout<<"Please select an action:"<<endl<<"p) Print"<<endl<<"r) Resize"<<endl<<"e) Edit an event"<<endl<<"d) Done with this item"<<endl<<": ";
    cin>>c;
    if(cin.eof())
      break;
  }
  return changed;
}
#define PRINTIDNAME(T) cout<<T::id<<") "<<#T<<endl;
#define MAKEREPCASE(T) case T::id:\
                         p=SP<T>(new T());\
                         break;
#define MAKEEDITCASE(T) case T::id:\
                          changed|=edit((T&)*p);\
                          break;
bool edit(SP<Condition>& p){
  char c='p';
  bool changed=false;
  while(c!='d'){
    switch(c){
      case 'p':
        cout<<p<<endl;
        break;
      case 'r':{
        cout<<"choose a new condition"<<endl;
        PRINTIDNAME(ConditionTrue)
        PRINTIDNAME(ConditionOr)
        PRINTIDNAME(ConditionAnd)
        PRINTIDNAME(ConditionNot)
        PRINTIDNAME(ConditionAfter)
        PRINTIDNAME(ConditionBefore)
        PRINTIDNAME(ConditionStringPattern)
        int i;
        bool done=false;
        while(!done){
          cout<<": ";
          cin>>i;
          while(cin.fail()){
            cout<<"enter a valid number"<<endl<<": ";
            cin.clear();
            cin.ignore(100,'\n');
            cin>>i;
          }
          done=true;
          switch(i){
            MAKEREPCASE(ConditionTrue)
            MAKEREPCASE(ConditionOr)
            MAKEREPCASE(ConditionAnd)
            MAKEREPCASE(ConditionNot)
            MAKEREPCASE(ConditionAfter)
            MAKEREPCASE(ConditionBefore)
            MAKEREPCASE(ConditionStringPattern)
            default:
              done=false;
              cout<<"Unknown Condition"<<endl;
          }
        }
        changed=true;
        break;
      }
      case 'e':
        switch(p->getid()){
          MAKEEDITCASE(ConditionTrue)
          MAKEEDITCASE(ConditionOr)
          MAKEEDITCASE(ConditionAnd)
          MAKEEDITCASE(ConditionNot)
          MAKEEDITCASE(ConditionAfter)
          MAKEEDITCASE(ConditionBefore)
          MAKEEDITCASE(ConditionStringPattern)
          default:
            cout<<"unknown condition can't edit"<<endl;
        }
        break;
      default: cout<<"invalid input \""<<c<<"\""<<endl;
    }
    cout<<"Please select an action to use on this Condition:"<<endl<<"p) Print"<<endl<<"r) Replace"<<endl<<"e) Edit"<<endl<<"d) Done with this item"<<endl<<": ";
    cin>>c;
    if(cin.eof())
      break;
  }
  return changed;
}
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
          while(cin.fail()){
            cout<<"enter a valid number"<<endl<<": ";
            cin.clear();
            cin.ignore(100,'\n');
            cin>>i;
          }
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
      default: cout<<"invalid input \""<<c<<"\""<<endl;
    }
    cout<<"Please select an action to use on this Action:"<<endl<<"p) Print"<<endl<<"r) Replace"<<endl<<"e) Edit"<<endl<<"d) Done with this item"<<endl<<": ";
    cin>>c;
    if(cin.eof())
      break;
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
      default: cout<<"invalid input \""<<c<<"\""<<endl;
    }
    cout<<"Please select an action to use on this Event:"<<endl<<"p) Print"<<endl<<"t) Change trigger"<<endl<<"c) Edit the condition"<<endl<<"a) Edit the action"<<endl<<"d) Done with this item"<<endl<<": ";
    cin>>c;
    if(cin.eof())
      break;
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
        while(newlen<0||!cin.good()){
          cout<<"must be positive integer"<<endl<<": ";
          cin.clear();
          cin.ignore(100,'\n');
          cin>>newlen;
        }
        while(c&&c!='y'&&c!='n'){
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
        if(s.geteventcount()<=0){
          cout<<"none to edit"<<endl;
          break;
        }
        cout<<"which event do you wish to edit? ";
        int n;
        cin>>n;
        while(n<0 || n>=s.geteventcount() || cin.fail()){
          cout<<"invalid index"<<endl<<": ";
          cin.clear();
          cin.ignore(100,'\n');
          cin>>n;
        }
        changed|=edit(s.getevents()[n]);
        break;
      }
      default: cout<<"invalid input \""<<c<<"\""<<endl;
    }
    cout<<"Please select an action to use on this Script:"<<endl<<"p) Print"<<endl<<"r) Resize"<<endl<<"e) Edit an event"<<endl<<"d) Done with this item"<<endl<<": ";
    cin>>c;
    if(cin.eof())
      break;
  }
  return changed;
}

void edit(){
  char c='p';
  Maze m(Vector(0,0,0));
  Script s;
  bool changed=false;
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
          while(c&&c!='y' && c!='n'){
            cout<<"invalid response. ";
            cin>>c;
          }
          if(c=='n')
            break;
        }
        cout<<"please enter the filename: ";
        cin.ignore(100,'\n');
        cin.getline(fname,256);
        cout<<"opening "<<fname<<endl;
        ifstream is(fname);
        if(!is.is_open()){
          cout<<"error opening file"<<endl;
          break;
        }
        CPPHypIStream ihs(is);
        read(ihs,m);
        read(ihs,s);
        changed=false;
        cout<<"new script loaded: "<<s<<endl;
        break;
      }
      case 'n':
        if(changed){
          cout<<"The current script has unsaved changes. Are you sure you want to create a new one? (y/n) ";
          cin>>c;
          while(c&&c!='y' && c!='n'){
            cout<<"invalid response. ";
            cin>>c;
          }
          if(c=='n')
            break;
        }
        m=Maze(Vector(0,0,0));
        s=Script();
        break;
      case 's':{
        char* tmp=new char[256];
        cout<<"Current filename is "<<fname<<"please enter the filename or leave blank to use same name: ";
	cin.ignore(100,'\n');
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
        write(ohs,m);
        ohs.setNextSpace("\n");
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
          cout<<"The current script has unsaved changes. Are you sure you want to exit? (y/n) ";
          cin>>c;
          while(c&&c!='y' && c!='n'){
            cout<<"invalid response. ";
            cin>>c;
          }
          if(c=='n')
            break;
        }
        return;
      default: cout<<"invalid input \""<<c<<"\""<<endl;
    }
    cout<<"What would you like to do?"<<endl<<"p) Print the current Script"<<endl<<"l) Load a script from file"<<endl<<"n) Create a new empty script"<<endl<<"s) Save the current script"<<endl<<"e) Edit the current script"<<endl<<"d) Done with editing"<<endl<<": ";
    cin>>c;
    if(cin.eof())
      break;
  }
}

int main(){
  edit();
}

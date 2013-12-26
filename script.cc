#include <cctype>
#include "script.hh"
#include "scriptimpl.hh"

inline bool StringElementCondition::matches(T el){
  if((selectionCondition&2)==0&&(((selectionCondition&1)==1)==el->selected))
    return false;
  if(dirnsCondition&to_mask(el->d)==0)
    return false;
  bool match=xrange_count==0;
  for(int i=0;i<xrange_count&&!match;++i)
    if(xrange[i].inRange(el->pos.X))
      match=true;
  if(!match)
    return false;
  match=yrange_count==0;
  for(int i=0;i<yrange_count&&!match;++i)
    if(yrange[i].inRange(el->pos.Y))
      match=true;
  if(!match)
    return false;
  match=zrange_count==0;
  for(int i=0;i<zrange_count&&!match;++i)
    if(zrange[i].inRange(el->pos.Z))
      match=true;
  return match;
}

IOResult read(HypIStream& s,StringElementCondition& c){
  IOResult r;
  if(!(r=read(s,c.selectionCondition,0)).ok)
    return r;
  if(!(r=read(s,c.dirnsCondition,0)).ok)
    return r;
  if(!(r=read(s,c.xrange_count,0)).ok){
    c.xrange_count=0;
    return r;
  }else{
    c.xrange=SPA[new Range[c.xrange_count]];
    for(int i=0;i<c.xrange_count){
      if(!(r=read(s,c.xrange[i].start,0)).ok)
        return r;
      if(!(r=read(s,c.xrange[i].end,0)).ok)
        return r;
    }
  }
  if(!(r=read(s,c.yrange_count,0)).ok){
    c.yrange_count=0;
    return r;
  }else{
    c.yrange=SPA[new Range[c.yrange_count]];
    for(int i=0;i<c.yrange_count){
      if(!(r=read(s,c.yrange[i].start,0)).ok)
        return r;
      if(!(r=read(s,c.yrange[i].end,0)).ok)
        return r;
    }
  }
  if(!(r=read(s,c.zrange_count,0)).ok){
    c.zrange_count=0;
    return r;
  }else{
    c.zrange=SPA[new Range[c.zrange_count]];
    for(int i=0;i<c.zrange_count){
      if(!(r=read(s,c.zrange[i].start,0)).ok)
        return r;
      if(!(r=read(s,c.zrange[i].end,0)).ok)
        return r;
    }
  }
  return IOResult(true,r.eof);
}
bool write(HypOStream& s,const StringElementCondition& c){
  if(!write(s,c.selectionCondition,0))
    return false;
  if(!write(s,c.dirnsCondition,0))
    return false;
  if(!write(s,c.xrange_count,0))
    return false;
  for(int i=0;i<c.xrange_count){
    if(!write(s,c.xrange[i].start,0))
      return false;
    if(!write(s,c.xrange[i].end,0))
      return false;
  }
  if(!write(s,c.yrange_count,0))
    return false;
  for(int i=0;i<c.yrange_count){
    if(!write(s,c.yrange[i].start,0))
      return false;
    if(!write(s,c.yrange[i].end,0))
      return false;
  }
  if(!write(s,c.zrange_count,0))
    return false;
  for(int i=0;i<c.zrange_count){
    if(!write(s,c.zrange[i].start,0))
      return false;
    if(!write(s,c.zrange[i].end,0))
      return false;
  }
  return true;
}

IOResult read(HypIStream& s,PatternTag& pt){
  IOResult r;
  (r=read(s,pt.min,0)).ok && (r=read(s,pt.max,0)).ok && (r=read(s,pt.greedy)).ok;
  return r;
}
bool write(HypOStream& s,const PatternTag& pt){
  return write(s,pt.min,0) && write(s,pt.max,0) && write(s,pt.greedy);
}

IOResult read(HypIStream& s,StringMatcher& sm){
  IOResult r;
  if(!(r=read(s,sm.count,0)).ok){
    sm.count=0;
    return r;
  }
  sm.pattern=SPA<Pair<PatternTag,StringElementCondition> >(new Pair<PatternTag,StringElementCondition>[sm.count]);
  for(int i=0;i<sm.count;++i){
    if(!(r=read(s,sm.pattern[i].a,0)).ok)
      return r;
    if(!(r=read(s,sm.pattern[i].b,0)).ok)
      return r;
  }
  if(!(r=read(s,sm.group_count,0)).ok){
    sm.group_count=0;
    return r;
  }
  sm.groups=SPA<Pair<int> >(new Pair<int>[sm.group_count]);
  for(int i=0;i<sm.group_count;++i){
    if(!(r=read(s,sm.groups[i].a,0)).ok)
      return r;
    if(!(r=read(s,sm.groups[i].b,0)).ok)
      return r;
  }
  return IOResult(true,r.eof);
}
bool write(HypOStream& s,const StringMatcher& sm){
  if(!write(s,sm.count,0))
    return false;
  for(int i=0;i<sm.count;++i){
    if(!(write(s,sm.pattern[i].a) && write(s,sm.pattern[i].b)))
      return false;
  if(!write(s,sm.group_count,0))
    return false;
  for(int i=0;i<sm.group_count;++i){
    if(!(write(s,sm.groups[i].a,0) && write(s,sm.groups[i].b,0)))
      return false;
  return true;
}

bool StringMatcher::match(const String& s,SPA<Pair<SP<ConstStringPointer> > > groups){
  return match<const String,ConstStringPointer>(s,groups,0);
}
bool StringMatcher::match(String& s,SPA<Pair<SP<StringPointer> > > groups){
  return match<String,StringPointer>(s,groups,0);
}
bool StringMatcher::match(const String& s,SPA<StringMatcherCallback<ConstStringPointer>& cb,
    Pair<SP<ConstStringPointer> > > groups){
  if(groups.isnull()){
    groups=SPA<Pair<SP<ConstStringPointer> > >()new Pair<SP<ConstStringPointer> >[group_count]);
	}
  return match<const String,ConstStringPointer>(s,groups,&cb);
}
bool StringMatcher::match(String& s,SPA<StringMatcherCallback<StringPointer>& cb,
    Pair<SP<StringPointer> > > groups){
  if(groups.isnull()){
    groups=SPA<Pair<SP<StringPointer> > >()new Pair<SP<StringPointer> >[group_count]);
	}
  return match<String,StringPointer>(s,groups,&cb);
}

template <class STRING,class POINTER>
bool StringMatcher::match(STRING& s,SPA<Pair<SP<POINTER> > > groups,StringMatcherCallback<POINTER>* cb){
  SPA<PatternMatch<POINTER> > m(groups.isnull()?0:new PatternMatch<POINTER>[count]);
  return matchStep(s,s.begin(),m,0,groups,cb);
}

template <class STRING,class POINTER>
bool StringMatcher::matchStep(STRING& s,POINTER p,SPA<PatternMatch<POINTER> > matches,int level,
    SPA<Pair<SP<POINTER> > > groups,StringMatcherCallback<POINTER>* cb){
  if(level==count){
    cout<<level<<" at end of pattern so are we at the end of the string? "<<(p==s.end())<<endl;
    if(p==s.end()){
      //valid so store in group if it's defined
			if(!groups.isnull()){
				for(int i=0;i<group_count;++i){
				  groups[i].a=matches[this->groups[i].a].start;
				  groups[i].b=matches[this->groups[i].b].end;
				}
			  if(cb)
			    cb->process(groups);
			}
      return true;
    }else{
      return false;
    }
  }else{
    bool match=false;
    PatternTag& pt=pattern[level].a;
    StringElementCondition& sec=pattern[level].b;
    if(!matches.isnull())
      matches[level].start=SP<POINTER>(new POINTER(p));
    cout<<level<<" "<<"starting match step from "<<p<<endl;
    int i=0;
    while(i<pt.min){
      cout<<level<<" "<<"stepping to min "<<i<<" "<<p<<" "<<sec.matches(p)<<endl;
      if(p==s.end()||!sec.matches(p))
        return false;
      ++i;++p;
    }
    if(pt.greedy){
      //go to first that doesn't match or length is max+1 (i is max)  (whichevers first)
      while(i<pt.max && p!=s.end() && sec.matches(p)){
        ++i;++p;
        cout<<level<<" "<<"stepping to max "<<i<<" "<<p<<" "<<sec.matches(p)<<endl;
      }
      cout<<level<<" "<<"at max (or first fail) "<<i<<" "<<p<<" "<<endl;
    }
    //while i is still in valid range
    while(i>=pt.min && i<=pt.max){
	    if(!matches.isnull())
			  matches[level].end=SP<POINTER>(new POINTER(p));
      //see if we can match the next group
      match|=matchStep(s,p,matches,level+1,groups,cb);
      
      //if we have a match and we don't have a call back then return match (true)
      if(match && cb==0)
        return true;
      
      if(pt.greedy){
        //stepping back. don't need to check if valid as was checked while searching forward]
        cout<<level<<" stepping back "<<i<<" "<<p<<endl;
        --i;--p;
      }else{
	      cout<<level<<" "<<"stepping up "<<i<<" "<<p<<" "<<sec.matches(p)<<endl;
        if(p==s.end() || !sec.matches(p))
          return match;
        ++i;++p;
      }
    }
    cout<<level<<" tried all options for this pattern"<<endl;
    return match;
  }
}

template <class T>
IOResult read(HypIStream& s,SPA<SP<T> >& a, int& c){
  IOResult r=read(s,c,0);
  if(!r.ok){
    c=0;
    return r;
  }
  a=SPA<SP<T> >(new SP<T>[c]);
  int i=0;
  for(;i<c;++i){
    if(!(r=read(s,a[i])).ok)
      break;
  }
  if(i<c){
    for(;i<c;++i)
      a[i]=T::defaultvalue;
    return IOResult(false,r.eof);
  }else{
    return IOResult(true,r.eof);
  }
}
template <class T>
bool write(HypOStream& s,const SPA<const SP<const T> >& a, const int& c){
  if(!write(s,c,0))
    return false;
  for(int i=0;i<c;++i)
    if(!write(s,a[i]))
      return false;
  return true;
}

const SP<Condition> Condition::defaultvalue=SP<Condition>(new ConditionTrue());

template <class T>
IOResult createAndRead(HypIStream& s,SP<T>& c){
  c=SP<T>(new T());
  return read(s,*c);
}
IOResult read(HypIStream& s,SP<Condition>& c){
  int id=0;
  IOResult r=read(s,id,0);
  if(!r.ok){
    c=Condition::defaultvalue;
    return r;
  }
  switch(id){
    case 1:
      return createAndRead<ConditionTrue>(s,c);
    case 2:
      return createAndRead<ConditionOr>(s,c);
    case 3:
      return createAndRead<ConditionAnd>(s,c);
    case 4:
      return createAndRead<ConditionNot>(s,c);
    case 5:
      return createAndRead<ConditionAfter>(s,c);
    case 6:
      return createAndRead<ConditionBefore>(s,c);
    case 7:
      return createAndRead<ConditionStringPattern>(s,c);
    default:
      c=Condition::defaultvalue;
  }
}
bool write(HypOStream& s,const SP<const Condition>& c){
  return c->dowrite(s);
}

bool ConditionOr::is(int time,const Script& script,const String& s){
  for(int i=0;i<count;++i)
    if(conditions[i]->is(time,script,s))
      return true;
  return false;
}
IOResult read(HypIStream& s,ConditionOr& c){
  return read(s,c.conditions,c.count);
}
bool write(HypOStream& s,const ConditionOr& c){
  return write(s,c.conditions,c.count);
}

bool ConditionAnd::is(int time,const Script& script,const String& s){
  for(int i=0;i<count;++i)
    if(!conditions[i]->is(time,script,s))
      return false;
  return true;
}
IOResult read(HypIStream& s,ConditionAnd& c){
  return read(s,c.conditions,c.count);
}
bool write(HypOStream& s,const ConditionAnd& c){
  return write(s,c.conditions,c.count);
}

IOResult read(HypIStream& s,ConditionNot& c){
  return read(s,c.condition);
}
bool write(HypOStream& s,const ConditionNot& c){
  return write(s,c.condition);
}

bool ConditionAfter::is(int time,const Script& script,const String& s){
  return script.getTime(event)+delay<=time;
}
IOResult read(HypIStream& s,ConditionAfter& c){
  IOResult r=read(s,c.event,0);
  if(!r.ok)
    return r;
  return read(s,c.delay,0);
}
bool write(HypOStream& s,const ConditionAfter& c){
  return write(s,c.event,0) && write(s,c.delay,0);
}

bool ConditionBefore::is(int time,const Script& script,const String& s){
  return script.getTime(event)==0;
}
IOResult read(HypIStream& s,ConditionBefore& c){
  return read(s,c.event,0);
}
bool write(HypOStream& s,const ConditionBefore& c){
  return write(s,c.event,0);
}

IOResult read(HypIStream& s,Message& m){
  IOResult r=read(s,m.count,0);
  if(!r.ok){
    m.count=0;
    return r;
  }
  m.paragraphs=SPA<Pair<SPA<char> > >(new Pair<SPA<char> >[m.count]);
  int i=0;
  for(;i<m.count;++i)
    if(!((r=read(s,m.paragraphs[i].a,false)).ok && (r=read(s,m.paragraphs[i].b,true)).ok))
      break;
  if(i<c){
    SP<char> defaultvalue(new char[1]);
    defaultvalue[0]='\0';
    for(;i<c;++i){
      m.paragraphs[i].a=defaultvalue;
      m.paragraphs[i].b=defaultvalue;
    }
  }
  return r;
}

bool write(HypOStream& s,const Message& m){
  if(!write(s,m.count,0))
    return false;
  for(int i=0;i<m.count;++i){
    if(!(write(s,m.paragraphs[i].a,false) && write(s,m.paragraphs[i].b,true)))
      return false;
  }
  return true;
}

const SP<Action> Action::defaultvalue=SP<Action>(new ActionNothing());

IOResult read(HypIStream& s,SP<Action>& a){
  int id=0;
  IOResult r=read(s,id,0);
  if(!r.ok){
    a=Action::defaultvalue;
    return r;
  }
  switch(id){
    case 1:
      return createAndRead<ActionNothing>(s,a);
    case 2:
      return createAndRead<ActionMesage>(s,a);
    case 3:
      return createAndRead<ActionBlockWin>(s,a);
    default:
      a=Action::defaultvalue;
  }
}
bool write(HypOStream& s,const SP<const Action>& a){
  return a->dowrite(s);
}

void ActionMessage::doCommon(ScriptResponse& r,String&){
  if(r.messageCount==0){
    r.messages=new Message[1];
  }else{
    Message* tmp=new Message[r.messageCount+1];
    memcpy(tmp,r.messages,r.messageCount*sizeof(Message));
    r.messages=tmp;
  }
  r.messages[r.messageCount]=m;
  ++r.messageCount;
};

IOResult read(HypIStream& s,ActionMessage& a){
  return read(s,a.m);
}
bool write(HypOStream& s,const ActionMessage& a){
  return write(s,a.m);
}

IOResult read(HypIStream& s,ActionWinMessage& a){
  return read(s,a.m);
}
bool write(HypOStream& s,const ActionWinMessage& a){
  return write(s,a.m);
}

class dataDescParser:public InputParser{
  Pair<irr::stringc>* d;
  int pos;
  irr::stringc lines;
  char delim;
  public:
    dataDescParser  (Pair<irr::stringc>* d):d(d),pos(0),lines(),delim(0){};
    Used parse(char* data,irr::u32 length,bool eof){
      char* start=data;
      char* end=data+length;
      if(!eof)
        end-=1;
      char* tmp;
      if(pos<2){
        if(pos==0){
          while(isspace(*data))
            if(++data>=end)
              return Used(data-start,false);
          tmp=data;
          while(!isspace(*tmp))
            if(++tmp>=end)
              return Used(data-start,false);
          d->a=irr::stringc(data,tmp-data);
          data=tmp;
          pos=1;
        }
        if(!delim){
          while(isspace(*data))
            if(++data>=end)
              return Used(data-start,false);
          delim=*data;
          if(++data>=end)
            return Used(data-start,false);
        }
        tmp=(char*)memchr(data,delim,end-data);
        if(!tmp){
          lines.append(data,end-data);
          return Used(end-start,false);
        }
        lines+=irr::stringc(data,tmp-data);
        d->b=lines;
        lines=irr::stringc();
        delim=0;
        pos=2;
        data=tmp+1;
      }
      return Used(data-start,true);
    }
};

InputParser* ActionWinNextLevel::createParser(){
  return new dataDescParser(&nextLevel);
}
void ActionWinNextLevel::output(irr::stringc* s,irr::IWriteFile* file){
  *s+="4 ";
  const char* const delims="\"'|^_!\\\3";
  if(nextLevel.a=="")
    nextLevel.a="-";
  *s+=nextLevel.a;
  *s+=" ";
  const char* delim=delims;
  while(nextLevel.b.findFirst(*delim)!=-1)
      ++delim; // if this fails then you have a string with '\3' in it!!!!!!!!
  *s+=*delim;
  *s+=nextLevel.b;
  *s+=*delim;
  *s+="\n";
  if(file && s->size()>256){
    file->write(s->c_str(),s->size());
    *s=irr::stringc();
  }
}

void ActionSelectStringPattern::doCommon(ScriptResponse& r,String& s){
  StringEdit se(s);
  for(StringPointer p=s.begin();p!=s.end();++p){
    if(!change.matches(p))
      continue;
    bool newsel=select.matches(p);
    if(newsel^p->selected){
      r.stringSelectionChanged=true;
      se.setSelected(p,newsel);
    }
  }
}

InputParser* ActionSelectStringPattern::createParser(){
  InputParser** parsers=new InputParser*[2];
  parsers[0]=new StringConditionParser(&this->change);
  parsers[1]=new StringConditionParser(&this->select);
  return new SequentialInputParser<Derefer<InputParser,InputParser**> >(
    Derefer<InputParser,InputParser**>(parsers),
    Derefer<InputParser,InputParser**>(parsers+2));
};
void ActionSelectStringPattern::returnParser(InputParser* parser){
  SequentialInputParser<Derefer<InputParser,InputParser**> >* p=(SequentialInputParser<Derefer<InputParser,InputParser**> >*)parser;
  delete *(p->end.data-2);
  delete *(p->end.data-1);
  delete[] (p->end.data-2);
  delete parser;
};

bool ActionSetStringRoute::process(SPA<Pair<SP<StringPointer> > > groups){
  for(int i=0;i<ranges.groupCount();++i){
    se->setStringSegment(*groups[i].a,*groups[i].b,count,route);
  }
}

void ActionSetStringRoute::doCommon(ScriptResponse& r,String& s){
  if(!ranges.groupCount())
    return;
  se=new StringEdit(s);
  SPA<Pair<SP<StringPointer> > > groups=SPA<Pair<SP<StringPointer> > >(
      new Pair<SP<StringPointer> >[ranges.groupCount()]);
  if(ranges.match(s,all?this:0,groups)){
    if(!all)
      process(groups);
	  r.stringChanged=true;
	}
	delete se;
}
InputParser* ActionSetStringRoute::createParser(){
  InputParser** parsers=new InputParser*[2];
  parsers[0]=ranges.createParser();
  parsers[1]=this;
  return new SequentialInputParser<Derefer<InputParser,InputParser**> >(
    Derefer<InputParser,InputParser**>(parsers),
    Derefer<InputParser,InputParser**>(parsers+2));
}
void ActionSetStringRoute::returnParser(InputParser* parser){
  SequentialInputParser<Derefer<InputParser,InputParser**> >* p=(SequentialInputParser<Derefer<InputParser,InputParser**> >*)parser;
  ranges.returnParser(*(p->end.data-2));
  delete[] (p->end.data-2);
  delete parser;
}
Used ActionSetStringRoute::parse(char* data,irr::u32 length,bool eof){
  char* start=data;
  char* end=data+length;
  if(!eof)
    end-=1;
  all=strtol(data,&data,10);
  if(data>=end&&!eof) return Used(0,false);
  count=strtol(data,&data,10);
  if(data>=end&&!eof) return Used(0,false);
  delete[] route;
  route=new Dirn[count];
  for(int i=0;i<count;++i){
		route[i]=from_id(strtol(data,&data,10));
		if(data>=end) return Used(0,false);
  }
  return Used(0,false);
}
void ActionSetStringRoute::output(irr::stringc* s,irr::IWriteFile* file){
  (*s)+="7 ";
  ranges.output(s,file);
  (*s)+=all;
  (*s)+=" ";
  (*s)+=count;
  (*s)+=" ";
  for(Dirn *d=route;d!=route+count;++d){
    (*s)+=to_id(*d);
    (*s)+=" ";
  }
  (*s)+="\n";
}



InputParser* Script::createParser(Condition** condition){
  InputParser** parsers=new InputParser*[2];
  parsers[0]=new Parser<Condition>(parsers+1,condition);
  return new SequentialInputParser<Derefer<InputParser,InputParser**> >(
      Derefer<InputParser,InputParser**>(parsers),
      Derefer<InputParser,InputParser**>(parsers+2));
}
InputParser* Script::createParser(Action*** action,int* count){
  InputParser** parsers=new InputParser*[2];
  parsers[0]=new ListParser<Action>(parsers+1,action,count);
  return new SequentialInputParser<Derefer<InputParser,InputParser**> >(
      Derefer<InputParser,InputParser**>(parsers),
      Derefer<InputParser,InputParser**>(parsers+2));
}
void Script::returnParser(InputParser* parser){
  SequentialInputParser<Derefer<InputParser,InputParser**> >* p=(SequentialInputParser<Derefer<InputParser,InputParser**> >*)parser;
  delete *(p->end.data-2);
  delete[] (p->end.data-2);
  delete parser;
};


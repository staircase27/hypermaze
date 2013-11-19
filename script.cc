#include <cctype>
#define USE_CLIM
#ifdef USE_CLIM
#include <climits>
#else
#define INT_MAX std::numeric_limits<int>::max()
#endif
#include "script.hh"
#include "scriptimpl.hh"

//New code
IOResult read(HypIStream& s,SP<Condition>& c){
  return IOResult(false,false);
}
bool write(HypOStream& s,const SP<const Condition>& c);

template <class T>
IOResult read(HypIStream& s,SPA<SP<T>>& c,const int& count){
  
}
template <class T>
bool write(HypOStream& s,const SPA<const SP<const T>>& c);
//End new code


void StringElementCondition::output(irr::stringc* s,irr::IWriteFile* file){
  if((selectionCondition&1)==0)
    (*s)+="*";
  else if((selectionCondition&2)==0)
    (*s)+="N";
  else
    (*s)+="Y";
  (*s)+=" ";
  (*s)+=dirnsCondition;
  (*s)+=" ";
  (*s)+=xrange_count;
  for(int i=0;i<xrange_count;++i){
    (*s)+=" ";
    if(xrange[i].start==INT_MAX)
      (*s)+="*";
    else
      (*s)+=xrange[i].start;
    (*s)+=" ";
    if(xrange[i].end==INT_MAX)
      (*s)+="*";
    else
      (*s)+=xrange[i].end;
  }
  (*s)+=" ";
  if(file && s->size()>256){
    file->write(s->c_str(),s->size());
    *s=irr::stringc();
  }
  (*s)+=yrange_count;
  for(int i=0;i<yrange_count;++i){
    (*s)+=" ";
    if(yrange[i].start==INT_MAX)
      (*s)+="*";
    else
      (*s)+=yrange[i].start;
    (*s)+=" ";
    if(yrange[i].end==INT_MAX)
      (*s)+="*";
    else
      (*s)+=yrange[i].end;
  }
  (*s)+=" ";
  if(file && s->size()>256){
    file->write(s->c_str(),s->size());
    *s=irr::stringc();
  }
  (*s)+=zrange_count;
  for(int i=0;i<zrange_count;++i){
    (*s)+=" ";
    if(zrange[i].start==INT_MAX)
      (*s)+="*";
    else
      (*s)+=zrange[i].start;
    (*s)+=" ";
    if(zrange[i].end==INT_MAX)
      (*s)+="*";
    else
      (*s)+=zrange[i].end;
  }
  (*s)+="\n";
  if(file && s->size()>256){
    file->write(s->c_str(),s->size());
    *s=irr::stringc();
  }
}

Used StringConditionParser::parse(char* data,irr::u32 length,bool eof){
  char* start=data;
  char* end=data+length;
  if(!eof)
    end-=1;
  while(stage!=4){
    if(stage==0){
      while(isspace(*data))
        if(++data>=end)
          return Used(data-start,false);
      if(*data=='*')
        c->selectionCondition=0;
      else if(*data=='Y'||*data=='y')
        c->selectionCondition=3;
      else
        c->selectionCondition=1;
      char* tmp=data+1;
      while(isspace(*tmp))
        if(++tmp>=end)
          return Used(data-start,false);
      c->dirnsCondition=strtol(tmp,&tmp,10);
      if(++tmp>=end)
        return Used(data-start,false);
      data=tmp;
      ++stage;
    }else{
      int* count;
      Range** ranges;
      if((stage&3)==1){
        count=&c->xrange_count;
        ranges=&c->xrange;
      }else if((stage&3)==2){
        count=&c->yrange_count;
        ranges=&c->yrange;
      }else{//(stage&3)==3
        count=&c->zrange_count;
        ranges=&c->zrange;
      }
      if((stage&~3)==0){
        char* tmp=data;
        *count=strtol(data,&tmp,10);
        if(tmp>=end&&!eof)
          return Used(data-start,false);
        (*ranges)=new Range[*count];
        data=tmp;
        stage+=4;
      }
      for(;(stage&~3)<=4*(*count);stage+=4){
        while(isspace(*data))
          if(++data>=end)
            return Used(data-start,false);
        char* tmp=data;
		    if(*data=='*'){
		      (*ranges)[(stage>>2)-1].start=INT_MAX;
		      ++tmp;
		    }else{
		      (*ranges)[(stage>>2)-1].start=strtol(data,&tmp,10);
		      if(tmp>=end&&!eof)
		        return Used(data-start,false);
		    }
        while(isspace(*tmp))
          if(++tmp>=end)
            return Used(data-start,false);
		    if(*tmp=='*'){
		      (*ranges)[(stage>>2)-1].end=INT_MAX;
		      ++tmp;
		    }else{
		      (*ranges)[(stage>>2)-1].end=strtol(tmp,&tmp,10);
		      if(tmp>=end&&!eof)
		        return Used(data-start,false);
		    }
		    data=tmp;
	    }
	    stage=(stage&3)+1;
    }
  }
  return Used(data-start,true);
}

Used PatternTag::parse(char* data,irr::u32 length,bool eof){
  char* start=data;
  char* end=data+length;
  if(!eof)
    end-=1;
  
	min=strtol(data,&data,10);
	if(data>=end&&!eof)
		return Used(0,false);  
	max=strtol(data,&data,10);
	if(data>=end&&!eof)
		return Used(0,false);  
	greedy=(bool)strtol(data,&data,10);
	if(data>=end&&!eof)
		return Used(0,false);  
	return Used(data-start,true);
}

void PatternTag::output(irr::stringc* s,irr::IWriteFile* file){
  (*s)+=min;
  (*s)+=" ";
  (*s)+=max;
  (*s)+=" ";
  (*s)+=greedy;
  (*s)+="\n";
}

void StringMatcher::output(irr::stringc* s,irr::IWriteFile* file){
  (*s)+=count;
  (*s)+="\n";
  for(int i=0;i<count;++i){
	  pattern[i].first.output(s,file);
    pattern[i].second.output(s,file);
  }
  (*s)+=group_count;
  (*s)+=" ";
  for(int i=0;i<group_count;++i){
    (*s)+=groups[i].first;
    (*s)+=" ";
    (*s)+=groups[i].second;
    (*s)+=" ";
  }
};

class StringMatcherGroupsParser:public InputParser{
  StringMatcher* sm;
  int stage;
  public:
		StringMatcherGroupsParser(StringMatcher* sm):sm(sm),stage(-1){};
		Used parse(char* data,irr::u32 length,bool eof){
			char* start=data;
			char* end=data+length;
			if(!eof)
				end-=1;
		  if(stage==-1){
				sm->group_count=strtol(data,&data,10);
				if(data>=end&&!eof)
					return Used(0,false);
			  delete[] sm->groups;
			  sm->groups=new pair<int,int>[sm->group_count];
		    ++stage;
		  }
		  while(stage<sm->group_count){
		    char* tmp;
				sm->groups[stage].first=strtol(data,&tmp,10);
				if(tmp>=end&&!eof)
					return Used(data-start,false);  
				sm->groups[stage].second=(bool)strtol(tmp,&tmp,10);
				if(data>=end&&!eof)
					return Used(data-start,false);  
		    data=tmp;
		    ++stage;
		  }
		  return Used(data-start,true);
		}
};

class StringMatcherParser:public InputParser{
  InputParser** dataParser;
  StringMatcher* sm;
  public:
    virtual Used parse(char* data,irr::u32 length,bool eof);
    StringMatcherParser(InputParser** dataParser,  StringMatcher* sm):
        dataParser(dataParser),sm(sm){};
    virtual ~StringMatcherParser();
};

Used StringMatcherParser::parse(char* data,irr::u32 length,bool eof){
  char* start=data;
  char* end=data+length;
  if(!eof)
    end-=1;
  sm->count=strtol(data,&data,10);
  if(data>=end) return Used(0,false);
  delete[] sm->pattern;
  sm->pattern=new pair<PatternTag,StringElementCondition>[sm->count];
  InputParser** parsers=new InputParser*[2*(sm->count)+1];
  for(int i=0;i<sm->count;++i){
    parsers[i*2]=&sm->pattern[i].first;
    parsers[i*2+1]=new StringConditionParser(&sm->pattern[i].second);
  }
  parsers[2*sm->count]=new StringMatcherGroupsParser(sm);
  *dataParser=new SequentialInputParser<Derefer<InputParser,InputParser**> >(
      Derefer<InputParser,InputParser**>(parsers),
      Derefer<InputParser,InputParser**>(parsers+(2*(sm->count)+1)));
  return Used(data-start,true);
}

StringMatcherParser::~StringMatcherParser(){
  SequentialInputParser<Derefer<InputParser,InputParser**> >* p=(SequentialInputParser<Derefer<InputParser,InputParser**> >*)(*dataParser);
  for(int i=0;i<sm->count;++i)
    delete *(p->end.data-2*(sm->count)+2*i);
  delete *(p->end.data-1);
  delete[] (p->end.data-(2*(sm->count)+1));
  delete p;
}
InputParser* StringMatcher::createParser(){
  InputParser** parsers=new InputParser*[2];
  parsers[0]=new StringMatcherParser(parsers+1,this);
  return new SequentialInputParser<Derefer<InputParser,InputParser**> >(
    Derefer<InputParser,InputParser**>(parsers),
    Derefer<InputParser,InputParser**>(parsers+2));
}
void StringMatcher::returnParser(InputParser* parser){
  SequentialInputParser<Derefer<InputParser,InputParser**> >* p=(SequentialInputParser<Derefer<InputParser,InputParser**> >*)parser;
  delete *(p->end.data-2);
  delete[] (p->end.data-2);
  delete parser;
}

bool StringMatcher::match(const String& s,pair<SP<ConstStringPointer>,SP<ConstStringPointer> >* groups){
  return match<const String,ConstStringPointer>(s,groups,0);
}
bool StringMatcher::match(String& s,pair<SP<StringPointer>,SP<StringPointer> >* groups){
  return match<String,StringPointer>(s,groups,0);
}
bool StringMatcher::match(String& s,StringMatcherCallback<StringPointer>* cb,pair<SP<StringPointer>,SP<StringPointer> >* groups){
  SPA<pair<SP<StringPointer>,SP<StringPointer> > > localgroups;
  if(groups==0){
    groups=new pair<SP<StringPointer>,SP<StringPointer> >[group_count];
	  localgroups=SPA<pair<SP<StringPointer>,SP<StringPointer> > >(groups);
	}
  return match<String,StringPointer>(s,groups,cb);
}

template <class STRING,class POINTER>
bool StringMatcher::match(STRING& s,pair<SP<POINTER>,SP<POINTER> >* groups,StringMatcherCallback<POINTER>* cb){
  PatternMatch<POINTER>* m=groups==0?0:new PatternMatch<POINTER>[count];
  bool valid=true;
  try{
	  bool valid=matchStep(s,s.begin(),m,0,groups,cb);
  }catch(bool){};
  delete[] m;
  return valid;
}

template <class STRING,class POINTER>
bool StringMatcher::matchStep(STRING& s,POINTER p,PatternMatch<POINTER>* matches,int level,pair<SP<POINTER>,SP<POINTER> >* groups,StringMatcherCallback<POINTER>* cb){
  if(level==count){
    cout<<level<<" at end of pattern so are we at the end of te string? "<<(p==s.end())<<endl;
    if(p==s.end()){
			if(groups!=0){
				for(int i=0;i<group_count;++i){
				  groups[i].first=matches[this->groups[i].first].start;
				  groups[i].second=matches[this->groups[i].second].end;
				}
			}
			if(cb==0||cb->process(groups))
			  throw true;
      return true;
    }else{
      return false;
    }
    
  }
  bool match=false;
  PatternTag& pt=pattern[level].first;
  StringElementCondition& sec=pattern[level].second;
  if(matches)
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
    //go to first that doesn't match or max+1 (whichevers first)
    while(i<pt.max && p!=s.end() && sec.matches(p)){
      ++i;++p;
      cout<<level<<" "<<"stepping to max "<<i<<" "<<p<<" "<<sec.matches(p)<<endl;
    }
    cout<<level<<" "<<"at max (or first fail) "<<i<<" "<<p<<" "<<sec.matches(p)<<endl;
  }
  //while i is still in valid range
  while(i>=pt.min && i<=pt.max){
	  if(matches)
			matches[level].end=SP<POINTER>(new POINTER(p));
    count=count;
    //see if we can match the next group (if matches and we only want one match will throw true else will return as a bool wether it matches)
    match|=matchStep(s,p,matches,level+1,groups,cb);
    
    if(pt.greedy){
      cout<<level<<" stepping back "<<i<<" "<<p<<endl;
      --i;--p;
    }else{
	    cout<<level<<" "<<"stepping up "<<i<<" "<<p<<" "<<sec.matches(p)<<endl;
      if(p==s.end() || !sec.matches(p))
        return false;
      ++i;++p;
    }
  }
  cout<<level<<" tried all options for this pattern"<<endl;
  return match;
}

template <class T>
class Parser:public InputParser{
  InputParser** dataParser;
  T** newT;
  public:
    Used parse(char* data,irr::u32 length,bool eof);
    Parser(InputParser** dataParser,T** newT):dataParser(dataParser),newT(newT){};
    virtual ~Parser();
};
template <>
Used Parser<Condition>::parse(char* data,irr::u32 length,bool eof){
  char* start=data;
  char* end=data+length;
  if(!eof)
    end-=1;
  int type=strtol(data,&data,10);
  if(!eof){
    if(data>=end) return Used(0,false);
  }else{
    if(data>end){
      type=0;
      data=end;
    }
  }
  switch(type){
    case 1:
      *newT=new ConditionTrue();
      break;
    case 2:
      *newT=new ConditionOr();
      break;
    case 3:
      *newT=new ConditionAnd();
      break;
    case 4:
      *newT=new ConditionNot();
      break;
    case 5:
      *newT=new ConditionAfter();
      break;
    case 6:
      *newT=new ConditionBefore();
      break;
    case 7:
      *newT=new ConditionStringPattern();
      break;
    default:
      *newT=new ConditionTrue();
  }
  *dataParser=(*newT)->createParser();
  return Used(data-start,true);
}
template<>
Used Parser<Action>::parse(char* data,irr::u32 length,bool eof){
  char* start=data;
  char* end=data+length;
  if(!eof)
    end-=1;
  int type=strtol(data,&data,10);
  if(!eof){
    if(data>=end) return Used(0,false);
  }else{
    if(data>end){
      type=0;
      data=end;
    }
  }
  switch(type){
    case 1:
      *newT=new ActionMessage();
      break;
    case 2:
      *newT=new ActionBlockWin();
      break;
    case 3:
      *newT=new ActionWinMessage();
      break;
    case 4:
      *newT=new ActionWinNextLevel();
      break;
    case 5:
      *newT=new ActionForceWin();
      break;
    case 6:
      *newT=new ActionSelectStringPattern();
      break;
    case 7:
      *newT=new ActionSetStringRoute();
      break;
    default:
      *newT=new ActionNothing();
      break;
  }
  *dataParser=(*newT)->createParser();
  return Used(data-start,true);
}
template <class T>
Parser<T>::~Parser(){
  (*newT)->returnParser(*dataParser);
}
template <class T>
class ListParser:public InputParser{
  InputParser** dataParser;
  T*** newTs;
  int* count;
  public:
    virtual Used parse(char* data,irr::u32 length,bool eof);
    ListParser(InputParser** dataParser,T*** newTs,int* count):
        dataParser(dataParser),newTs(newTs),count(count){};
    virtual ~ListParser();
};
template <class T>
Used ListParser<T>::parse(char* data,irr::u32 length,bool eof){
  char* start=data;
  char* end=data+length;
  if(!eof)
    end-=1;
  *count=strtol(data,&data,10);
  if(data>=end) return Used(0,false);
  *newTs=new T*[*count];
  InputParser** parsers=new InputParser*[2*(*count)];
  for(int i=0;i<*count;++i)
    parsers[i*2]=new Parser<T>(parsers+i*2+1,(*newTs)+i);
  *dataParser=new SequentialInputParser<Derefer<InputParser,InputParser**> >(
      Derefer<InputParser,InputParser**>(parsers),
      Derefer<InputParser,InputParser**>(parsers+2*(*count)));
  return Used(data-start,true);
}
template <class T>
ListParser<T>::~ListParser(){
  SequentialInputParser<Derefer<InputParser,InputParser**> >* p=(SequentialInputParser<Derefer<InputParser,InputParser**> >*)(*dataParser);
  for(int i=0;i<*count;++i)
    delete *(p->end.data-2*(*count)+2*i);
  delete[] (p->end.data-2*(*count));
  delete p;
}


bool ConditionOr::is(int time,const Script& script,const String& s){
  for(int i=0;i<count;++i)
    if(conditions[i]->is(time,script,s))
      return true;
  return false;
}
InputParser* ConditionOr::createParser(){
  InputParser** parsers=new InputParser*[2];
  parsers[0]=new ListParser<Condition>(parsers+1,&conditions,&count);
  return new SequentialInputParser<Derefer<InputParser,InputParser**> >(
    Derefer<InputParser,InputParser**>(parsers),
    Derefer<InputParser,InputParser**>(parsers+2));
};
void ConditionOr::returnParser(InputParser* parser){
  SequentialInputParser<Derefer<InputParser,InputParser**> >* p=(SequentialInputParser<Derefer<InputParser,InputParser**> >*)parser;
  delete *(p->end.data-2);
  delete[] (p->end.data-2);
  delete parser;
};
void ConditionOr::output(irr::stringc* s,irr::IWriteFile* file){
  (*s)+="2 ";(*s)+=count;(*s)+="\n";
  for(int i=0;i<count;++i)
    conditions[i]->output(s,file);
  if(file && s->size()>256){
    file->write(s->c_str(),s->size());
    *s=irr::stringc();
  }
}
ConditionOr::~ConditionOr(){
  for(int i=0;i<count;++i)
    delete conditions[i];
  delete[] conditions;
}

bool ConditionAnd::is(int time,const Script& script,const String& s){
  for(int i=0;i<count;++i)
    if(!conditions[i]->is(time,script,s))
      return false;
  return true;
}
InputParser* ConditionAnd::createParser(){
  InputParser** parsers=new InputParser*[2];
  parsers[0]=new ListParser<Condition>(parsers+1,&conditions,&count);
  return new SequentialInputParser<Derefer<InputParser,InputParser**> >(
    Derefer<InputParser,InputParser**>(parsers),
    Derefer<InputParser,InputParser**>(parsers+2));
};
void ConditionAnd::returnParser(InputParser* parser){
  SequentialInputParser<Derefer<InputParser,InputParser**> >* p=(SequentialInputParser<Derefer<InputParser,InputParser**> >*)parser;
  delete *(p->end.data-2);
  delete[] (p->end.data-2);
  delete parser;
};
void ConditionAnd::output(irr::stringc* s,irr::IWriteFile* file){
  (*s)+="3 ";(*s)+=count;(*s)+="\n";
  for(int i=0;i<count;++i)
    conditions[i]->output(s,file);
  if(file && s->size()>256){
    file->write(s->c_str(),s->size());
    *s=irr::stringc();
  }
}
ConditionAnd::~ConditionAnd(){
  for(int i=0;i<count;++i)
    delete conditions[i];
  delete[] conditions;
}

InputParser* ConditionNot::createParser(){
  InputParser** parsers=new InputParser*[2];
  parsers[0]=new Parser<Condition>(parsers+1,&condition);
  return new SequentialInputParser<Derefer<InputParser,InputParser**> >(
    Derefer<InputParser,InputParser**>(parsers),
    Derefer<InputParser,InputParser**>(parsers+2));
};
void ConditionNot::returnParser(InputParser* parser){
  SequentialInputParser<Derefer<InputParser,InputParser**> >* p=(SequentialInputParser<Derefer<InputParser,InputParser**> >*)parser;
  delete *(p->end.data-2);
  delete[] (p->end.data-2);
  delete parser;
};
void ConditionNot::output(irr::stringc* s,irr::IWriteFile* file){
  (*s)+="4\n";
  condition->output(s,file);
  if(file && s->size()>256){
    file->write(s->c_str(),s->size());
    *s=irr::stringc();
  }
}

bool ConditionAfter::is(int time,const Script& script,const String& s){
  return script.getTime(event)+delay<=time;
}
Used ConditionAfter::parse(char* data,irr::u32 length,bool eof){
  char* start=data;
  char* end=data+length;
  if(!eof)
    end-=1;
  event=strtol(data,&data,10);
  if(data>=end) return Used(0,false);
  delay=strtol(data,&data,10);
  if(data>=end) return Used(0,false);
  return Used(data-start,true);
}
void ConditionAfter::output(irr::stringc* s,irr::IWriteFile* file){
  (*s)+="5 ";(*s)+=event;(*s)+=" ";(*s)+=delay;(*s)+="\n";
  if(file && s->size()>256){
    file->write(s->c_str(),s->size());
    *s=irr::stringc();
  }
}

bool ConditionBefore::is(int time,const Script& script,const String& s){
  return script.getTime(event)==0;
}
Used ConditionBefore::parse(char* data,irr::u32 length,bool eof){
  char* start=data;
  char* end=data+length;
  if(!eof)
    end-=1;
  event=strtol(data,&data,10);
  if(data>=end) return Used(0,false);
  return Used(data-start,true);
}
void ConditionBefore::output(irr::stringc* s,irr::IWriteFile* file){
  (*s)+="6 ";(*s)+=event;(*s)+="\n";
  if(file && s->size()>256){
    file->write(s->c_str(),s->size());
    *s=irr::stringc();
  }
}

void Message::output(irr::stringc* s,irr::IWriteFile* file){
  *s+=count;
  *s+="\n";
  const char* const delims="\"'|^_!\\\3";
  for(int i=0;i<count;++i){
    *s+=paragraphs[i].a;
    *s+=" ";
    const char* delim=delims;
    while(paragraphs[i].b.findFirst(*delim)!=-1)
      ++delim; // if this fails then you have a string with '\3' in it!!!!!!!!
    *s+=*delim;
    *s+=paragraphs[i].b;
    *s+=*delim;
    *s+="\n";
    if(file && s->size()>256){
      file->write(s->c_str(),s->size());
      *s=irr::stringc();
    }
  }
}

class MessageParser: public InputParser{
  Message* m;
  int pos;
  irr::stringc lines;
  char delim;
  public:
    MessageParser(Message* m):m(m),pos(-1),lines(),delim(0){};
    Used parse(char* data,irr::u32 length,bool eof){
      char* start=data;
      char* end=data+length;
      if(!eof)
        end-=1;
      if(pos==-1){
        int len=strtol(data,&data,10);
        if(data>=end) return Used(0,false);
        m->paragraphs=new Pair<irr::stringc,irr::stringc>[len];
        m->count=len;
        pos=0;
      }
      
      char* tmp;
      while(pos<2*m->count){
        if(pos%2==0){
          while(isspace(*data))
            if(++data>=end)
              return Used(data-start,false);
          tmp=data;
          while(!isspace(*tmp))
            if(++tmp>=end)
              return Used(data-start,false);
          m->paragraphs[pos/2].a=irr::stringc(data,tmp-data);
          data=tmp;
          ++pos;
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
        m->paragraphs[pos/2].b=lines;
        lines=irr::stringc();
        delim=0;
        ++pos;
        data=tmp+1;
      }
      return Used(data-start,true);
    }
};

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
InputParser* ActionMessage::createParser(){
  return new MessageParser(&m);
}
void ActionMessage::output(irr::stringc* s,irr::IWriteFile* file){
  *s+="1 ";
  m.output(s,file);
}
InputParser* ActionWinMessage::createParser(){
  return new MessageParser(&m);
}
void ActionWinMessage::output(irr::stringc* s,irr::IWriteFile* file){
  *s+="3 ";
  m.output(s,file);
}

class dataDescParser:public InputParser{
  Pair<irr::stringc,irr::stringc>* d;
  int pos;
  irr::stringc lines;
  char delim;
  public:
    dataDescParser  (Pair<irr::stringc,irr::stringc>* d):d(d),pos(0),lines(),delim(0){};
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

bool ActionSetStringRoute::process(pair<SP<StringPointer>,SP<StringPointer> >* groups){
  for(int i=0;i<ranges.groupCount();++i){
    se->setStringSegment(*groups[i].first,*groups[i].second,count,route);
  }
  return !all;
}

void ActionSetStringRoute::doCommon(ScriptResponse& r,String& s){
  if(!ranges.groupCount())
    return;
  se=new StringEdit(s);
  if(ranges.match(s,this))
	  r.stringChanged=true;
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


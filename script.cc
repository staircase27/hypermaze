#include <cctype>
#define USE_CLIM
#ifdef USE_CLIM
#include <climits>
#else
#define INT_MAX std::numeric_limits<int>::max()
#endif
#include "script.hh"
#include "scriptimpl.hh"

void StringElementCondition::output(irr::stringc* s,irr::IWriteFile* file){
  if((selectionCondition&1)==0)
    (*s)+="*";
  else if((selectionCondition&2)==0)
    (*s)+="N";
  else
    (*s)+="Y";
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
      ++stage;
      ++data;
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

void StringMatcher::output(irr::stringc* s,irr::IWriteFile* file){
  (*s)+=count;
  (*s)+="\n";
  for(int i=0;i<count;++i){
    (*s)+=pattern[i].first.min;
    (*s)+=" ";
    (*s)+=pattern[i].first.max;
    (*s)+=" ";
    (*s)+=pattern[i].first.greedy;
    (*s)+="\n";
    pattern[i].second.output(s,file);
  }
  (*s)+=group_count;
  for(int i=0;i<group_count;++i){
    (*s)+=groups[i].first;
    (*s)+=" ";
    (*s)+=groups[i].second;
    (*s)+=" ";
  }
};

InputParser* StringMatcher::createParser(){}
void StringMatcher::returnParser(InputParser*){}

bool StringMatcher::match(const String& s,pair<SP<ConstStringPointer>,SP<ConstStringPointer> >* groups){
  PatternMatch* m=groups==0?0:new PatternMatch[count];
  bool valid=matchStep(s,s.begin(),m,0);
  if(groups!=0){
    for(int i=0;i<group_count;++i){
      groups[i].first=m[this->groups[i].first].start;
      groups[i].second=m[this->groups[i].second].end;
    }
  }
  delete[] m;
  return valid;
}

bool StringMatcher::matchStep(const String& s,ConstStringPointer p,PatternMatch* matches,int level){
  if(level==count){
    cout<<level<<" at end of pattern so are we at the end of te string? "<<(p==s.end())<<endl;
    return p==s.end();
  }
  PatternTag& pt=pattern[level].first;
  StringElementCondition& sec=pattern[level].second;
  if(matches)
    matches[level].start=SP<ConstStringPointer>(new ConstStringPointer(p));
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
    while(i<pt.max&&p!=s.end()&&sec.matches(p)){
      ++i;++p;
      cout<<level<<" "<<"stepping to max "<<i<<" "<<p<<" "<<sec.matches(p)<<endl;
    }
    cout<<level<<" "<<"at max (or first fail) "<<i<<" "<<p<<" "<<sec.matches(p)<<endl;
  }
  //while i is still in valid range
  while(i>=pt.min && i<=pt.max){
    //see if we can match the next group
    if(matchStep(s,p,matches,level+1)){
      cout<<level<<" found a match"<<endl;
		  if(matches)
				matches[level].end=SP<ConstStringPointer>(new ConstStringPointer(p));
      count=count;
      return true;
    }
    if(pt.greedy){
      cout<<level<<" stepping back "<<i<<" "<<p<<endl;
      --i;--p;
    }else{
	    cout<<level<<" "<<"stepping up "<<i<<" "<<p<<" "<<sec.matches(p)<<endl;
      if(!sec.matches(p))
        return false;
      ++i;++p;
    }
  }
  cout<<level<<" no match for trailing groups (but we matched over all allowed repeats)"<<endl;
  //didn't find a match in the valid range
  return false;
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
      *newT=new ConditionStringPosition();
      break;
    case 8:
      *newT=new ConditionStringSelection();
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
//    case 7:
//      *newT=new ConditionStringPosition();
//      break;
//    case 8:
//      *newT=new ActionStringSelectionPattern();
//      break;
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


bool ConditionOr::is(int time,Script script,const String& s){
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

bool ConditionAnd::is(int time,Script script,const String& s){
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

bool ConditionAfter::is(int time,Script script,const String& s){
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

bool ConditionBefore::is(int time,Script script,const String& s){
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

class ConditionStringPositionParser: public InputParser{
  ConditionStringPosition* c;
  int i;
  public:
    ConditionStringPositionParser(ConditionStringPosition* c):c(c),i(-2){};
  
    Used parse(char* data,irr::u32 length,bool eof){
      char* start=data;
      char* end=data+length;
      if(!eof)
        end-=1;
      if(i==-1){
        c->tiestart=strtol(data,&data,10);
        if(data>=end) return Used(0,false);

        c->tieend=strtol(data,&data,10);
        if(data>=end) return Used(0,false);

        c->count=strtol(data,&data,10);
        if(data>=end) return Used(0,false);
        c->poss=new Vector[c->count];
        i=0;
      }
      
      char* tmp;
      while(i<c->count){
        c->poss[i].X=strtol(data,&tmp,10);
        if(tmp>=end) return Used(data-start,false);
        c->poss[i].Y=strtol(tmp,&tmp,10);
        if(tmp>=end) return Used(data-start,false);
        c->poss[i].Z=strtol(tmp,&tmp,10);
        if(tmp>=end) return Used(data-start,false);
        ++i;
        data=tmp;
      }
      return Used(data-start,true);
    }
};
InputParser* ConditionStringPosition::createParser(){
  return new ConditionStringPositionParser(this);
}
void ConditionStringPosition::output(irr::stringc* s,irr::IWriteFile* file){
  (*s)+="7 ";(*s)+=tiestart;(*s)+=" ";(*s)+=tieend;(*s)+=" ";(*s)+=count;(*s)+=" ";
  for(int i=0;i<count;++i){
    (*s)+=poss[i].X;(*s)+=" ";(*s)+=poss[i].Y;(*s)+=" ";(*s)+=poss[i].Z;(*s)+=" ";
  }
  (*s)+="\n";
  if(file && s->size()>256){
    file->write(s->c_str(),s->size());
    *s=irr::stringc();
  }
}
ConditionStringPosition::~ConditionStringPosition(){
  delete[] poss;
}

class ConditionStringSelectionParser: public InputParser{
  ConditionStringSelection* c;
  int i;
  public:
    ConditionStringSelectionParser(ConditionStringSelection* c):c(c),i(-2){};
  
    Used parse(char* data,irr::u32 length,bool eof){
      char* start=data;
      char* end=data+length;
      if(!eof)
        end-=1;
      if(i==-1){
        c->tiestart=strtol(data,&data,10);
        if(data>=end) return Used(0,false);

        c->tieend=strtol(data,&data,10);
        if(data>=end) return Used(0,false);

        c->count=strtol(data,&data,10);
        if(data>=end) return Used(0,false);
        c->sels=new bool[c->count];
        i=0;
      }
      
      char* tmp;
      while(i<c->count){
        c->sels[i]=strtol(data,&tmp,10);
        if(tmp>=end) return Used(data-start,false);
        ++i;
        data=tmp;
      }
      return Used(data-start,true);
    }
};
InputParser* ConditionStringSelection::createParser(){
  return new ConditionStringSelectionParser(this);
}
void ConditionStringSelection::output(irr::stringc* s,irr::IWriteFile* file){
  (*s)+="7 ";(*s)+=tiestart;(*s)+=" ";(*s)+=tieend;(*s)+=" ";(*s)+=count;(*s)+=" ";
  for(int i=0;i<count;++i)
    (*s)+=sels[i]+=" ";
  (*s)+="\n";
  if(file && s->size()>256){
    file->write(s->c_str(),s->size());
    *s=irr::stringc();
  }
}
ConditionStringSelection::~ConditionStringSelection(){
  delete[] sels;
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


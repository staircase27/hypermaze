#include "irrdisp.hh"
#include "controller.hh"
#include "script.hh"

#ifdef IOSTREAM
#include <iostream>
#endif

void MazeDisplay::init(Maze& m,NodeGen* ng,irr::vector3df center){
  for(set<Dirn>::iterator d=dirns.begin();d!=dirns.end();++d){
    limits[*d].first.first=limits[*d].first.second=0;
    limits[*d].second.first=*d;
    limits[*d].second.second=true;
    limits[opposite(*d)].first.first=limits[opposite(*d)].first.second=2*(m.size.dotProduct(to_vector(*d))-1);
    limits[opposite(*d)].second.first=*d;
    limits[opposite(*d)].second.second=false;

    nodes[*d]=new vector<vector<VisibleCounter*>*>(
        2*m.size.dotProduct(to_vector(*d))-1,(vector<VisibleCounter*>*)NULL);
  }

  irr::vector3df position=center-(wall+gap)*con(m.size-Vector(1,1,1))/2;

  for(int x=0;x<m.size.X;++x)
    for(int y=0;y<m.size.Y;++y)
      for(int z=0;z<m.size.Z;++z){
        Vector pos(x,y,z);
        irr::IMeshSceneNode* node = ng->makeUnitWall(true);
        node->grab();

        node->setScale(irr::vector3df(wall,wall,wall));
        node->setPosition(position+con(pos)*(wall+gap));

        VisibleCounter* vc=new VisibleCounter(node);
        for(set<Dirn>::iterator dir=dirns.begin();dir!=dirns.end();++dir){
          if((*nodes[*dir])[2*pos.dotProduct(to_vector(*dir))]==0)
            (*nodes[*dir])[2*pos.dotProduct(to_vector(*dir))]=new vector<VisibleCounter*>();
          (*nodes[*dir])[2*pos.dotProduct(to_vector(*dir))]->push_back(vc);
        }

        for(set<Dirn>::iterator d=dirns.begin();d!=dirns.end();++d)
          if((*m[Vector(x,y,z)]&to_mask(*d))!=0){
            irr::IMeshSceneNode* node = ng->makeUnitWall(false);
            node->grab();

            node->setScale(wall*irr::vector3df(1,1,1)+(gap-wall)*remSgn(con(to_vector(*d))));

            node->setPosition(position+con(pos)*(wall+gap)+con(to_vector(*d))*(wall+gap)/2);

            VisibleCounter* vc=new VisibleCounter(node);
            for(set<Dirn>::iterator dir=dirns.begin();dir!=dirns.end();++dir){
              if((*nodes[*dir])[(2*pos+to_vector(*d)).dotProduct(to_vector(*dir))]==0)
                (*nodes[*dir])[(2*pos+to_vector(*d)).dotProduct(to_vector(*dir))]=new vector<VisibleCounter*>();
              (*nodes[*dir])[(2*pos+to_vector(*d)).dotProduct(to_vector(*dir))]->push_back(vc);
            }
          }
      }
}

void MazeDisplay::clear(){
  for(vector<vector<VisibleCounter*>*>::iterator layer=nodes[UP]->begin();layer!=nodes[UP]->end();++layer){
    if(*layer==0)
      continue;
    for(vector<VisibleCounter*>::iterator node=(*layer)->begin();node!=(*layer)->end();++node){
      (*node)->node->remove();
      (*node)->node->drop();
      delete *node;
    }
  }
  for(set<Dirn>::iterator d=dirns.begin();d!=dirns.end();++d){
    for(vector<vector<VisibleCounter*>*>::iterator layer=nodes[*d]->begin();layer!=nodes[*d]->end();++layer){
      if(*layer!=0)
        delete *layer;
    }
    delete nodes[*d];
  }
  nodes.clear();
  limits.clear();
}

bool MazeDisplay::hideSide(Dirn side,bool out){
  if(limits.empty())
    return false;
  pair<pair<int,int>,pair<Dirn,bool> >& data=limits[side];
  if(!out){
    if(limits[opposite(side)].first.first==data.first.first)
      return false;
    vector<VisibleCounter*>* ns=(*nodes[data.second.first])[data.first.first];
    if(!(ns==0))
      for(vector<VisibleCounter*>::iterator it=ns->begin();it!=ns->end();++it)
        (*it)->setVisible(false);
  }else if(data.first.first==data.first.second)
      return false;
  if(out==data.second.second)
    data.first.first--;
  else
    data.first.first++;
  if(out){
    vector<VisibleCounter*>* ns=(*nodes[data.second.first])[data.first.first];
    if(!(ns==0))
      for(vector<VisibleCounter*>::iterator it=ns->begin();it!=ns->end();++it)
        (*it)->setVisible(true);
  }
  return true;
}
void StringDisplay::update(){
  irr::vector3df position=center-(MazeDisplay::wall+MazeDisplay::gap)*con(s.maze.size)/2;
  bool active=false;
  int i=0;
  list<irr::IMeshSceneNode*>::iterator nit=nodes.begin();
  StringPointer sit=s.begin();
  
  startEnd->setPosition(position+con(s.getStart())*(MazeDisplay::wall+MazeDisplay::gap)
      -con(to_vector(s.stringDir))*(MazeDisplay::wall/2+MazeDisplay::gap/2));
  startEnd->setScale(MazeDisplay::wall*irr::vector3df(1,1,1)+(MazeDisplay::gap-MazeDisplay::wall)*remSgn(con(to_vector(s.stringDir))));
  endEnd->setPosition(position+con(s.getEnd())*(MazeDisplay::wall+MazeDisplay::gap)
      +con(to_vector(s.stringDir))*(MazeDisplay::wall/2+MazeDisplay::gap/2));
  endEnd->setScale(MazeDisplay::wall*irr::vector3df(1,1,1)+(MazeDisplay::gap-MazeDisplay::wall)*remSgn(con(to_vector(s.stringDir))));
  
  while(sit!=s.end()){

    if(nit==nodes.end()){
      nit=nodes.insert(nit,ng->makeUnitString(true));
      (*nit)->setID(STRING_ID+i);
    }
    (*nit)->setScale(MazeDisplay::wall*irr::vector3df(1,1,1));
    (*nit)->setPosition(position+con(sit->pos)*(MazeDisplay::wall+MazeDisplay::gap));
    if(i>=activeNodes)
      (*nit)->setVisible(true);

    ng->makeStringActive(*nit,active||sit->selected);
    active=sit->selected;

    ++nit;
    ++i;

    if(nit==nodes.end()){
      nit=nodes.insert(nit,ng->makeUnitString(false));
      (*nit)->setID(STRING_ID+i);
    }
    (*nit)->setScale(MazeDisplay::wall*irr::vector3df(1,1,1)+(MazeDisplay::gap-MazeDisplay::wall)*remSgn(con(to_vector(sit->d))));
    (*nit)->setPosition(position+con(sit->pos)*(MazeDisplay::wall+MazeDisplay::gap)+(MazeDisplay::gap+MazeDisplay::wall)*con(to_vector(sit->d))/2);
    if(i>=activeNodes)
      (*nit)->setVisible(true);

    ng->makeStringActive(*nit,active);

    ++nit;
    ++i;

    ++sit;

  }
  if(nit==nodes.end()){
    nit=nodes.insert(nit,ng->makeUnitString(true));
    (*nit)->setID(STRING_ID+i);
  }
  (*nit)->setScale(MazeDisplay::wall*irr::vector3df(1,1,1));
  (*nit)->setPosition(position+con(s.getEnd())*(MazeDisplay::wall+MazeDisplay::gap));
  if(i>=activeNodes)
    (*nit)->setVisible(true);

  ng->makeStringActive(*nit,active);
  ++nit;
  ++i;

  int j=i;
  while(j<activeNodes){
    (*nit)->setVisible(false);
    ++j;
    ++nit;
  }
  activeNodes=i;
}
void StringDisplay::updateActive(){
  bool active=false;
  list<irr::IMeshSceneNode*>::iterator nit=nodes.begin();
  StringPointer sit=s.begin();
  while(sit!=s.end()){
    ng->makeStringActive(*nit,active||sit->selected);
    active=sit->selected;

    ++nit;
    ng->makeStringActive(*nit,active);
    ++nit;
    ++sit;

  }
  ng->makeStringActive(*nit,active);
}

pair<StringPointer,bool> StringDisplay::getStringPointer(irr::ISceneNode* node){
  if(node==0)
    return pair<StringPointer,bool>(s.end(),false);
  int i=node->getID()-STRING_ID;
  if(i<0)
    return pair<StringPointer,bool>(s.end(),false);
  
  StringPointer sit=s.begin();
  while(sit!=s.end()){
    if(i==0)
      return pair<StringPointer,bool>(sit,true);
    --i;
    if(i==0)
      return pair<StringPointer,bool>(sit,false);
    ++sit;
    --i;
  }
  if(i==0)
    return pair<StringPointer,bool>(sit,true);
  else
    return pair<StringPointer,bool>(sit,false);
}


const map<irr::ISceneNode*,Dirn>& PuzzleDisplay::getSlicers(){
  return slicers;
}

pair<StringPointer,bool> PuzzleDisplay::getStringPointer(irr::ISceneNode* node){
  return sd.getStringPointer(node);
}

void PuzzleDisplay::win(){
  ScriptResponseWin r=sc.runWin(s);
  if(r.stringChanged)
    sd.update();
  else if(r.stringSelectionChanged)
    sd.updateActive();
  if(r.messageCount>0)
#ifdef IOSTREAM
    cout<<"onwin message"<<endl;
#endif
    ;//TODO show message
  if(r.block)
    return;
  won=true;
  if(c!=0){
    cout<<"on win"<<endl;
    cout<<r.nextLevel.a<<endl;
    c->onWin();//TODO use the nextlevel and win message
  }
}

void PuzzleDisplay::stringUpdated(){
  ScriptResponseMove r=sc.runMove(s);
  if(r.messageCount>0)
#ifdef IOSTREAM
    cout<<"onmove message"<<endl;
#endif
    ;//TODO show message
  sd.update();
  if((!won) && (s.hasWon()||r.forceWin))
    win();
};
void PuzzleDisplay::stringSelectionUpdated(){
  ScriptResponseSelect r=sc.runSelect(s);
  if(r.stringChanged)
    sd.update();
  if(r.messageCount>0)
#ifdef IOSTREAM
    cout<<"onselect message"<<endl;
#endif
    ;//TODO show message
  sd.updateActive();
  if((!won) && r.forceWin)
    win();
};
void PuzzleDisplay::mazeUpdated(){
    md.clear();
    md.init(m,ng);
    s=String(m);
    sp=StringPlay(s);
    sd.setString(s);
    for(map<irr::ISceneNode*,Dirn>::iterator slicer=slicers.begin();slicer!=slicers.end();++slicer)
      slicer->first->setPosition(-con(to_vector(slicer->second))*(abs(to_vector(slicer->second).dotProduct(m.size))/2+2)*(md.wall+md.gap));
    won=false;
    ScriptResponseStart r=sc.runStart(s);
    if(r.stringChanged)
      sd.update();
    else if(r.stringSelectionChanged)
      sd.updateActive();
    if(r.messageCount>0)
#ifdef IOSTREAM
    cout<<"onstart message"<<endl;
#endif
      ;//TODO show message
};
bool PuzzleDisplay::hideSide(Dirn side,bool out){
  return md.hideSide(side,out);
};


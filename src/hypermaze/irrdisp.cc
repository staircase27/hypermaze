/**
 * @file irrdisp.cc
 * @brief The implementation of irrdisp.hh
 */
#include "irrdisp.hh"
#include "irrdispimp.hh"
#include "controller.hh"
#include "../core/script.hh"
#include "guis.hh"

#ifdef IOSTREAM
#include <iostream>
#endif

using namespace std;
namespace irr{
  using namespace core;
  using namespace scene;
}

const double WALL_SIZE = 5;
const double GAP_SIZE = 20;

void MazeDisplay::init(Maze& m,NodeGen* ng,irr::vector3df center){
  for(set<Dirn>::iterator d=dirns.begin();d!=dirns.end();++d){
    limits[*d].first.first=limits[*d].first.second=0;
    limits[*d].second.first=*d;
    limits[*d].second.second=true;
    limits[opposite(*d)].first.first=limits[opposite(*d)].first.second=2*(m.size().dotProduct(to_vector(*d))-1);
    limits[opposite(*d)].second.first=*d;
    limits[opposite(*d)].second.second=false;

    nodes[*d]=new vector<vector<VisibleCounter*>*>(
        2*m.size().dotProduct(to_vector(*d))-1,(vector<VisibleCounter*>*)NULL);
  }

  irr::vector3df position=center-(WALL_SIZE+GAP_SIZE)*con(m.size()-Vector(1,1,1))/2;

  for(int x=0;x<m.size().X;++x)
    for(int y=0;y<m.size().Y;++y)
      for(int z=0;z<m.size().Z;++z){
        Vector pos(x,y,z);
        irr::IMeshSceneNode* node = ng->makeUnitWall(true);
        node->grab();

        node->setScale(irr::vector3df(WALL_SIZE,WALL_SIZE,WALL_SIZE));
        node->setPosition(position+con(pos)*(WALL_SIZE+GAP_SIZE));

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

            node->setScale(WALL_SIZE*irr::vector3df(1,1,1)+(GAP_SIZE-WALL_SIZE)*remSgn(con(to_vector(*d))));

            node->setPosition(position+con(pos)*(WALL_SIZE+GAP_SIZE)+con(to_vector(*d))*(WALL_SIZE+GAP_SIZE)/2);

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
  irr::vector3df position=center-(WALL_SIZE+GAP_SIZE)*con(s->maze.size())/2;
  bool active=false;
  int i=0;
  list<irr::IMeshSceneNode*>::iterator nit=nodes.begin();
  StringPointer sit=s->begin();

  startEnd->setPosition(position+con(s->getStart())*(WALL_SIZE+GAP_SIZE)
      -con(to_vector(s->stringDir))*(WALL_SIZE/2+GAP_SIZE/2));
  startEnd->setScale(WALL_SIZE*irr::vector3df(1,1,1)+(GAP_SIZE-WALL_SIZE)*remSgn(con(to_vector(s->stringDir))));
  endEnd->setPosition(position+con(s->getEnd())*(WALL_SIZE+GAP_SIZE)
      +con(to_vector(s->stringDir))*(WALL_SIZE/2+GAP_SIZE/2));
  endEnd->setScale(WALL_SIZE*irr::vector3df(1,1,1)+(GAP_SIZE-WALL_SIZE)*remSgn(con(to_vector(s->stringDir))));

  while(sit!=s->end()){

    if(nit==nodes.end()){
      nit=nodes.insert(nit,ng->makeUnitString(true));
      (*nit)->setID(STRING_ID+i);
    }
    (*nit)->setScale(WALL_SIZE*irr::vector3df(1,1,1));
    (*nit)->setPosition(position+con(sit->pos)*(WALL_SIZE+GAP_SIZE));
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
    (*nit)->setScale(WALL_SIZE*irr::vector3df(1,1,1)+(GAP_SIZE-WALL_SIZE)*remSgn(con(to_vector(sit->d))));
    (*nit)->setPosition(position+con(sit->pos)*(WALL_SIZE+GAP_SIZE)+(GAP_SIZE+WALL_SIZE)*con(to_vector(sit->d))/2);
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
  (*nit)->setScale(WALL_SIZE*irr::vector3df(1,1,1));
  (*nit)->setPosition(position+con(s->getEnd())*(WALL_SIZE+GAP_SIZE));
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
  StringPointer sit=s->begin();
  while(sit!=s->end()){
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
    return pair<StringPointer,bool>(s->end(),false);
  int i=node->getID()-STRING_ID;
  if(i<0)
    return pair<StringPointer,bool>(s->end(),false);

  StringPointer sit=s->begin();
  while(sit!=s->end()){
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

PuzzleDisplay::PuzzleDisplay(NodeGen* ng,irr::IrrlichtDevice* device,FontManager* fm,SoundManager* sm):m(Vector(5,5,5)),sc(),s(new String(m)),sp(s),ng(ng),md(new MazeDisplay(m,ng)),sd(new StringDisplay(s,ng)),won(false),device(device),fm(fm),sm(sm){
  for(Dirn* d=allDirns;d!=allDirns+6;++d){
    irr::scene::IMeshSceneNode* node = ng->makeUnitHandle(to_vector(*d).dotProduct(to_vector(s->targetDir)));
    node->setScale(irr::core::vector3df(1,1,1)*(WALL_SIZE+GAP_SIZE)/2);
    node->setPosition(-con(to_vector(*d))*(abs(to_vector(*d).dotProduct(m.size()))/2+2)*(WALL_SIZE+GAP_SIZE));
    slicers[node]=*d;
  }
};

SP<Dirn> PuzzleDisplay::getSlicerDirn(irr::ISceneNode* slicer){
    map<irr::ISceneNode*,Dirn>::iterator it=slicers.find(slicer);
    if(it==slicers.end()){
        return SP<Dirn>();
    }else{
        return SP<Dirn>(new Dirn(it->second));
    }
}

pair<StringPointer,bool> PuzzleDisplay::getStringPointer(irr::ISceneNode* node){
  return sd->getStringPointer(node);
}

void PuzzleDisplay::win(MultiInterfaceController* c){
  ScriptResponseWin r=sc.runWin(s);
  if(r.stringChanged)
    sd->update();
  else if(r.stringSelectionChanged)
    sd->updateActive();
  if(r.stringChanged||r.stringSelectionChanged)
    sp.externalEditHappened();
  if(r.messageCount>0){
    MessageGui g;
    c->showGUI(false);
    for(int i=0;i<r.messageCount;++i)
      g.message(device,fm,r.messages[i]);
    c->showGUI(true);
  }
  if(r.block)
    return;
  won=true;
  if(sm)
    sm->playEffect(SoundManager::SE_WIN);
  if(device){
    c->showGUI(false);
    WinGui wg;
    if(wg.won(device,fm,*this,r.winMessage,r.nextLevel))
      mazeUpdated(c);
    c->showGUI(true);
  }
}

void PuzzleDisplay::stringUpdated(MultiInterfaceController* c){
  ScriptResponseMove r=sc.runMove(s);
  sd->update();
  if(r.stringChanged||r.stringSelectionChanged)
    sp.externalEditHappened();
  if(r.messageCount>0){
    MessageGui g;
    c->showGUI(false);
    for(int i=0;i<r.messageCount;++i)
      g.message(device,fm,r.messages[i]);
    c->showGUI(true);
  }
  if((!won) && (s->hasWon()||r.forceWin))
    win(c);
};
void PuzzleDisplay::stringSelectionUpdated(MultiInterfaceController* c){
  ScriptResponseSelect r=sc.runSelect(s);
  if(r.stringChanged)
    sd->update();
  else
    sd->updateActive();
  if(r.stringChanged||r.stringSelectionChanged)
    sp.externalEditHappened();
  if(r.messageCount>0){
    MessageGui g;
    c->showGUI(false);
    for(int i=0;i<r.messageCount;++i)
      g.message(device,fm,r.messages[i]);
    c->showGUI(true);
  }
  if((!won) && r.forceWin)
    win(c);
};
void PuzzleDisplay::mazeUpdated(MultiInterfaceController* c){
  md->clear();
  md->init(m,ng);
  s=SP<String>(new String(m));
  sp.SetString(s);
  sd->setString(s);
  for(map<irr::ISceneNode*,Dirn>::iterator slicer=slicers.begin();slicer!=slicers.end();++slicer)
    slicer->first->setPosition(-con(to_vector(slicer->second))*(abs(to_vector(slicer->second).dotProduct(m.size()))/2+2)*(WALL_SIZE+GAP_SIZE));
  won=false;
  ScriptResponseStart r=sc.runStart(s);
  if(r.stringChanged)
    sd->update();
  else if(r.stringSelectionChanged)
    sd->updateActive();
  if(r.messageCount>0){
    MessageGui g;
    c->showGUI(false);
    for(int i=0;i<r.messageCount;++i)
      g.message(device,fm,r.messages[i]);
    c->showGUI(true);
  }
};
bool PuzzleDisplay::hideSide(Dirn side,bool out){
  return md->hideSide(side,out);
};

PuzzleDisplay::~PuzzleDisplay(){
  delete md;
  delete sd;
}

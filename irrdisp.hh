#include "irrlicht.h"
#include "maze.hh"
#include "string.hh"
#include <map>
#include <vector>
#include <list>
#include "dirns.hh"

#ifndef IRRDISP_HH_INC
#define IRRDISP_HH_INC

namespace irr{
  using namespace core;
  using namespace scene;
  using namespace io;
  using namespace video;
};

using namespace std;

irr::vector3df con(Vector v){
  return irr::vector3df(-v.X,v.Y,v.Z);
}

irr::vector3df remSgn(irr::vector3df v){
  return irr::vector3df(abs(v.X),abs(v.Y),abs(v.Z));
}

class NodeGen{
  public:
    virtual irr::IMeshSceneNode* makeUnitWall(bool isNode)=0;
    virtual irr::IMeshSceneNode* makeUnitString(bool isNode)=0;
    virtual void makeStringActive(irr::IMeshSceneNode* node,bool active)=0;
    virtual irr::IMeshSceneNode* makeUnitHandle()=0;
};


class VisibleCounter{
  int visible;
  public:
  irr::IMeshSceneNode* node;
    VisibleCounter(irr::IMeshSceneNode* node):node(node),visible(0){};
    bool setVisible(bool isVisible){
      if(isVisible)
        visible++;
      else
        visible--;
      if(visible<0&&node->isVisible()){
        node->setVisible(false);
        return true;
      }else if(visible>=0&&!node->isVisible()){
        node->setVisible(true);
        return true;
      }
      return false;
    }
};

class MazeDisplay{
  map<Dirn,pair<pair<int,int>,pair<Dirn,bool> > > limits;
  map<Dirn,vector<vector<VisibleCounter*>*>*> nodes;
  set<Dirn> dirns;
  public:
    static const double wall=5;
    static const double gap=20;

    void init(Maze& m,NodeGen* ng,irr::vector3df center=irr::vector3df(0,0,0)){
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

    void clear(){
      for(vector<vector<VisibleCounter*>*>::iterator layer=nodes[UP]->begin();layer!=nodes[UP]->end();++layer){
        if(*layer==0)
          continue;
        for(vector<VisibleCounter*>::iterator node=(*layer)->begin();node!=(*layer)->end();++node){
          (*node)->node->remove();
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

    MazeDisplay(Maze& m,NodeGen* ng,irr::vector3df center=irr::vector3df(0,0,0)){
      dirns.insert(UP);
      dirns.insert(LEFT);
      dirns.insert(FORWARD);
      init(m,ng,center);
    }

    bool hideSide(Dirn side,bool out){
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
};

class StringDisplay{
  StringSlice& s;
  list<irr::IMeshSceneNode*> nodes;
  int activeNodes;
  NodeGen* ng;
  irr::vector3df center;
  static const int STRING_ID=1536;//(3<<9)
  public:

    void update(){
      irr::vector3df position=center-(MazeDisplay::wall+MazeDisplay::gap)*con(s.getString().maze.size)/2;
      bool active=false;
      int i=0;
      list<irr::IMeshSceneNode*>::iterator nit=nodes.begin();
      StringPointer sit=s.getString().begin();
      while(sit!=s.getString().end()){

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
      (*nit)->setPosition(position+con(s.getString().getEnd())*(MazeDisplay::wall+MazeDisplay::gap));
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
    void updateActive(){
      bool active=false;
      list<irr::IMeshSceneNode*>::iterator nit=nodes.begin();
      StringPointer sit=s.getString().begin();
      while(sit!=s.getString().end()){
        ng->makeStringActive(*nit,active||sit->selected);
        active=sit->selected;

        ++nit;
        ng->makeStringActive(*nit,active);
        ++nit;
        ++sit;

      }
      ng->makeStringActive(*nit,active);
    }
    
    pair<StringPointer,bool> getStringPointer(irr::ISceneNode* node){
      cout<<node<<" "<<endl<<flush;
      if(node==0)
        return pair<StringPointer,bool>(s.getString().end(),false);
      int i=node->getID()-STRING_ID;
      cout<<i<<endl<<flush;
      if(i<0)
        return pair<StringPointer,bool>(s.getString().end(),false);
      
      StringPointer sit=s.getString().begin();
      while(sit!=s.getString().end()){
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
      
    StringDisplay(StringSlice& s,NodeGen* ng,irr::vector3df center=irr::vector3df(0,0,0)):s(s),center(center),ng(ng),activeNodes(0){
      update();
    };
    void setString(StringSlice _s){
      s=_s;
      update();
    }
};



class PuzzleDisplay{
  public:
    Maze m;
    String s;
    StringSlice ss;
  private:
    NodeGen* ng;
    MazeDisplay md;
    StringDisplay sd;
    map<irr::ISceneNode*,Dirn> slicers;
   public:
    PuzzleDisplay(NodeGen* ng):m(Vector(5,5,5)),s(m),ss(s),ng(ng),md(m,ng),sd(ss,ng){
      for(Dirn* d=allDirns;d!=allDirns+6;++d){
        irr::IMeshSceneNode* node = ng->makeUnitHandle();
        node->setScale(irr::vector3df(1,1,1)*(md.wall+md.gap)/2);
        node->setPosition(-con(to_vector(*d))*(abs(to_vector(*d).dotProduct(m.size))/2+2)*(md.wall+md.gap));
        slicers[node]=*d;
      }
    };

    const map<irr::ISceneNode*,Dirn>& getSlicers(){
      return slicers;
    }
    
    pair<StringPointer,bool> getStringPointer(irr::ISceneNode* node){
      return sd.getStringPointer(node);
    }

    void stringUpdated(){
      sd.update();
    };
    void stringSelectionUpdated(){
      sd.updateActive();
    };
    void mazeUpdated(){
        md.clear();
        md.init(m,ng);
        s=String(m);
        ss=StringSlice(s);
        sd.setString(ss);
        for(map<irr::ISceneNode*,Dirn>::iterator slicer=slicers.begin();slicer!=slicers.end();++slicer)
          slicer->first->setPosition(-con(to_vector(slicer->second))*(abs(to_vector(slicer->second).dotProduct(m.size))/2+2)*(md.wall+md.gap));
    };
    bool hideSide(Dirn side,bool out){
      return md.hideSide(side,out);
    };
};

#endif

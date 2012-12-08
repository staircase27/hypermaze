#include "irrlicht.h"
#include "maze.hh"
#include "string.hh"
#include <map>
#include <vector>
#include <list>
#include "dirns.hh"

using namespace irr;
using namespace core;
using namespace scene;

using namespace std;

class NodeGen{
  public:
    virtual IMeshSceneNode* makeUnitWall(bool isNode)=0;
    virtual IMeshSceneNode* makeUnitString(bool isNode)=0;
    virtual void makeStringActive(IMeshSceneNode* node,bool active)=0;
};


class VisibleCounter{
  int visible;
  IMeshSceneNode* node;
  public:
    VisibleCounter(IMeshSceneNode* node):node(node),visible(0){};
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
  std::map<Dirn,pair<pair<int,int>,pair<Dirn,bool> > > limits;
  std::map<Dirn,vector<vector<VisibleCounter*>*>*> nodes;
  Maze& m;
  
  vector3df center;
  public:
    static const double wall=5;
    static const double gap=20;
  
    void init(NodeGen* ng){
      nodes.clear();
      limits.clear();
      
      limits[UP].first.first=0;
      limits[UP].first.second=0;
      limits[UP].second.first=UP;
      limits[UP].second.second=true;
      limits[LEFT].first.first=0;
      limits[LEFT].first.second=0;
      limits[LEFT].second.first=LEFT;
      limits[LEFT].second.second=true;
      limits[FORWARD].first.first=0;
      limits[FORWARD].first.second=0;
      limits[FORWARD].second.first=FORWARD;
      limits[FORWARD].second.second=true;
      limits[DOWN].first.first=2*(m.size.Y-1);
      limits[DOWN].first.second=2*(m.size.Y-1);
      limits[DOWN].second.first=UP;
      limits[DOWN].second.second=false;
      limits[RIGHT].first.first=2*(m.size.X-1);
      limits[RIGHT].first.second=2*(m.size.X-1);
      limits[RIGHT].second.first=LEFT;
      limits[RIGHT].second.second=false;
      limits[BACK].first.first=2*(m.size.Z-1);
      limits[BACK].first.second=2*(m.size.Z-1);
      limits[BACK].second.first=FORWARD;
      limits[BACK].second.second=false;
      
      nodes[UP]=new vector<vector<VisibleCounter*>*>(2*m.size.Y-1,(vector<VisibleCounter*>*)NULL);
      nodes[LEFT]=new vector<vector<VisibleCounter*>*>(2*m.size.X-1,(vector<VisibleCounter*>*)NULL);
      nodes[FORWARD]=new vector<vector<VisibleCounter*>*>(2*m.size.Z-1,(vector<VisibleCounter*>*)NULL);
      
      vector3df position=center-((wall+gap)*vector3df(m.size.X,m.size.Y,m.size.Z)-gap*vector3df(1,1,1))/2;
      
      int z=0;
      
      for(int x=0;x<m.size.X;++x)
        for(int y=0;y<m.size.Y;++y)
          for(int z=0;z<m.size.Z;++z){
	          IMeshSceneNode* node = ng->makeUnitWall(true);

            node->setScale(vector3df(wall,wall,wall));
            node->setPosition(position+vector3df(x,y,z)*(wall+gap)+vector3df(wall/2,wall/2,wall/2));
            
            VisibleCounter* vc=new VisibleCounter(node);
            if((*nodes[UP])[2*y]==0)
              (*nodes[UP])[2*y]=new vector<VisibleCounter*>();
            (*nodes[UP])[2*y]->push_back(vc);
            if((*nodes[LEFT])[2*x]==0)
              (*nodes[LEFT])[2*x]=new vector<VisibleCounter*>();
            (*nodes[LEFT])[2*x]->push_back(vc);
            if((*nodes[FORWARD])[2*z]==0)
              (*nodes[FORWARD])[2*z]=new vector<VisibleCounter*>();
            (*nodes[FORWARD])[2*z]->push_back(vc);

            if((*m[Vector(x,y,z)]&to_mask(UP))!=0){
  	          IMeshSceneNode* node = ng->makeUnitWall(false);

              node->setScale(vector3df(wall,gap,wall));
              node->setPosition(position+vector3df(x,y,z)*(wall+gap)+vector3df(wall/2,wall+gap/2,wall/2));
              
              VisibleCounter* vc=new VisibleCounter(node);
              if((*nodes[UP])[2*y+1]==0)
                (*nodes[UP])[2*y+1]=new vector<VisibleCounter*>();
              (*nodes[UP])[2*y+1]->push_back(vc);
              if((*nodes[LEFT])[2*x]==0)
                (*nodes[LEFT])[2*x]=new vector<VisibleCounter*>();
              (*nodes[LEFT])[2*x]->push_back(vc);
              if((*nodes[FORWARD])[2*z]==0)
                (*nodes[FORWARD])[2*z]=new vector<VisibleCounter*>();
              (*nodes[FORWARD])[2*z]->push_back(vc);
            }

            if((*m[Vector(x,y,z)]&to_mask(LEFT))!=0){
  	          IMeshSceneNode* node = ng->makeUnitWall(false);

              node->setScale(vector3df(gap,wall,wall));
              node->setPosition(position+vector3df(x,y,z)*(wall+gap)+vector3df(wall+gap/2,wall/2,wall/2));
              
              VisibleCounter* vc=new VisibleCounter(node);
              if((*nodes[UP])[2*y]==0)
                (*nodes[UP])[2*y]=new vector<VisibleCounter*>();
              (*nodes[UP])[2*y]->push_back(vc);
              if((*nodes[LEFT])[2*x+1]==0)
                (*nodes[LEFT])[2*x+1]=new vector<VisibleCounter*>();
              (*nodes[LEFT])[2*x+1]->push_back(vc);
              if((*nodes[FORWARD])[2*z]==0)
                (*nodes[FORWARD])[2*z]=new vector<VisibleCounter*>();
              (*nodes[FORWARD])[2*z]->push_back(vc);
            }
            
            if((*m[Vector(x,y,z)]&to_mask(FORWARD))!=0){
  	          IMeshSceneNode* node = ng->makeUnitWall(false);

              node->setScale(vector3df(wall,wall,gap));
              node->setPosition(position+vector3df(x,y,z)*(wall+gap)+vector3df(wall/2,wall/2,wall+gap/2));
              
              VisibleCounter* vc=new VisibleCounter(node);
              if((*nodes[UP])[2*y]==0)
                (*nodes[UP])[2*y]=new vector<VisibleCounter*>();
              (*nodes[UP])[2*y]->push_back(vc);
              if((*nodes[LEFT])[2*x]==0)
                (*nodes[LEFT])[2*x]=new vector<VisibleCounter*>();
              (*nodes[LEFT])[2*x]->push_back(vc);
              if((*nodes[FORWARD])[2*z+1]==0)
                (*nodes[FORWARD])[2*z+1]=new vector<VisibleCounter*>();
              (*nodes[FORWARD])[2*z+1]->push_back(vc);
            }
          }
    }
    MazeDisplay(Maze& m,NodeGen* ng,vector3df center=vector3df(0,0,0)):m(m),center(center){
      init(ng);
    }
    
    bool hideSide(Dirn side,bool out){
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

vector3df con(Vector v){
  return vector3df(v.X,v.Y,v.Z);
}

vector3df remSgn(vector3df v){
  return vector3df(abs(v.X),abs(v.Y),abs(v.Z));
}

class StringDisplay{
  StringSlice& s;
  std::list<IMeshSceneNode*> nodes;
  int activeNodes;
  NodeGen* ng;
  vector3df center;
  public:
    
    void update(){
      vector3df position=center-(MazeDisplay::wall+MazeDisplay::gap)*vector3df(s.s.maze.size.X,s.s.maze.size.Y,s.s.maze.size.Z)/2;
    
    
      bool active=false;
      int i=0;
      std::list<IMeshSceneNode*>::iterator nit=nodes.begin();
      std::list<Dirn>::iterator sit=s.s.route.begin();
      Vector posn=s.s.start;
      while(sit!=s.s.route.end()){
        if(nit==nodes.end())
          nit=nodes.insert(nit,ng->makeUnitString(true));
        (*nit)->setScale(MazeDisplay::wall*vector3df(1,1,1));
        (*nit)->setPosition(position+vector3df(posn.X,posn.Y,posn.Z)*(MazeDisplay::wall+MazeDisplay::gap));
        if(i>=activeNodes)
          (*nit)->setVisible(true);
        if((!active) && (sit==s.start))
          active=true;
        cout<<active<<endl;
        ng->makeStringActive(*nit,active);
        if(active&& (sit==s.end))
          active=false;
        
        ++nit;
        ++i;
        
        if(nit==nodes.end())
          nit=nodes.insert(nit,ng->makeUnitString(false));
        (*nit)->setScale(MazeDisplay::wall*vector3df(1,1,1)+(MazeDisplay::gap-MazeDisplay::wall)*remSgn(con(to_vector(*sit))));
        (*nit)->setPosition(position+vector3df(posn.X,posn.Y,posn.Z)*(MazeDisplay::wall+MazeDisplay::gap)+(MazeDisplay::gap+MazeDisplay::wall)*con(to_vector(*sit))/2  );
        if(i>=activeNodes)
          (*nit)->setVisible(true);

        if((!active) && (sit==s.start))
          active=true;
        ng->makeStringActive(*nit,active);

        ++nit;
        ++i;
        
        posn=posn+to_vector(*sit);
        ++sit;
        
      }
      if(nit==nodes.end())
        nit=nodes.insert(nit,ng->makeUnitString(true));
      (*nit)->setScale(MazeDisplay::wall*vector3df(1,1,1));
      (*nit)->setPosition(position+vector3df(posn.X,posn.Y,posn.Z)*(MazeDisplay::wall+MazeDisplay::gap));
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
      std::list<IMeshSceneNode*>::iterator nit=nodes.begin();
      std::list<Dirn>::iterator sit=s.s.route.begin();
      while(sit!=s.s.route.end()){
        if((!active) && (sit==s.start))
          active=true;
        ng->makeStringActive(*nit,active);
        if(active&& (sit==s.end))
          active=false;
        ++nit;
        ng->makeStringActive(*nit,active);
        ++nit;
        ++sit;
        
      }
      ng->makeStringActive(*nit,active);
    }
    
    StringDisplay(StringSlice& s,NodeGen* ng,vector3df center=vector3df(0,0,0)):s(s),center(center),ng(ng),activeNodes(0){
      update();
    };

};

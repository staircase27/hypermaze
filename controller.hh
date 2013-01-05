#include "irrlicht.h"
#include "irrdisp.hh"
#include "keymap.hh"

#ifndef CONTROLLER_HH_INC
#define CONTROLLER_HH_INC

namespace irr{
  using namespace core;
  using namespace scene;
  using namespace io;
  using namespace video;
};

using namespace std;

class Controller: public irr::IEventReceiver{
  protected:
    PuzzleDisplay& pd;
  public:
    virtual void run(irr::u32 now)=0;
    virtual bool OnEvent(const irr::SEvent& event)=0;
    Controller(PuzzleDisplay& pd):pd(pd){};
};

class KeyboardController:public Controller{
  // We use this array to store the current state of each key
  bool actionTriggered[KeyMap::A_COUNT];
  irr::u32 actionTime[KeyMap::A_COUNT];
  static const irr::u32 DELAY=500;
  irr::IrrlichtDevice* device;

  public:
    KeyMap map;

    virtual bool OnEvent(const irr::SEvent& event)
    {
      // Remember whether each key is down or up
      if (event.EventType == irr::EET_KEY_INPUT_EVENT && event.KeyInput.PressedDown){
        KeyMap::Action a;
        if(event.KeyInput.Char==0)
          a=map.getAction(event.KeyInput.Key);
        else
          a=map.getAction(event.KeyInput.Char);
        if(a!=KeyMap::A_NONE){
          actionTriggered[a]=true;
          return true;
        }
      }
      return false;
    }

    // This is used to check whether a key is being held down
    virtual bool isTriggered(KeyMap::Action a)
    {
      if(actionTriggered[a]){
        actionTriggered[a]=false;
        return true;
       }
       return false;
    }

    virtual void run(irr::u32 now){
      for(const pair<KeyMap::Action,pair<Dirn,bool> >* it=KeyMap::sliceActions;it!=KeyMap::sliceActions+12;++it){
        if(isTriggered(it->first)&&actionTime[it->first]<now){
          pd.hideSide(it->second.first,it->second.second);
          actionTime[it->first]=now+1*DELAY;
        }
      }
      for(const pair<KeyMap::Action,pair<bool,bool> >* it=KeyMap::slideActions;it!=KeyMap::slideActions+4;++it){
        if(isTriggered(it->first)&&actionTime[it->first]<now){
          if(pd.ss.slide(it->second.first,it->second.second)){
            pd.stringSelectionUpdated();
            actionTime[it->first]=now+1*DELAY;
          }
        }
      }
      for(const pair<KeyMap::Action,Dirn>* it=KeyMap::moveActions;it!=KeyMap::moveActions+6;++it){
        if(isTriggered(it->first)&&actionTime[it->first]<now){
          if(pd.ss.tryMove(it->second)){
            pd.stringUpdated();
            actionTime[it->first]=now+1*DELAY;
          }
        }
      }

      if(isTriggered(KeyMap::A_GENERATE)&&actionTime[KeyMap::A_GENERATE]<now){
        GenerateGui gg;
        if(gg.generate(device,pd.m)){
          pd.mazeUpdated();
        }
        actionTime[KeyMap::A_GENERATE]=now+1*DELAY;
      }
      if(isTriggered(KeyMap::A_LOAD)&&actionTime[KeyMap::A_LOAD]<now){
        OpenGui og;
        if(og.open(device,pd.m)){
          pd.mazeUpdated();
        }
        actionTime[KeyMap::A_GENERATE]=now+1*DELAY;
      }
      if(isTriggered(KeyMap::A_SAVE)&&actionTime[KeyMap::A_SAVE]<now){
        SaveGui sg;
        sg.save(device,pd.m);
        actionTime[KeyMap::A_SAVE]=now+1*DELAY;
      }
      if(isTriggered(KeyMap::A_CONF)&&actionTime[KeyMap::A_CONF]<now){
        HelpGui kmg;
        kmg.help(device,map);
        actionTime[KeyMap::A_CONF]=now+1*DELAY;
      }
    };

    KeyboardController(PuzzleDisplay& pd,irr::IrrlichtDevice *device):Controller(pd),device(device){
      for (irr::u32 i=0; i<KeyMap::A_COUNT; ++i){
        actionTriggered[i] = false;
        actionTime[i] = 0;
      }
    };
};


class MouseSlicerController: public Controller{
  irr::ISceneCollisionManager* collMan;
  irr::ISceneNode* slice;
  int sliced;
  irr::vector3df sliceStart;
  irr::position2d<irr::s32> mousePos;


  public:
  virtual void run(irr::u32 now){
    if(slice){
      irr::line3d<irr::f32> ray=collMan->getRayFromScreenCoordinates(mousePos);
      irr::vector3df dir=con(to_vector(pd.getSlicers().find(slice)->second));
      irr::vector3df ldir=ray.getVector();
      irr::f32 d=(sliceStart-ray.start).dotProduct(dir*ldir.getLengthSQ()-ldir*dir.dotProduct(ldir))/(dir.dotProduct(ldir)*dir.dotProduct(ldir)-dir.getLengthSQ()*ldir.getLengthSQ())/(MazeDisplay::wall+MazeDisplay::gap)*2-sliced;
      while(d>1&&pd.hideSide(pd.getSlicers().find(slice)->second,false)){
        d--;
        sliced++;
      }
      while(d<-1&&pd.hideSide(pd.getSlicers().find(slice)->second,true)){
        d++;
        sliced--;
      }
    }
  };

    virtual bool OnEvent(const irr::SEvent& event)
    {
      if (event.EventType != irr::EET_MOUSE_INPUT_EVENT)
        return false;

      switch(event.MouseInput.Event){
        case irr::EMIE_LMOUSE_PRESSED_DOWN:
          {
            mousePos = irr::position2d<irr::s32> (event.MouseInput.X,event.MouseInput.Y);
            irr::triangle3df tmp;
            slice=collMan-> getSceneNodeAndCollisionPointFromRay(
                collMan->getRayFromScreenCoordinates(mousePos),sliceStart,tmp);
            if(pd.getSlicers().find(slice)!=pd.getSlicers().end()){
              sliced=0;
              return true;
            }else{
              slice=0;
              return false;
            }
          }
        case irr::EMIE_LMOUSE_LEFT_UP:
          if(slice!=0){
            slice=0;
            return true;
          }else
            return false;
        case irr::EMIE_LMOUSE_DOUBLE_CLICK:
          if(slice!=0){
            while(pd.hideSide(pd.getSlicers().find(slice)->second,true)){};
            return true;
          }else
            return false;
        case irr::EMIE_MOUSE_MOVED:
          if(slice!=0){
            mousePos = irr::position2d<irr::s32> (event.MouseInput.X,event.MouseInput.Y);
            return true;
          }
          break;
      }
      return false;
    }

    MouseSlicerController(PuzzleDisplay& pd,irr::IrrlichtDevice *device):Controller(pd),collMan(device->getSceneManager()->getSceneCollisionManager()),slice(0),mousePos(0,0){};
};


class MouseStringDraggerController: public Controller{
  irr::ISceneCollisionManager* collMan;
  irr::ISceneNode* string;
  irr::vector3df startPoint;
  irr::position2d<irr::s32> mousePos;
  int dist;
  Dirn currdir;


  public:
    virtual void run(irr::u32 now){
      if(string!=0){
        irr::line3d<irr::f32> ray=collMan->getRayFromScreenCoordinates(mousePos);
        irr::vector3df ldir=ray.getVector();
        while(true){
          if(dist==0){
            irr::vector3df weight;
            weight=(ldir.dotProduct(startPoint-ray.start)*ldir-(startPoint-ray.start)*ldir.getLengthSQ())/(MazeDisplay::wall+MazeDisplay::gap);
            if(ldir.Y*ldir.Y+ldir.Z*ldir.Z>0.1*ldir.getLengthSQ())
              weight.X/=ldir.Y*ldir.Y+ldir.Z*ldir.Z;
            else
              weight.X=0;
            if(ldir.X*ldir.X+ldir.Y*ldir.Y>0.1*ldir.getLengthSQ())
              weight.Y/=ldir.X*ldir.X+ldir.Z*ldir.Z;
            else
              weight.Y=0;
            if(ldir.X*ldir.X+ldir.Y*ldir.Y>.1*ldir.getLengthSQ())
              weight.Z/=ldir.X*ldir.X+ldir.Y*ldir.Y;
            else
              weight.Z=0;
            Dirn dir;
            irr::f32 largest=0.9999;
            for(Dirn *d=allDirns;d!=allDirns+6;++d){
              if(weight.dotProduct(con(to_vector(*d)))>largest&&pd.ss.canMove(*d)){
                largest=weight.dotProduct(con(to_vector(*d)));
                dir=*d;
              }
            }
            if(largest<1)
              break;
            if(pd.ss.tryMove(dir)){
              pd.stringUpdated();
              currdir=dir;
              dist+=1;
            }else
              break;
          }else{
            irr::f32 weight=con(to_vector(currdir)).dotProduct(
                (ldir.dotProduct(startPoint-ray.start)*ldir-(startPoint-ray.start)*ldir.getLengthSQ())/(MazeDisplay::wall+MazeDisplay::gap))/
                (ldir.getLengthSQ()-ldir.dotProduct(con(to_vector(currdir)))*ldir.dotProduct(con(to_vector(currdir))))-dist;
            if(weight>=1 && pd.ss.tryMove(currdir)){
              pd.stringUpdated();
              dist+=1;
            }else if(weight<=-1 && pd.ss.tryMove(opposite(currdir))){
              pd.stringUpdated();
              dist-=1;
            }else
              break;
          }
        }
      }
    };

    virtual bool OnEvent(const irr::SEvent& event)
    {
      if (event.EventType != irr::EET_MOUSE_INPUT_EVENT)
        return false;

      switch(event.MouseInput.Event){
        case irr::EMIE_LMOUSE_PRESSED_DOWN:
          {
            mousePos = irr::position2d<irr::s32> (event.MouseInput.X,event.MouseInput.Y);
            irr::triangle3df tmp;
            string=collMan-> getSceneNodeAndCollisionPointFromRay(
                collMan->getRayFromScreenCoordinates(mousePos),startPoint,tmp);
            pair<StringPointer,bool> sp=pd.getStringPointer(string);
            bool selected=false;
            if(sp.first!=pd.s.end()){
              selected=sp.first->selected;
            }
            if(sp.second && ! selected){
              if(sp.first!=pd.s.begin()){
                --sp.first;
                selected|=sp.first->selected;
              }
            }
            if(selected){
              dist=0;
              return true;
            }else{
              string=0;
              return false;
            }
          }
        case irr::EMIE_LMOUSE_LEFT_UP:
          if(string!=0){
            string=0;
            return true;
          }else
            return false;
        case irr::EMIE_MOUSE_MOVED:
          if(string!=0){
            mousePos = irr::position2d<irr::s32> (event.MouseInput.X,event.MouseInput.Y);
            return true;
          }
          break;
      }
      return false;
    }

    MouseStringDraggerController(PuzzleDisplay& pd,irr::IrrlichtDevice *device):Controller(pd),collMan(device->getSceneManager()->getSceneCollisionManager()),string(0),mousePos(0,0){};
};


class MouseStringSelectorController: public Controller{
  irr::ISceneCollisionManager* collMan;
  irr::position2d<irr::s32> mousePos;
  irr::vector3df startPoint;
  irr::ISceneNode* string;
  pair<StringPointer,bool> sp;
  bool selected;
  int moved;
  
  public:
  
    virtual void run(irr::u32 now){
      if(string){
        irr::line3d<irr::f32> ray=collMan->getRayFromScreenCoordinates(mousePos);
        irr::vector3df ldir=ray.getVector();
        irr::f32 weight=con(to_vector(LEFT)).dotProduct(
            (ldir.dotProduct(startPoint-ray.start)*ldir-(startPoint-ray.start)*ldir.getLengthSQ())/(MazeDisplay::wall+MazeDisplay::gap))/
            (ldir.getLengthSQ()-ldir.dotProduct(con(to_vector(LEFT)))*ldir.dotProduct(con(to_vector(LEFT))))-moved;
        
        while(weight>1){
          if(sp.first!=pd.s.end()){
            if(moved<0)
              pd.ss.setSelected(sp.first,selected);
            ++sp.first;
            if(moved>=0)
              if(sp.first!=pd.s.end())
                pd.ss.setSelected(sp.first,!selected);
            pd.stringSelectionUpdated();
            ++moved;
            --weight;
          }else{
            break;
          }
        }
        while(weight<-1){
          if(sp.first!=pd.s.begin()){
            if(moved>0)
              if(sp.first!=pd.s.end())
                pd.ss.setSelected(sp.first,selected);
            --sp.first;
            if(moved<=0)
              pd.ss.setSelected(sp.first,!selected);
            pd.stringSelectionUpdated();
            --moved;
            ++weight;
          }else{
            break;
          }
        }
      }
    }
    
    virtual bool OnEvent(const irr::SEvent& event){
      if (event.EventType != irr::EET_MOUSE_INPUT_EVENT)
        return false;

      switch(event.MouseInput.Event){
        case irr::EMIE_LMOUSE_DOUBLE_CLICK:
        case irr::EMIE_RMOUSE_DOUBLE_CLICK:
          {
            mousePos = irr::position2d<irr::s32> (event.MouseInput.X,event.MouseInput.Y);
            irr::triangle3df tmp;
            irr::vector3df tmp2;
            irr::ISceneNode* node=collMan-> getSceneNodeAndCollisionPointFromRay(
                collMan->getRayFromScreenCoordinates(mousePos),tmp2,tmp);
            pair<StringPointer,bool> sp=pd.getStringPointer(node);
            if(sp.second||sp.first!=pd.s.end()){
              bool selected=false;
              if(sp.first!=pd.s.end()){
                selected=sp.first->selected;
              }
              if(sp.second && ! selected){
                if(sp.first!=pd.s.begin()){
                  --sp.first;
                  selected|=sp.first->selected;
                }
              }
              selected=!selected;
              for(StringPointer p=pd.s.begin();p!=pd.s.end();++p)
                pd.ss.setSelected(p,selected);
              pd.stringSelectionUpdated();
              return true;
            }
            break;
          }
        case irr::EMIE_RMOUSE_PRESSED_DOWN:
          {
            mousePos = irr::position2d<irr::s32> (event.MouseInput.X,event.MouseInput.Y);
            irr::triangle3df tmp;
            irr::ISceneNode* node=collMan-> getSceneNodeAndCollisionPointFromRay(
                collMan->getRayFromScreenCoordinates(mousePos),startPoint,tmp);
            pair<StringPointer,bool> sp=pd.getStringPointer(node);
            if(sp.second||sp.first!=pd.s.end()){
              bool selected=false;
              if(sp.first!=pd.s.end()){
                selected=sp.first->selected;
              }
              if(sp.second && ! selected){
                if(sp.first!=pd.s.begin()){
                  --sp.first;
                  selected|=sp.first->selected;
                  ++sp.first;
                }
              }
              if(sp.first!=pd.s.end())
                pd.ss.setSelected(sp.first,!selected);
              if(sp.second && sp.first!=pd.s.begin()){
                --sp.first;
                pd.ss.setSelected(sp.first,!selected);
              }
              pd.stringSelectionUpdated();
              string=node;
              moved=0;
              this->sp=sp;
              this->selected=selected;
              return true;
            }
            break;
          }
        case irr::EMIE_RMOUSE_LEFT_UP:
          if(string!=0){
            string=0;
            return true;
          }
          break;
        case irr::EMIE_MOUSE_MOVED:
          if(string!=0){
            mousePos = irr::position2d<irr::s32> (event.MouseInput.X,event.MouseInput.Y);
            return true;
          }
          break;
      }
      return false;
    }
  
    MouseStringSelectorController(PuzzleDisplay& pd,irr::IrrlichtDevice *device):Controller(pd),collMan(device->getSceneManager()->getSceneCollisionManager()),sp(pd.s.end(),false){};
};
  

class MultiInterfaceController:public Controller{
  public:
    KeyboardController kc;
    MouseSlicerController msc;
    MouseStringDraggerController mdc;
    MouseStringSelectorController mssc;

    MultiInterfaceController(PuzzleDisplay& pd,irr::IrrlichtDevice *device):Controller(pd),kc(pd,device),msc(pd,device),mdc(pd,device),mssc(pd,device){};

    virtual bool OnEvent(const irr::SEvent& event){
      return kc.OnEvent(event)||msc.OnEvent(event)||mdc.OnEvent(event)||mssc.OnEvent(event);
    }

    virtual void run(irr::u32 now){
      kc.run(now);
      msc.run(now);
      mdc.run(now);
      mssc.run(now);
    }

};


#endif

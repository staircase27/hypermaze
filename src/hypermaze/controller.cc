#include "controller.hh"
#include "irrdisp.hh"
#include "gui.hh"
#include "helpgui.hh"

    bool KeyboardController::OnEvent(const irr::SEvent& event)
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
    bool KeyboardController::isTriggered(KeyMap::Action a)
    {
      if(actionTriggered[a]){
        actionTriggered[a]=false;
        return true;
       }
       return false;
    }

    void KeyboardController::run(irr::u32 now){
      for(const pair<KeyMap::Action,pair<Dirn,bool> >* it=KeyMap::sliceActions;it!=KeyMap::sliceActions+12;++it){
        if(isTriggered(it->first)&&actionTime[it->first]<now){
          pd.hideSide(it->second.first,it->second.second);
          actionTime[it->first]=now+1*DELAY;  
        }
      }
      for(const pair<KeyMap::Action,pair<bool,bool> >* it=KeyMap::slideActions;it!=KeyMap::slideActions+4;++it){
        if(isTriggered(it->first)&&actionTime[it->first]<now){
          if(pd.sp.slide(it->second.first,it->second.second)){
            pd.stringSelectionUpdated();
            actionTime[it->first]=now+1*DELAY;
          }
        }
      }
      for(const pair<KeyMap::Action,Dirn>* it=KeyMap::moveActions;it!=KeyMap::moveActions+6;++it){
        if(isTriggered(it->first)&&actionTime[it->first]<now){
          if(pd.sp.tryMove(it->second)){
            pd.stringUpdated();
            actionTime[it->first]=now+1*DELAY;
          }else
            sm->playEffect(SoundManager::SE_BLOCK);
        }
      }
      
      if(isTriggered(KeyMap::A_UNDO)&&actionTime[KeyMap::A_UNDO]<now){
        if(pd.sp.undo()){
          pd.stringUpdated();
          actionTime[KeyMap::A_UNDO]=now+1*DELAY;
        }else
          sm->playEffect(SoundManager::SE_BLOCK);
      }

      if(isTriggered(KeyMap::A_GENERATE)&&actionTime[KeyMap::A_GENERATE]<now){
        GenerateGui gg;
        if(gg.generate(device,fm,pd)){
          pd.mazeUpdated();
        }
        actionTime[KeyMap::A_GENERATE]=now+1*DELAY;
      }
      if(isTriggered(KeyMap::A_LOAD)&&actionTime[KeyMap::A_LOAD]<now){
        OpenGui og;
        if(og.open(device,fm,pd)){
          pd.mazeUpdated();
        }
        actionTime[KeyMap::A_GENERATE]=now+1*DELAY;
      }
      if(isTriggered(KeyMap::A_SAVE)&&actionTime[KeyMap::A_SAVE]<now){
        SaveGui sg;
        sg.save(device,fm,pd);
        actionTime[KeyMap::A_SAVE]=now+1*DELAY;
      }
      if(isTriggered(KeyMap::A_CONF)&&actionTime[KeyMap::A_CONF]<now){
        HelpGui kmg;
        kmg.help(device,fm,map);
        actionTime[KeyMap::A_CONF]=now+1*DELAY;
      }
    };
    
  void MouseSlicerController::run(irr::u32 now){
    if(!slice.isnull()){
      irr::line3d<irr::f32> ray=collMan->getRayFromScreenCoordinates(mousePos);
      irr::vector3df dir=con(to_vector(*slice));
      irr::vector3df ldir=ray.getVector();
      irr::f32 d=(sliceStart-ray.start).dotProduct(dir*ldir.getLengthSQ()-ldir*dir.dotProduct(ldir))/(dir.dotProduct(ldir)*dir.dotProduct(ldir)-dir.getLengthSQ()*ldir.getLengthSQ())/(MazeDisplay::wall+MazeDisplay::gap)*2-sliced;
      while(d>1&&pd.hideSide(*slice,false)){
        d--;
        sliced++;
      }
      while(d<-1&&pd.hideSide(*slice,true)){
        d++;
        sliced--;
      }
    }
  };

    bool MouseSlicerController::OnEvent(const irr::SEvent& event)
    {
      if (event.EventType != irr::EET_MOUSE_INPUT_EVENT)
        return false;

      switch(event.MouseInput.Event){
        case irr::EMIE_LMOUSE_PRESSED_DOWN:
          {
            mousePos = irr::position2d<irr::s32> (event.MouseInput.X,event.MouseInput.Y);
            irr::triangle3df tmp;
            irr::ISceneNode* slicenode=collMan-> getSceneNodeAndCollisionPointFromRay(
                collMan->getRayFromScreenCoordinates(mousePos),sliceStart,tmp);
            slice=pd.getSlicerDirn(slicenode);
            if(!slice.isnull()){
              sliced=0;
              return true;
            }else{
              return false;
            }
          }
        case irr::EMIE_LMOUSE_LEFT_UP:
          if(!slice.isnull()){
            slice=SP<Dirn>();
            return true;
          }else
            return false;
        case irr::EMIE_LMOUSE_DOUBLE_CLICK:
          if(!slice.isnull()){
            while(pd.hideSide(*slice,true)){};
            return true;
          }else
            return false;
        case irr::EMIE_MOUSE_MOVED:
          if(!slice.isnull()){
            mousePos = irr::position2d<irr::s32> (event.MouseInput.X,event.MouseInput.Y);
            return true;
          }
          break;
      }
      return false;
    }

    void MouseStringDraggerController::run(irr::u32 now){
      if(string!=0){
        irr::line3d<irr::f32> ray=collMan->getRayFromScreenCoordinates(mousePos);
        irr::vector3df ldir=ray.getVector();
        while(true){
          if(dist==0){
            /*
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
            irr::f32 reallargest=0.9999;
            for(Dirn *d=allDirns;d!=allDirns+6;++d){
              if(weight.dotProduct(con(to_vector(*d)))>largest){
                if(weight.dotProduct(con(to_vector(*d)))>reallargest)
                  reallargest=weight.dotProduct(con(to_vector(*d)));
                if(pd.sp.canMove(*d)){
                  largest=weight.dotProduct(con(to_vector(*d)));
                  dir=*d;
                }
              }
            }
            if(largest<1){
              if(reallargest>=1.5)
                sm->playEffect(SoundManager::SE_BLOCK);
              break;
            }
            if(pd.sp.tryMove(dir)){
              pd.stringUpdated();
              currdir=dir;
              dist+=1;
            }else
              break;
            /*/
            Dirn dir;
            irr::f32 smallest=10000000;
            irr::f32 realsmallest=10000000;
            for(Dirn *d=allDirns;d!=allDirns+6;++d){
              irr::f32 steps=
                  (ldir.getLengthSQ()*con(to_vector(*d)).dotProduct(startPoint-ray.start)-ldir.dotProduct(con(to_vector(*d)))*ldir.dotProduct(startPoint-ray.start))/
                  (ldir.dotProduct(con(to_vector(*d)))*ldir.dotProduct(con(to_vector(*d)))-ldir.getLengthSQ())/(MazeDisplay::wall+MazeDisplay::gap);
//              cout<<"testing "<<*d<<" "<<steps<<endl;
              if(steps<=0.8)
                  continue;
              irr::f32 screendist=mousePos.getDistanceFromSQ(collMan->getScreenCoordinatesFrom3DPosition(startPoint+steps*con(to_vector(*d))));
//              cout<<"dist "<<*d<<" "<<screendist<<endl;
              if(screendist<smallest){
                if(screendist<realsmallest)
                  realsmallest=screendist;
                if(pd.sp.canMove(*d)){
                  smallest=screendist;
                  dir=*d;
                }
              }
            }
//            cout<<"best is "<<dir<<" "<<smallest<<endl;
//            cout<<"real best is "<<dir<<" "<<smallest<<endl;
            if(smallest>=10000000){
              if(realsmallest<10000000)
                sm->playEffect(SoundManager::SE_BLOCK);
              break;
            }
            if(pd.sp.tryMove(dir)){
              pd.stringUpdated();
              currdir=dir;
              dist+=1;
            }else
              break;
            //*/
          }else{
            irr::f32 weight=con(to_vector(currdir)).dotProduct(
                (ldir.dotProduct(startPoint-ray.start)*ldir-(startPoint-ray.start)*ldir.getLengthSQ())/(MazeDisplay::wall+MazeDisplay::gap))/
                (ldir.getLengthSQ()-ldir.dotProduct(con(to_vector(currdir)))*ldir.dotProduct(con(to_vector(currdir))))-dist;
            if(weight>=1 && pd.sp.tryMove(currdir)){
              pd.stringUpdated();
              dist+=1;
            }else if(weight<=-1 && pd.sp.tryMove(opposite(currdir))){
              pd.stringUpdated();
              dist-=1;
            }else{
              if(weight>=1.1 || weight<=-1.1)
                sm->playEffect(SoundManager::SE_BLOCK);
              break;
            }
          }
        }
      }
    };

    bool MouseStringDraggerController::OnEvent(const irr::SEvent& event)
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

    void MouseStringDraggerController2::run(irr::u32 now){
      if(string!=0){
        irr::line3d<irr::f32> ray=collMan->getRayFromScreenCoordinates(mousePos);
        irr::vector3df ldir=ray.getVector();
        if(dist==0){
          alloweddirns=0;
          for(Dirn *d=allDirns;d!=allDirns+6;++d)
            if(pd.sp.canMove(*d))
              alloweddirns|=to_mask(*d);
          pd.sp.startExtendedMove();
        }
        Dirn dir;
        int beststeps=0;
        irr::f32 smallest=10000000;
        irr::f32 realsmallest=10000000;
        for(Dirn *d=allDirns;d!=allDirns+6;++d){
          irr::f32 steps=
              (ldir.getLengthSQ()*con(to_vector(*d)).dotProduct(startPoint-ray.start)-ldir.dotProduct(con(to_vector(*d)))*ldir.dotProduct(startPoint-ray.start))/
              (ldir.dotProduct(con(to_vector(*d)))*ldir.dotProduct(con(to_vector(*d)))-ldir.getLengthSQ())/(MazeDisplay::wall+MazeDisplay::gap);
          if(steps<=0.8)
              continue;
          irr::f32 screendist=mousePos.getDistanceFromSQ(collMan->getScreenCoordinatesFrom3DPosition(startPoint+steps*con(to_vector(*d))));
          if(screendist<smallest){
            if(screendist<realsmallest)
              realsmallest=screendist;
            if((alloweddirns&to_mask(*d))!=0){
              beststeps=(int)(steps+0.2);
              smallest=screendist;
              dir=*d;
            }
          }
        }
        if(dist==0 || currdir==dir){
          if(dist<beststeps){
            if(pd.sp.tryMove(dir,true)){
              pd.stringUpdated();
              currdir=dir;
              dist+=1;
            }else{
              sm->playEffect(SoundManager::SE_BLOCK);
            }
          }else if(dist>beststeps){
            if(pd.sp.undo(true)){
              pd.stringUpdated();
              currdir=dir;
              dist-=1;
            }else{
              sm->playEffect(SoundManager::SE_BLOCK);
            }
          }
        }else{
          if(pd.sp.undo(true)){
            pd.stringUpdated();
            dist-=1;
          }else{
            sm->playEffect(SoundManager::SE_BLOCK);
          }
        }
      }
    };

    bool MouseStringDraggerController2::OnEvent(const irr::SEvent& event)
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
    
    void MouseStringSelectorController::run(irr::u32 now){
      if(string){
        irr::line3d<irr::f32> ray=collMan->getRayFromScreenCoordinates(mousePos);
        irr::vector3df ldir=ray.getVector();
        irr::f32 weight=con(to_vector(LEFT)).dotProduct(
            (ldir.dotProduct(startPoint-ray.start)*ldir-(startPoint-ray.start)*ldir.getLengthSQ())/(MazeDisplay::wall+MazeDisplay::gap))/
            (ldir.getLengthSQ()-ldir.dotProduct(con(to_vector(LEFT)))*ldir.dotProduct(con(to_vector(LEFT))))-moved;
        
        while(weight>1){
          if(sp.first!=pd.s.end()){
            if(moved<0)
              pd.sp.setSelected(sp.first,selected);
            ++sp.first;
            if(moved>=0)
              if(sp.first!=pd.s.end())
                pd.sp.setSelected(sp.first,!selected);
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
                pd.sp.setSelected(sp.first,selected);
            --sp.first;
            if(moved<=0)
              pd.sp.setSelected(sp.first,!selected);
            pd.stringSelectionUpdated();
            --moved;
            ++weight;
          }else{
            break;
          }
        }
      }
    }
    
    bool MouseStringSelectorController::OnEvent(const irr::SEvent& event){
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
                pd.sp.setSelected(p,selected);
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
                pd.sp.setSelected(sp.first,!selected);
              if(sp.second && sp.first!=pd.s.begin()){
                --sp.first;
                pd.sp.setSelected(sp.first,!selected);
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
  

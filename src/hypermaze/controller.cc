/**
 * @file controller.cc
 * @brief The implementation of controller.hh
 */
#include "guis.hh"
#include "helpgui.hh"
#include "controller.hh"
#include "controllerimp.hh"
#include "irrdisp.hh"
namespace irr{
  using namespace core;
  using namespace scene;
}

    bool KeyboardController::OnEvent(const irr::SEvent& event)
    {
      if (event.EventType == irr::EET_KEY_INPUT_EVENT && event.KeyInput.PressedDown){
        KeyMap::Action a;
        a=map.getTriggeredAction(KeySpec(event.KeyInput.Key,event.KeyInput.Shift,event.KeyInput.Control));
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
      for(const std::pair<KeyMap::Action,std::pair<Dirn,bool> >* it=KeyMap::sliceActions;it!=KeyMap::sliceActions+12;++it){
        if(isTriggered(it->first)&&actionTime[it->first]<now){
          pd.hideSide(it->second.first,it->second.second);
          actionTime[it->first]=now+1*DELAY;
        }
      }
      for(const std::pair<KeyMap::Action,std::pair<bool,bool> >* it=KeyMap::slideActions;it!=KeyMap::slideActions+4;++it){
        if(isTriggered(it->first)&&actionTime[it->first]<now){
          if(pd.sp.slide(it->second.first,it->second.second)){
            pd.stringSelectionUpdated(base);
            actionTime[it->first]=now+1*DELAY;
          }
        }
      }
      for(const std::pair<KeyMap::Action,Dirn>* it=KeyMap::moveActions;it!=KeyMap::moveActions+6;++it){
        if(isTriggered(it->first)&&actionTime[it->first]<now){
          if(pd.sp.tryMove(it->second)){
            pd.stringUpdated(base);
            actionTime[it->first]=now+1*DELAY;
          }else
            sm->playEffect(SoundManager::SE_BLOCK);
        }
      }

      if(isTriggered(KeyMap::A_UNDO)&&actionTime[KeyMap::A_UNDO]<now){
        if(pd.sp.undo()){
          pd.stringUpdated(base);
          actionTime[KeyMap::A_UNDO]=now+1*DELAY;
        }else
          sm->playEffect(SoundManager::SE_BLOCK);
      }

      if(isTriggered(KeyMap::A_GENERATE)&&actionTime[KeyMap::A_GENERATE]<now){
        base->showGUI(false);
        GenerateGui gg;
        if(gg.generate(device,fm,pd)){
          pd.mazeUpdated(base);
        }
        base->showGUI(true);
        actionTime[KeyMap::A_GENERATE]=now+1*DELAY;
      }
      if(isTriggered(KeyMap::A_LOAD)&&actionTime[KeyMap::A_LOAD]<now){
        base->showGUI(false);
        OpenGui og;
        if(og.open(device,fm,pd)){
          pd.mazeUpdated(base);
        }
        base->showGUI(true);
        actionTime[KeyMap::A_LOAD]=now+1*DELAY;
      }
      if(isTriggered(KeyMap::A_SAVE)&&actionTime[KeyMap::A_SAVE]<now){
        base->showGUI(false);
        SaveGui sg;
        sg.save(device,fm,pd);
        base->showGUI(true);
        actionTime[KeyMap::A_SAVE]=now+1*DELAY;
      }
      if(isTriggered(KeyMap::A_CONF)&&actionTime[KeyMap::A_CONF]<now){
        base->showGUI(false);
        HelpGui kmg;
        kmg.help(device,fm,map);
        base->showGUI(true);
        actionTime[KeyMap::A_CONF]=now+1*DELAY;
      }
    };

    void HUDController::run(irr::u32 now){
      if(undoclicked&&undotime<now){
        undoclicked=false;
        if(pd.sp.undo()){
          pd.stringUpdated(base);
          undotime=now+1*DELAY;
        }else
          sm->playEffect(SoundManager::SE_BLOCK);
      }

      if(generateclicked&&generatetime<now){
        generateclicked=false;
        base->showGUI(false);
        GenerateGui gg;
        if(gg.generate(device,fm,pd)){
          pd.mazeUpdated(base);
        }
        base->showGUI(true);
        generatetime=now+1*DELAY;
      }
      if(loadclicked&&loadtime<now){
        loadclicked=false;
        base->showGUI(false);
        OpenGui og;
        if(og.open(device,fm,pd)){
          pd.mazeUpdated(base);
        }
        base->showGUI(true);
        loadtime=now+1*DELAY;
      }
      if(saveclicked&&savetime<now){
        saveclicked=false;
        base->showGUI(false);
        SaveGui sg;
        sg.save(device,fm,pd);
        base->showGUI(true);
        savetime=now+1*DELAY;
      }
      if(helpclicked&&helptime<now){
        helpclicked=false;
        base->showGUI(false);
        HelpGui kmg;
        kmg.help(device,fm,base->getKeyMap());
        base->showGUI(true);
        helptime=now+1*DELAY;
      }
      scoretext->setText((irr::stringw(L"Score: ")+=pd.sp.getScore()).c_str());
      irr::core::dimension2d<irr::u32> dim=device->getVideoDriver()->getScreenSize();

      if(dim!=lastdim){
        if(dim.Height!=0&&dim.Width!=0){
          irr::rect<irr::s32> rect(irr::position2di(0,0),dim);
          rect.LowerRightCorner.X-=10;
          rect.LowerRightCorner.Y-=10;
          rect.UpperLeftCorner=rect.LowerRightCorner;
          rect.UpperLeftCorner.Y-=32;
          rect.UpperLeftCorner.X-=55;
          undobutton->setRelativePosition(rect);
          rect.UpperLeftCorner.X=10;
          rect.LowerRightCorner.X=100;
          scoretext->setRelativePosition(rect);
          lastdim=dim;
        }
      }
    };

    bool HUDController::OnEvent(const irr::SEvent& event){
      if(event.EventType==irr::EET_GUI_EVENT){
        if(event.GUIEvent.EventType==irr::gui::EGET_BUTTON_CLICKED){
          if(event.GUIEvent.Caller->getID()==ID_UNDO){
            undoclicked=true;
            return true;
          }
          if(event.GUIEvent.Caller->getID()==ID_GENERATE){
            generateclicked=true;
            return true;
          }
          if(event.GUIEvent.Caller->getID()==ID_OPEN){
            loadclicked=true;
            return true;
          }
          if(event.GUIEvent.Caller->getID()==ID_SAVE){
            saveclicked=true;
            return true;
          }
          if(event.GUIEvent.Caller->getID()==ID_HELP){
            helpclicked=true;
            return true;
          }
        }
      }
      return false;
    }

    void HUDController::showGUI(bool show){
      undobutton->setEnabled(show);
      helpbutton->setEnabled(show);
      generatebutton->setEnabled(show);
      loadbutton->setEnabled(show);
      savebutton->setEnabled(show);
      undobutton->setVisible(show);
      helpbutton->setVisible(show);
      generatebutton->setVisible(show);
      loadbutton->setVisible(show);
      savebutton->setVisible(show);
    }
    HUDController::HUDController(PuzzleDisplay& pd,irr::IrrlichtDevice *device,FontManager* fm,SoundManager* sm,MultiInterfaceController* base):Controller(pd),
        undoclicked(false),helpclicked(false),generateclicked(false),loadclicked(false),saveclicked(false),
        undotime(0),helptime(0),generatetime(0),loadtime(0),savetime(0),device(device),fm(fm),sm(sm),base(base){
      irr::gui::IGUIEnvironment* guienv = device->getGUIEnvironment();
      scoretext=guienv->addStaticText(L"",irr::core::rect<irr::s32>(0,0,0,0),false,false);
      undobutton=guienv->addButton(irr::core::rect<irr::s32>(0,0,0,0),0,ID_UNDO,L"Undo",L"Undo last move");
      helpbutton=guienv->addButton(irr::core::rect<irr::s32>(10,10,55,42),0,ID_HELP,L"Help",L"Help and about the puzzle");
      generatebutton=guienv->addButton(irr::core::rect<irr::s32>(10+55,10,55+10+70,42),0,ID_GENERATE,L"Generate",L"Generate new puzzle");
      loadbutton=guienv->addButton(irr::core::rect<irr::s32>(55+10+70+10,10,55+10+70+10+45,42),0,ID_OPEN,L"Open",L"Open a saved puzzle");
      savebutton=guienv->addButton(irr::core::rect<irr::s32>(55+10+70+10+45+10,10,55+10+70+10+45+55,42),0,ID_SAVE,L"Save",L"Save this puzzle");
      irr::video::ITexture* tex=device->getVideoDriver()->getTexture("irrlicht/hudtexture.png");
      undobutton->setImage(tex);
      helpbutton->setImage(tex);
      generatebutton->setImage(tex);
      loadbutton->setImage(tex);
      savebutton->setImage(tex);
      undobutton->setDrawBorder(false);
      helpbutton->setDrawBorder(false);
      generatebutton->setDrawBorder(false);
      loadbutton->setDrawBorder(false);
      savebutton->setDrawBorder(false);
      undobutton->setScaleImage(true);
      helpbutton->setScaleImage(true);
      generatebutton->setScaleImage(true);
      loadbutton->setScaleImage(true);
      savebutton->setScaleImage(true);
      undobutton->setUseAlphaChannel(true);
      helpbutton->setUseAlphaChannel(true);
      generatebutton->setUseAlphaChannel(true);
      loadbutton->setUseAlphaChannel(true);
      savebutton->setUseAlphaChannel(true);
    };


  void MouseSlicerController::run(irr::u32 now){
    if(!slice.isnull()){
      irr::line3d<irr::f32> ray=collMan->getRayFromScreenCoordinates(mousePos);
      irr::vector3df dir=con(to_vector(*slice));
      irr::vector3df ldir=ray.getVector();
      double d=(sliceStart-ray.start).dotProduct(dir*ldir.getLengthSQ()-ldir*dir.dotProduct(ldir))/(dir.dotProduct(ldir)*dir.dotProduct(ldir)-dir.getLengthSQ()*ldir.getLengthSQ())/(WALL_SIZE+GAP_SIZE)*2-sliced;
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
        if(dist==0){
          alloweddirns=0;
          for(Dirn *d=allDirns;d!=allDirns+6;++d)
            if(pd.sp.canMove(*d))
              alloweddirns|=to_mask(*d);
          pd.sp.startExtendedMove();
        }
        Dirn dir=FORWARD;
        int beststeps=0;
        double smallest=10000000;
        double realsmallest=10000000;
        for(Dirn *d=allDirns;d!=allDirns+6;++d){
          double steps=
              (ldir.getLengthSQ()*con(to_vector(*d)).dotProduct(startPoint-ray.start)-ldir.dotProduct(con(to_vector(*d)))*ldir.dotProduct(startPoint-ray.start))/
              (ldir.dotProduct(con(to_vector(*d)))*ldir.dotProduct(con(to_vector(*d)))-ldir.getLengthSQ())/(WALL_SIZE+GAP_SIZE);
          if(steps<=0.8)
              continue;
          double screendist=mousePos.getDistanceFromSQ(collMan->getScreenCoordinatesFrom3DPosition(startPoint+steps*con(to_vector(*d))));
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
              pd.stringUpdated(base);
              currdir=dir;
              dist+=1;
            }else{
              sm->playEffect(SoundManager::SE_BLOCK);
            }
          }else if(dist>beststeps){
            if(pd.sp.undo(true)){
              pd.stringUpdated(base);
              currdir=dir;
              dist-=1;
            }else{
              sm->playEffect(SoundManager::SE_BLOCK);
            }
          }
        }else{
          if(pd.sp.undo(true)){
            pd.stringUpdated(base);
            dist-=1;
          }else{
            sm->playEffect(SoundManager::SE_BLOCK);
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
            std::pair<StringPointer,bool> sp=pd.getStringPointer(string);
            bool selected=false;
            if(sp.first!=pd.s->end()){
              selected=sp.first->selected;
            }
            if(sp.second && ! selected){
              if(sp.first!=pd.s->begin()){
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
          break;
        case irr::EMIE_LMOUSE_LEFT_UP:
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

    void MouseStringSelectorController::run(irr::u32 now){
      if(string){
        irr::line3d<irr::f32> ray=collMan->getRayFromScreenCoordinates(mousePos);
        irr::vector3df ldir=ray.getVector();
        double weight=con(to_vector(LEFT)).dotProduct(
            (ldir.dotProduct(startPoint-ray.start)*ldir-(startPoint-ray.start)*ldir.getLengthSQ())/(WALL_SIZE+GAP_SIZE))/
            (ldir.getLengthSQ()-ldir.dotProduct(con(to_vector(LEFT)))*ldir.dotProduct(con(to_vector(LEFT))))-moved;

        while(weight>1){
          if(sp.first!=pd.s->end()){
            if(moved<0)
              pd.sp.setSelected(sp.first,selected);
            ++sp.first;
            if(moved>=0)
              if(sp.first!=pd.s->end())
                pd.sp.setSelected(sp.first,!selected);
            notifyselectionchanged=true;
            ++moved;
            --weight;
          }else{
            break;
          }
        }
        while(weight<-1){
          if(sp.first!=pd.s->begin()){
            if(moved>0)
              if(sp.first!=pd.s->end())
                pd.sp.setSelected(sp.first,selected);
            --sp.first;
            if(moved<=0)
              pd.sp.setSelected(sp.first,!selected);
            notifyselectionchanged=true;
            --moved;
            ++weight;
          }else{
            break;
          }
        }
      }
      if(notifyselectionchanged){
        notifyselectionchanged=false;
        pd.stringSelectionUpdated(base);
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
            std::pair<StringPointer,bool> sp=pd.getStringPointer(node);
            if(sp.second||sp.first!=pd.s->end()){
              bool selected=false;
              if(sp.first!=pd.s->end()){
                selected=sp.first->selected;
              }
              if(sp.second && ! selected){
                if(sp.first!=pd.s->begin()){
                  --sp.first;
                  selected|=sp.first->selected;
                }
              }
              selected=!selected;
              for(StringPointer p=pd.s->begin();p!=pd.s->end();++p)
                pd.sp.setSelected(p,selected);
              notifyselectionchanged=true;
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
            std::pair<StringPointer,bool> sp=pd.getStringPointer(node);
            if(sp.second||sp.first!=pd.s->end()){
              bool selected=false;
              if(sp.first!=pd.s->end()){
                selected=sp.first->selected;
              }
              if(sp.second && ! selected){
                if(sp.first!=pd.s->begin()){
                  --sp.first;
                  selected|=sp.first->selected;
                  ++sp.first;
                }
              }
              if(sp.first!=pd.s->end())
                pd.sp.setSelected(sp.first,!selected);
              if(sp.second && sp.first!=pd.s->begin()){
                --sp.first;
                pd.sp.setSelected(sp.first,!selected);
              }
              string=node;
              moved=0;
              this->sp=sp;
              this->selected=selected;
              notifyselectionchanged=true;
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

MultiInterfaceController* createController(PuzzleDisplay& pd,irr::IrrlichtDevice *device,FontManager* fm,SoundManager* sm){
  return new HypermazeMultiInterfaceController(pd,device,fm,sm);
}


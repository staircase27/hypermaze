#include "irrlicht.h"
#include "gui.hh"
#include "mazegen.hh"
#include "vector.hh"
#include "iMyCamera.hh"
#include "dirns.hh"

using namespace irr;
using namespace core;
using namespace scene;
using namespace io;
using namespace video;

class MyEventReceiver : public IEventReceiver
{

private:
        // We use this array to store the current state of each key
        bool KeyIsDown[KEY_KEY_CODES_COUNT];
public:
        bool MouseDown;
        position2d<s32> MousePos;
    // This is the one method that we have to implement
    virtual bool OnEvent(const SEvent& event)
    {
        // Remember whether each key is down or up
        if (event.EventType == irr::EET_KEY_INPUT_EVENT)
                KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;

    if (event.EventType != EET_MOUSE_INPUT_EVENT)
        return false;

    switch(event.MouseInput.Event)
    {
    case EMIE_LMOUSE_PRESSED_DOWN:
        MouseDown = true;
        break;
    case EMIE_LMOUSE_LEFT_UP:
        MouseDown = false;
        break;
    case EMIE_MOUSE_MOVED:
        MousePos = position2d<s32> (event.MouseInput.X,event.MouseInput.Y);
        break;
    }
                return false;
        }

        // This is used to check whether a key is being held down
        virtual bool IsKeyDown(EKEY_CODE keyCode) const
        {
                return KeyIsDown[keyCode];
        }
        
        MyEventReceiver():MouseDown(false),MousePos(0,0)
        {
                for (u32 i=0; i<KEY_KEY_CODES_COUNT; ++i)
                        KeyIsDown[i] = false;
        }

};

class MyNodeGen:public NodeGen{
  ISceneManager* smgr;
  ITexture* wall;
  ITexture* string;
  ITexture* activeString;
  public:
    MyNodeGen(ISceneManager* smgr,ITexture* wall,ITexture* string,ITexture* activeString):smgr(smgr),wall(wall),string(string),activeString(activeString){};
    virtual IMeshSceneNode* makeUnitWall(bool isNode){
      IMeshSceneNode* node = smgr->addCubeSceneNode(1);
      node->setMaterialTexture( 0, wall);
      node->setMaterialFlag(video::EMF_LIGHTING, true);
      return node;
    }

    virtual IMeshSceneNode* makeUnitString(bool isNode){
      IMeshSceneNode* node = smgr->addCubeSceneNode(1);
      node->setMaterialTexture( 0, string);
      node->setMaterialFlag(video::EMF_LIGHTING, true);
      return node;
    }

    virtual void makeStringActive(IMeshSceneNode* node,bool active){
      if(active)
        node->setMaterialTexture(0,activeString);
      else
        node->setMaterialTexture(0,string);
    }

};


int main(){

  MyEventReceiver e;
  
	IrrlichtDevice *device =
		createDevice( video::EDT_OPENGL, dimension2d<u32>(640, 480), 16,
			false, false, false, &e);

	if (!device)
		return 1;
		
	
	device->setWindowCaption(L"Hyper Maze! - Irrlicht");
	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->getSceneManager();
	
	addCameraSceneNodeMy(smgr,device->getCursorControl(),0,vector3df(0,0,0),
	    vector3df(0,0,-300),-500.,-100.,3.,100.);
	
  ILightSceneNode* light1 =
                smgr->addLightSceneNode(0, core::vector3df(-100,100,-100),
                video::SColorf(1.0f,1.0f,1.0f,1.0f), 800.0f);
  smgr->setAmbientLight(video::SColorf(0.3,0.3,0.3,1));

  
                
  Maze m=generate(Vector(5,5,5));
  String s(m);
  StringSlice ss(s);
  
  
  MyNodeGen* ng=new MyNodeGen(smgr,driver->getTexture("irrlicht/wall.png"),driver->getTexture("irrlicht/string.png"),driver->getTexture("irrlicht/activeString.png"));
  cout<<"display"<<endl;
  MazeDisplay md(m,ng);
  StringDisplay sd(ss,ng);

  std::map<ISceneNode*,Dirn> slicers;
  
  f32 r=vector3df(m.size.X,m.size.Y,m.size.Z).getLength()/2;
  
  for(Dirn* d=allDirns;d!=allDirns+6;++d){
    IMeshSceneNode* node = smgr->addSphereSceneNode(r*(md.wall+md.gap)/15);
    node->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
    node->setMaterialTexture( 0, driver->getTexture("irrlicht/string.png"));
    node->setMaterialFlag(video::EMF_LIGHTING, true);
    node->setPosition(-con(to_vector(*d))*r*(md.wall+md.gap));
    ITriangleSelector* selector = smgr->createTriangleSelector(node->getMesh(),node);
    node->setTriangleSelector(selector);
    selector->drop(); // We're done with this selector, so drop it now.
    slicers[node]=*d;
  }

  ISceneCollisionManager* collMan = smgr->getSceneCollisionManager();
  ISceneNode* slice=0;
  int sliced=0;
  vector3df sliceStart;
  
  u32 then = device->getTimer()->getTime();

  double delay=-1;
	while(device->run())
	{
    const u32 now = device->getTimer()->getTime();
    if(delay>=0)
      delay-=double(now-then)/500.0;
    then = now;
	  
	  if(slice==0&&e.MouseDown){
	    triangle3df tmp;
	    slice=collMan-> getSceneNodeAndCollisionPointFromRay(
	        collMan->getRayFromScreenCoordinates(e.MousePos),sliceStart,tmp);
	    sliced=0;
	  }
	  if(slice){
	    line3d<f32> ray=collMan->getRayFromScreenCoordinates(e.MousePos);
	    vector3df dir=con(to_vector(slicers[slice]));
	    vector3df ldir=ray.getVector();
	    f32 d=(sliceStart-ray.start).dotProduct(dir*ldir.getLengthSQ()-ldir*dir.dotProduct(ldir))/(dir.dotProduct(ldir)*dir.dotProduct(ldir)-dir.getLengthSQ()*ldir.getLengthSQ())/15-sliced;
	    while(d>1&&md.hideSide(slicers[slice],false)){
	      d--;
	      sliced++;
	    }
	    while(d<-1&&md.hideSide(slicers[slice],true)){
	      d++;
	      sliced--;
	    }
	      
	    if(!e.MouseDown)
	      slice=0;
	  }
	    
    if(delay<0&&e.IsKeyDown(irr::KEY_PERIOD)){
      md.hideSide(RIGHT,true);
      delay=1;
    }
    if(delay<0&&e.IsKeyDown(irr::KEY_COMMA)){
      md.hideSide(RIGHT,false);
      delay=1;
    }
    if(delay<0&&e.IsKeyDown(irr::KEY_KEY_M)){
      md.hideSide(LEFT,false);
      delay=1;
    }
    if(delay<0&&e.IsKeyDown(irr::KEY_KEY_N)){
      md.hideSide(LEFT,true);
      delay=1;
    }

    if(delay<0&&e.IsKeyDown(irr::KEY_KEY_L)){
      md.hideSide(FORWARD,true);
      delay=1;
    }
    if(delay<0&&e.IsKeyDown(irr::KEY_KEY_K)){
      md.hideSide(FORWARD,false);
      delay=1;
    }
    if(delay<0&&e.IsKeyDown(irr::KEY_KEY_J)){
      md.hideSide(BACK,false);
      delay=1;
    }
    if(delay<0&&e.IsKeyDown(irr::KEY_KEY_H)){
      md.hideSide(BACK,true);
      delay=1;
    }

    if(delay<0&&e.IsKeyDown(irr::KEY_KEY_O)){
      md.hideSide(UP,true);
      delay=1;
    }
    if(delay<0&&e.IsKeyDown(irr::KEY_KEY_I)){
      md.hideSide(UP,false);
      delay=1;
    }
    if(delay<0&&e.IsKeyDown(irr::KEY_KEY_U)){
      md.hideSide(DOWN,false);
      delay=1;
    }
    if(delay<0&&e.IsKeyDown(irr::KEY_KEY_Y)){
      md.hideSide(DOWN,true);
      delay=1;
    }
    
    if(delay<0&&e.IsKeyDown(irr::KEY_KEY_V)){
      if(ss.slide(true,true)){
        sd.updateActive();
        delay=1;
        }
    }
    if(delay<0&&e.IsKeyDown(irr::KEY_KEY_C)){
      if(ss.slide(true,false)){
        sd.updateActive();
        delay=1;
        }
    }
    if(delay<0&&e.IsKeyDown(irr::KEY_KEY_X)){
      if(ss.slide(false,true)){
        sd.updateActive();
        delay=1;
        }
    }
    if(delay<0&&e.IsKeyDown(irr::KEY_KEY_Z)){
      if(ss.slide(false,false)){
        sd.updateActive();
        delay=1;
        }
    }

    if(delay<0&&e.IsKeyDown(irr::KEY_KEY_W)){
      if(ss.tryMove(UP)){
        sd.update();
        delay=1;
      }
    }
    if(delay<0&&e.IsKeyDown(irr::KEY_KEY_S)){
      if(ss.tryMove(DOWN)){
        sd.update();
        delay=1;
      }
    }
    if(delay<0&&e.IsKeyDown(irr::KEY_KEY_A)){
      if(ss.tryMove(RIGHT)){
        sd.update();
        delay=1;
      }
    }
    if(delay<0&&e.IsKeyDown(irr::KEY_KEY_D)){
      if(ss.tryMove(LEFT)){
        sd.update();
        delay=1;
      }
    }
    if(delay<0&&e.IsKeyDown(irr::KEY_KEY_Q)){
      if(ss.tryMove(FORWARD)){
        sd.update();
        delay=1;
      }
    }
    if(delay<0&&e.IsKeyDown(irr::KEY_KEY_E)){
      if(ss.tryMove(BACK)){
        sd.update();
        delay=1;
      }
    }
	
		driver->beginScene(true, true, SColor(255,100,101,140));

		smgr->drawAll();
		driver->endScene();
	}
	device->drop();

	return 0;
}


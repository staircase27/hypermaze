#define IRRLICHT
#include "irrlicht.h"
#include "gui.hh"
#include "mazegen.hh"
#include "vector.hh"
#include "iMyCamera.hh"
#include "dirns.hh"

namespace irr{
  using namespace core;
  using namespace scene;
  using namespace io;
  using namespace video;
};

class MyEventReceiver : public irr::IEventReceiver
{

private:
        // We use this array to store the current state of each key
        bool KeyIsDown[irr::KEY_KEY_CODES_COUNT];
public:
        bool MouseDown;
        irr::position2d<irr::s32> MousePos;
    // This is the one method that we have to implement
    virtual bool OnEvent(const irr::SEvent& event)
    {
        // Remember whether each key is down or up
        if (event.EventType == irr::EET_KEY_INPUT_EVENT)
                KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;

    if (event.EventType != irr::EET_MOUSE_INPUT_EVENT)
        return false;

    switch(event.MouseInput.Event)
    {
    case irr::EMIE_LMOUSE_PRESSED_DOWN:
        MouseDown = true;
        break;
    case irr::EMIE_LMOUSE_LEFT_UP:
        MouseDown = false;
        break;
    case irr::EMIE_MOUSE_MOVED:
        MousePos = irr::position2d<irr::s32> (event.MouseInput.X,event.MouseInput.Y);
        break;
    }
                return false;
        }

        // This is used to check whether a key is being held down
        virtual bool IsKeyDown(irr::EKEY_CODE keyCode) const
        {
                return KeyIsDown[keyCode];
        }
        
        MyEventReceiver():MouseDown(false),MousePos(0,0)
        {
                for (irr::u32 i=0; i<irr::KEY_KEY_CODES_COUNT; ++i)
                        KeyIsDown[i] = false;
        }

};

class MyNodeGen:public NodeGen{
  irr::ISceneManager* smgr;
  irr::ITexture* wall;
  irr::ITexture* string;
  irr::ITexture* activeString;
  public:
    MyNodeGen(irr::ISceneManager* smgr,irr::ITexture* wall,irr::ITexture* string,irr::ITexture* activeString):
        smgr(smgr),wall(wall),string(string),activeString(activeString){};
    virtual irr::IMeshSceneNode* makeUnitWall(bool isNode){
      irr::IMeshSceneNode* node = smgr->addCubeSceneNode(1);
      node->setMaterialTexture( 0, wall);
      node->setMaterialFlag(irr::video::EMF_LIGHTING, true);
      return node;
    }

    virtual irr::IMeshSceneNode* makeUnitString(bool isNode){
      irr::IMeshSceneNode* node = smgr->addCubeSceneNode(1);
      node->setMaterialTexture( 0, string);
      node->setMaterialFlag(irr::video::EMF_LIGHTING, true);
      return node;
    }

    virtual void makeStringActive(irr::IMeshSceneNode* node,bool active){
      if(active)
        node->setMaterialTexture(0,activeString);
      else
        node->setMaterialTexture(0,string);
    }

};


int main(){

  MyEventReceiver e;
  
	irr::IrrlichtDevice *device =
		irr::createDevice( irr::video::EDT_OPENGL, irr::dimension2d<irr::u32>(640, 480), 16,
			false, false, false, &e);

	if (!device)
		return 1;
		
	
	device->setWindowCaption(L"Hyper Maze! - Irrlicht");
	irr::IVideoDriver* driver = device->getVideoDriver();
	irr::ISceneManager* smgr = device->getSceneManager();
	
	addCameraSceneNodeMy(smgr,device->getCursorControl(),0,irr::vector3df(0,0,0),
	    irr::vector3df(0,0,-300),-500.,-100.,3.,100.);
	
  irr::ILightSceneNode* light1 =
                smgr->addLightSceneNode(0, irr::vector3df(-100,100,-100),
                irr::video::SColorf(1.0f,1.0f,1.0f,1.0f), 800.0f);
  smgr->setAmbientLight(irr::video::SColorf(0.3,0.3,0.3,1));

  
                
  Maze m=generate(Vector(5,5,5));
  String s(m);
  StringSlice ss(s);
  
  
  MyNodeGen* ng=new MyNodeGen(smgr,driver->getTexture("irrlicht/wall.png"),driver->getTexture("irrlicht/string.png"),driver->getTexture("irrlicht/activeString.png"));
  cout<<"display"<<endl;
  MazeDisplay md(m,ng);
  StringDisplay sd(ss,ng);

  map<irr::ISceneNode*,Dirn> slicers;
  
  irr::f32 r=irr::vector3df(m.size.X,m.size.Y,m.size.Z).getLength()/2;
  
  for(Dirn* d=allDirns;d!=allDirns+6;++d){
    irr::IMeshSceneNode* node = smgr->addSphereSceneNode(r*(md.wall+md.gap)/15);
    node->setMaterialType(irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL);
    node->setMaterialTexture( 0, driver->getTexture("irrlicht/handle.png"));
    node->setMaterialFlag(irr::video::EMF_LIGHTING, true);
    node->setPosition(-con(to_vector(*d))*r*(md.wall+md.gap));
    irr::ITriangleSelector* selector = smgr->createTriangleSelector(node->getMesh(),node);
    node->setTriangleSelector(selector);
    selector->drop(); // We're done with this selector, so drop it now.
    slicers[node]=*d;
  }

  irr::ISceneCollisionManager* collMan = smgr->getSceneCollisionManager();
  irr::ISceneNode* slice=0;
  int sliced=0;
  irr::vector3df sliceStart;
  
  irr::u32 then = device->getTimer()->getTime();

  double delay=-1;
	while(device->run())
	{
    const irr::u32 now = device->getTimer()->getTime();
    if(delay>=0)
      delay-=double(now-then)/500.0;
    then = now;
	  
	  if(slice==0&&e.MouseDown){
	    irr::triangle3df tmp;
	    slice=collMan-> getSceneNodeAndCollisionPointFromRay(
	        collMan->getRayFromScreenCoordinates(e.MousePos),sliceStart,tmp);
	    sliced=0;
	  }
	  if(slice){
	    irr::line3d<irr::f32> ray=collMan->getRayFromScreenCoordinates(e.MousePos);
	    irr::vector3df dir=con(to_vector(slicers[slice]));
	    irr::vector3df ldir=ray.getVector();
	    irr::f32 d=(sliceStart-ray.start).dotProduct(dir*ldir.getLengthSQ()-ldir*dir.dotProduct(ldir))/(dir.dotProduct(ldir)*dir.dotProduct(ldir)-dir.getLengthSQ()*ldir.getLengthSQ())/15-sliced;
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
	
		driver->beginScene(true, true, irr::SColor(255,100,101,140));

		smgr->drawAll();
		driver->endScene();
	}
	device->drop();

	return 0;
}


#include "irrlicht.h"
#include "gui.hh"
#include "mazegen.hh"
#include "vector.hh"
#include "iMyCamera.hh"

using namespace irr;
using namespace core;
using namespace scene;
using namespace io;
using namespace video;

class MyEventReceiver : public IEventReceiver
{
public:
        // This is the one method that we have to implement
        virtual bool OnEvent(const SEvent& event)
        {
                // Remember whether each key is down or up
                if (event.EventType == irr::EET_KEY_INPUT_EVENT)
                        KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;

                return false;
        }

        // This is used to check whether a key is being held down
        virtual bool IsKeyDown(EKEY_CODE keyCode) const
        {
                return KeyIsDown[keyCode];
        }
        
        MyEventReceiver()
        {
                for (u32 i=0; i<KEY_KEY_CODES_COUNT; ++i)
                        KeyIsDown[i] = false;
        }

private:
        // We use this array to store the current state of each key
        bool KeyIsDown[KEY_KEY_CODES_COUNT];
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
  
  MazeDisplay md(m,smgr,driver->getTexture("irrlicht/wall.png"));

  int delay=0;
	while(device->run())
	{
	  --delay;
	  
    if(delay<0&&e.IsKeyDown(irr::KEY_PERIOD)){
      md.hideSide(RIGHT,true);
      delay=100;
    }
    if(delay<0&&e.IsKeyDown(irr::KEY_COMMA)){
      md.hideSide(RIGHT,false);
      delay=100;
    }
    if(delay<0&&e.IsKeyDown(irr::KEY_KEY_M)){
      md.hideSide(LEFT,false);
      delay=100;
    }
    if(delay<0&&e.IsKeyDown(irr::KEY_KEY_N)){
      md.hideSide(LEFT,true);
      delay=100;
    }

    if(delay<0&&e.IsKeyDown(irr::KEY_KEY_L)){
      md.hideSide(FORWARD,true);
      delay=100;
    }
    if(delay<0&&e.IsKeyDown(irr::KEY_KEY_K)){
      md.hideSide(FORWARD,false);
      delay=100;
    }
    if(delay<0&&e.IsKeyDown(irr::KEY_KEY_J)){
      md.hideSide(BACK,false);
      delay=100;
    }
    if(delay<0&&e.IsKeyDown(irr::KEY_KEY_H)){
      md.hideSide(BACK,true);
      delay=100;
    }

    if(delay<0&&e.IsKeyDown(irr::KEY_KEY_O)){
      md.hideSide(UP,true);
      delay=100;
    }
    if(delay<0&&e.IsKeyDown(irr::KEY_KEY_I)){
      md.hideSide(UP,false);
      delay=100;
    }
    if(delay<0&&e.IsKeyDown(irr::KEY_KEY_U)){
      md.hideSide(DOWN,false);
      delay=100;
    }
    if(delay<0&&e.IsKeyDown(irr::KEY_KEY_Y)){
      md.hideSide(DOWN,true);
      delay=100;
    }

	
		driver->beginScene(true, true, SColor(255,100,101,140));

		smgr->drawAll();
		driver->endScene();
	}
	device->drop();

	return 0;
}


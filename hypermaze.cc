#define IRRLICHT
#include "irrlicht.h"
#include "irrdisp.hh"
#include "mazegen.hh"
#include "vector.hh"
#include "iMyCamera.hh"
#include "dirns.hh"
#include "gui.hh"
#include "keymap.hh"
#include "helpgui.hh"
#include <fstream>
#include <map>
#include "controller.hh"

using namespace std;

namespace irr{
  using namespace core;
  using namespace scene;
  using namespace io;
  using namespace video;
};



class MyNodeGen:public NodeGen{
  irr::ISceneManager* smgr;
  irr::ITexture* wall;
  irr::ITexture* string;
  irr::ITexture* activeString;
  irr::ITexture* handle;
  public:
    MyNodeGen(irr::ISceneManager* smgr,irr::ITexture* wall,irr::ITexture* string,irr::ITexture* activeString,irr::ITexture* handle):
        smgr(smgr),wall(wall),string(string),activeString(activeString),handle(handle){};
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
    virtual irr::IMeshSceneNode* makeUnitHandle(){
      irr::IMeshSceneNode* node = smgr->addSphereSceneNode(1);
      node->setMaterialType(irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL);
      node->setMaterialTexture( 0, handle);
      node->setMaterialFlag(irr::video::EMF_LIGHTING, true);
      irr::ITriangleSelector* selector = smgr->createTriangleSelector(node->getMesh(),node);
      node->setTriangleSelector(selector);
      selector->drop(); // We're done with this selector, so drop it now.
      return node;
    };

};


int main(){

	irr::IrrlichtDevice *device =
		irr::createDevice( irr::video::EDT_OPENGL, irr::dimension2d<irr::u32>(640, 480), 16,
			false, false, false);

	if (!device)
		return 1;
	
	device->setResizable(true);
	
	{
    device->getGUIEnvironment()->getSkin()->setFont(device->getGUIEnvironment()->getFont("irrlicht/myfont.xml"));
    irr::SColor c=device->getGUIEnvironment()->getSkin()->getColor(irr::EGDC_3D_HIGH_LIGHT);
    c.setAlpha(170);
    device->getGUIEnvironment()->getSkin()->setColor(irr::EGDC_3D_HIGH_LIGHT,c);
  }
	
	device->setWindowCaption(L"Hyper Maze! - Irrlicht");
	irr::IVideoDriver* driver = device->getVideoDriver();
	irr::ISceneManager* smgr = device->getSceneManager();
	
	addCameraSceneNodeMy(smgr,device->getCursorControl(),0,irr::vector3df(0,0,0),
	    irr::vector3df(0,0,-300),-500.,-100.,3.,100.);
	
  irr::ILightSceneNode* light1 =
                smgr->addLightSceneNode(0, irr::vector3df(-100,100,-100),
                irr::video::SColorf(1.0f,1.0f,1.0f,1.0f), 800.0f);
  smgr->setAmbientLight(irr::video::SColorf(0.3,0.3,0.3,1));

  MyNodeGen* ng=new MyNodeGen(smgr,driver->getTexture("irrlicht/wall.png"),driver->getTexture("irrlicht/string.png"),driver->getTexture("irrlicht/activeString.png"),driver->getTexture("irrlicht/handle.png"));
  
  PuzzleDisplay pd(ng);
  
  
  MultiInterfaceController *mic=new MultiInterfaceController(pd,device);
  Controller* c=mic;
  device->setEventReceiver(c);

  mic->kc.map.addMapping(irr::KEY_F1,KeyMap::A_CONF);
  wifstream ifs("hypermaze.keymap.conf");
  ifs>>mic->kc.map;
  ifs.close();
  
  while(device->run())
	{
    const irr::u32 now = device->getTimer()->getTime();
	  
	  c->run(now);
	  
		driver->beginScene(true, true, irr::SColor(255,100,101,140));

		smgr->drawAll();
		driver->endScene();
	}
	device->drop();

	return 0;
}


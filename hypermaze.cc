#include "irrlicht.h"
#include "irrdisp.hh"
#include "vector.hh"
#include "iMyCamera.hh"
#include "dirns.hh"
#include "keymap.hh"
#include "controller.hh"
#include "sound.hh"
#include <map>
#ifdef IOSTREAM
#include <iostream>
#endif

using namespace std;

namespace irr{
  using namespace core;
  using namespace scene;
  using namespace io;
  using namespace video;
  using namespace gui;
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
      irr::ITriangleSelector* selector = smgr->createTriangleSelector(node->getMesh(),node);
      node->setTriangleSelector(selector);
      selector->drop(); // We're done with this selector, so drop it now.
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

class IrrlichtMusicSource: public MusicSource{
  irr::IFileSystem* fs;
  irr::stringc folder;
  irr::IFileList* files;
  int i;
  public:
    IrrlichtMusicSource(irr::IFileSystem* fs):fs(fs),i(-1),folder("irrlicht/music/"){
      fs->grab();
      irr::EFileSystemType type=fs->setFileListSystem(irr::FILESYSTEM_VIRTUAL);
      irr::path cwd=fs->getWorkingDirectory();
      if(!fs->changeWorkingDirectoryTo(folder)){
        files=fs->createEmptyFileList(folder,false,false);
      }else{
        files=fs->createFileList();
        bool hasfiles=false;
        for(int i=0;(!hasfiles)&&i<files->getFileCount();++i)
          if(!files->isDirectory(i))
            hasfiles=true;
        if(!hasfiles){
          files->drop();
          files=fs->createEmptyFileList(folder,false,false);
        }
      }
      fs->changeWorkingDirectoryTo(folder);
      fs->setFileListSystem(type);
    };
    virtual ~IrrlichtMusicSource(){
      fs->drop();
      files->drop();
    }
    virtual const char* getNextTrack(){
      do{
        if(++i>files->getFileCount())
          i=0;
      }while(files->isDirectory(i));
      return files->getFullFileName(i).c_str();
    };
    virtual const char* getEffectName(SoundManager::SOUND_EFFECT effect){
      switch(effect){
        case SoundManager::SE_BLOCK:
          return "irrlicht/sounds/thud.wav";
        case SoundManager::SE_WIN:
          return "irrlicht/sounds/HypermazeSuccessTrumpet2_16PCM.wav";
        default:
          return "";
      }
    };
};

class IrrlichtMusicLoader: public MusicLoader{
  irr::IFileSystem* fs;
  public:
    virtual void* loadTrack(const char* track,int& length){
      irr::IReadFile* f=fs->createAndOpenFile(track);
      int len=f->getSize()+2;
      int extra=64;
      length=0;
      char* data=new char[len];
      while(int read=f->read(data+length,len-length)){
        length+=read;
        if(length=len){
          char* tmp=new char[len+extra];
          memcpy(tmp,data,len);
          delete[] data;
          data=tmp;
          len+=extra;
          extra*=2;
        }
      }
      return data;
    }
    virtual void finished(void* data,int length){
      delete[] (char*)data;
    }
    IrrlichtMusicLoader(irr::IFileSystem* fs):fs(fs){
      fs->grab();
    };
    virtual ~IrrlichtMusicLoader(){
      fs->drop();
    }
    
};

int main(int argc,char* argv[]){

	irr::IrrlichtDevice *device =
		irr::createDevice( irr::video::EDT_OPENGL, irr::dimension2d<irr::u32>(640, 480), 16,
			false, false, false);

	if (!device)
		return 1;

	device->setResizable(true);
	
	{
	  irr::IFileSystem* fs=device->getFileSystem();
	  fs->addFileArchive(fs->getFileDir(argv[0])+"/",false,false,irr::EFAT_FOLDER);
	  fs->setFileListSystem(irr::FILESYSTEM_VIRTUAL);
	}

	{
    device->getGUIEnvironment()->getSkin()->setFont(device->getGUIEnvironment()->getFont("irrlicht/fonts/Scada16r.xml"));
    irr::SColor c=device->getGUIEnvironment()->getSkin()->getColor(irr::EGDC_3D_HIGH_LIGHT);
    c.setAlpha(170);
    device->getGUIEnvironment()->getSkin()->setColor(irr::EGDC_3D_HIGH_LIGHT,c);
  }

	device->setWindowCaption(L"Hyper Maze! - Irrlicht");
	irr::IVideoDriver* driver = device->getVideoDriver();
	irr::ISceneManager* smgr = device->getSceneManager();

	addCameraSceneNodeMy(smgr,device->getCursorControl(),0,irr::vector3df(0,0,0),
	    irr::vector3df(0,0,-300),-500.,-100.,-3.,100.);

  irr::ILightSceneNode* light1 =
                smgr->addLightSceneNode(0, irr::vector3df(-100,100,-100),
                irr::video::SColorf(1.0f,1.0f,1.0f,1.0f), 800.0f);
  smgr->setAmbientLight(irr::video::SColorf(0.3,0.3,0.3,1));

  MyNodeGen* ng=new MyNodeGen(smgr,driver->getTexture("irrlicht/wall.png"),driver->getTexture("irrlicht/string.png"),driver->getTexture("irrlicht/activeString.png"),driver->getTexture("irrlicht/handle.png"));

  PuzzleDisplay pd(ng);
  
  SoundManager* sm=createSoundManager();
  sm->setMusicSource(new IrrlichtMusicSource(device->getFileSystem()));
  sm->setMusicLoader(new IrrlichtMusicLoader(device->getFileSystem()));
  sm->startMusic();

  MultiInterfaceController *mic=new MultiInterfaceController(pd,device,sm);
  Controller* c=mic;
  device->setEventReceiver(c);
  pd.c=c;

  mic->kc.map.addMapping(irr::KEY_F1,KeyMap::A_CONF);
  {
    irr::IReadFile* in=device->getFileSystem()->createAndOpenFile("hypermaze.keymap.conf");
    mic->kc.map.load(in);
    in->drop();
  }
  
  while(device->run())
	{
    const irr::u32 now = device->getTimer()->getTime();

	  c->run(now);
	  
	  sm->run();

		driver->beginScene(true, true, irr::SColor(255,100,101,140));

		smgr->drawAll();
		driver->endScene();
	}
	device->drop();

	return 0;
}


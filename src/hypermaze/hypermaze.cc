#include "irrlicht.h"
#include "irrdisp.hh"
#include "../core/vector.hh"
#include "../irrshared/iMyCamera.hh"
#include "../core/dirns.hh"
#include "controller.hh"
#include "../shared/sound.hh"
#include <map>
#include "../core/script.hh"
#include "../core/SmartPointer.hh"
#include "../irrshared/fonts.hh"
#include "../irrshared/platformcompat.hh"

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

    virtual irr::IMeshSceneNode* makeStringEnd(){
      irr::IMeshSceneNode* node = smgr->addCubeSceneNode(1);
      node->setMaterialTexture( 0, string);
      node->setMaterialFlag(irr::video::EMF_LIGHTING, true);
      irr::IMeshSceneNode* washer = smgr->addSphereSceneNode(2,16,node);
      washer->setScale(irr::vector3df(0.01,1,1));
      washer->setPosition(irr::vector3df(-(0.5-0.01),0,0));
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

    virtual irr::IMeshSceneNode* makeUnitHandle(int isForward){
      irr::IMeshSceneNode* node = smgr->addSphereSceneNode(1);
      node->setMaterialType(irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL);
      if(isForward==0)
        node->setMaterialTexture( 0, handle);
      else if(isForward<0)
        node->setMaterialTexture( 0, string);
      else
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
      if(files->getFileCount()==0)
        return (char*)NULL;
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
          return NULL;
      }
    };
};

class IrrlichtMusicLoader: public MusicLoader{
  irr::IFileSystem* fs;
  public:
    virtual void* loadTrack(const char* track,int& length){
      irr::IReadFile* f=fs->createAndOpenFile(track);
      length=0;
      if(!f)
        return (char*)NULL;
      int len=f->getSize()+2;
      int extra=64;
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
      f->drop();
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

  irr::fschar_t* drives;
  int len=getDriveList(drives);
  cout<<"Drives: "<<len<<endl;
  for(irr::fschar_t* drive=drives;strlen(drive)>0;drive+=strlen(drive)+1){
    cout<<drive<<endl;
  }
  cout<<"End Drives"<<endl;

  irr::IrrlichtDevice *device = irr::createDevice( irr::video::EDT_OPENGL,
      irr::dimension2d<irr::u32>(640, 480), 16, false, false, false);

  if (!device)
    return 1;

  device->setResizable(true);

  {
    irr::IFileSystem* fs=device->getFileSystem();
    const irr::fschar_t* path=
    #ifdef __IRR_WCHAR_FILESYSTEM
	 _wgetenv("HYPERMAZE_DATA_DIR");
    #else
         getenv("HYPERMAZE_DATA_DIR");
    #endif
    if(!path)
      path=getDataPath();
    if(path){
      fs->addFileArchive(path,false,false,irr::EFAT_FOLDER);
      #ifdef IRRFONTFIX
        #ifdef _IRR_WCHAR_FILESYSTEM
          int len=wcslen(path);
        #else
          int len=strlen(path);
        #endif
        irr::fschar_t* tmp=new irr::fschar_t[len+1];
        memcpy(tmp,path,(len+1)*sizeof(irr::fschar_t));
        font_prefix=tmp;
      #endif
    }
  }

  FontManager fm(device->getFileSystem(),device->getGUIEnvironment());
  {
    device->getGUIEnvironment()->getSkin()->setFont(fm.getFont(16));
    irr::SColor c=device->getGUIEnvironment()->getSkin()->getColor(irr::EGDC_3D_HIGH_LIGHT);
    c.setAlpha(170);
    device->getGUIEnvironment()->getSkin()->setColor(irr::EGDC_3D_HIGH_LIGHT,c);
  }

  device->setWindowCaption(L"Hyper Maze");
  irr::IVideoDriver* driver = device->getVideoDriver();
  irr::ISceneManager* smgr = device->getSceneManager();

  {
    irr::IAnimatedMesh* mesh = smgr->getMesh("irrlicht/worldsky.obj");
    if (!mesh)
    {
      device->drop();
      return 1;
    }
    irr::IAnimatedMeshSceneNode* sky = smgr->addAnimatedMeshSceneNode( mesh );
    sky->setScale(irr::vector3df(1,1,1)*1000);
    sky->setPosition(irr::vector3df(0,-300,0));
    sky->setMaterialFlag(irr::EMF_LIGHTING, false);

    sky->getMaterial(0).setTexture( 0, driver->getTexture("irrlicht/ground.png"));
    sky->getMaterial(1).setTexture( 0, driver->getTexture("irrlicht/sky.png"));
  }

  addCameraSceneNodeMy(smgr,device->getCursorControl(),0,irr::vector3df(0,0,0),
      irr::vector3df(0,0,-300),-500.,-100.,-3.);

  irr::ILightSceneNode* light1 =
                smgr->addLightSceneNode(0, irr::vector3df(-100,100,-100),
                irr::video::SColorf(1.0f,1.0f,1.0f,1.0f), 800.0f);
  smgr->setAmbientLight(irr::video::SColorf(0.3,0.3,0.3,1));

  MyNodeGen* ng=new MyNodeGen(smgr,driver->getTexture("irrlicht/wall.png"),driver->getTexture("irrlicht/string.png"),driver->getTexture("irrlicht/activeString.png"),driver->getTexture("irrlicht/handle.png"));

  SoundManager* sm=createSoundManager();
  sm->setMusicSource(new IrrlichtMusicSource(device->getFileSystem()));
  sm->setMusicLoader(new IrrlichtMusicLoader(device->getFileSystem()));
  sm->startMusic();

  PuzzleDisplay pd(ng,device,&fm,sm);

  Controller* c=new MultiInterfaceController(pd,device,&fm,sm);
  device->setEventReceiver(c);

  while(device->run())
  {
    const irr::u32 now = device->getTimer()->getTime();

    pd.sc.setnow(now);

    c->run(now);

    sm->run();

    driver->beginScene(true, true, irr::SColor(255,100,101,140));

    smgr->drawAll();

    device->setWindowCaption((irr::stringw(L"Hyper Maze: ")+=pd.sp.getScore()).c_str());

    driver->endScene();
  }
  delete ng;
  delete c;
  delete sm->getMusicSource();
  delete sm->getMusicLoader();
  delete sm;
  device->drop();
  #ifdef IRRFONTFIX
  delete[] font_prefix;
  #endif


  return 0;
}


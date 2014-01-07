#include "irrlicht.h"
#include "irrdisp.hh"
#include "vector.hh"
#include "iMyCamera.hh"
#include "dirns.hh"
#include "keymap.hh"
#include "controller.hh"
#include "sound.hh"
#include <map>
#include "script.hh"
#include "SmartPointer.hh"

//for testing REMOVE LATER
#include "hypioimp.hh"
#include "scriptimpl.hh"


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

  irr::IrrlichtDevice *device = irr::createDevice( irr::video::EDT_OPENGL,
      irr::dimension2d<irr::u32>(640, 480), 16, false, false, false);

  if (!device)
    return 1;

  device->setResizable(true);
  
  {
    irr::IFileSystem* fs=device->getFileSystem();
    fs->addFileArchive(fs->getFileDir(argv[0])+"/",false,false,irr::EFAT_FOLDER);
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
  
  #ifdef IOSTREAM
  
  {
    StringElementCondition sec;
    cout<<"output for blank StringElementCondition"<<endl;
    SPA<char> str;
    MemoryHypOStream os(str);
    if(!write(os,sec))
      cout<<"error writing"<<endl;
    os.flush();
    cout<<"'"<<str<<"'"<<endl;
    const char* data=" 1 0 "//
                      "0 "//no x conditions
                      "1 "//one y condition
                      "1 1 "//y is exactly 1
                      "4 "//4 for the z
                      "* 0 2 3 5 *";//up to <0 2-3 and >5 (plus a missing one)
    cout<<"input"<<endl<<"'"<<data<<"'"<<endl;
    MemoryHypIStream is(data,strlen(data));
    read(is,sec);
    os.strlen=0;
    cout<<"output"<<endl;
    if(!write(os,sec))
      cout<<"error writing"<<endl;
    os.flush();
    cout<<"'"<<str<<"'"<<endl;
  }
  
  {
    SPA<char> str;
    MemoryHypOStream os(str);

    StringMatcher sm;
    
    sm.count=1;
    sm.pattern=SPA<Pair<PatternTag,StringElementCondition> >(5);
    
    write(os,sm);
    os.flush();
    cout<<"pattern"<<endl<<str<<endl;
    cout<<sm.match(pd.s)<<endl;
    
    sm.pattern[0].a.max=6000;
    os.strlen=0;
    write(os,sm);
    os.flush();
    cout<<"pattern"<<endl<<str<<endl;
    cout<<sm.match(pd.s)<<endl;
     
    sm.pattern[0].a.greedy=false;
    os.strlen=0;
    write(os,sm);
    os.flush();
    cout<<"pattern"<<endl<<str<<endl;
    cout<<sm.match(pd.s)<<endl;
     
    sm.pattern[0].a.min=6000;
    os.strlen=0;
    write(os,sm);
    os.flush();
    cout<<"pattern"<<endl<<str<<endl;
    cout<<sm.match(pd.s)<<endl;
    
    sm.pattern[0].a.min=0;
    sm.pattern[0].b.selectionCondition=0;
    os.strlen=0;
    write(os,sm);
    os.flush();
    cout<<"pattern"<<endl<<str<<endl;
    cout<<sm.match(pd.s)<<endl;
    
    sm.pattern[1].a.max=6000;
    sm.count=2;
    os.strlen=0;
    write(os,sm);
    os.flush();
    cout<<"pattern"<<endl<<str<<endl;
    cout<<sm.match(pd.s)<<endl;
    
    sm.pattern[0].a.greedy=true;
    os.strlen=0;
    write(os,sm);
    os.flush();
    cout<<"pattern"<<endl<<str<<endl;
    cout<<sm.match(pd.s)<<endl;
    
    sm.pattern[0].b.selectionCondition=1;
    sm.pattern[1].a.min=3;
    sm.pattern[1].a.max=3;
    os.strlen=0;
    write(os,sm);
    os.flush();
    cout<<"pattern"<<endl<<str<<endl;
    cout<<sm.match(pd.s)<<endl;
    
    sm.pattern[0].a.greedy=false;
    os.strlen=0;
    write(os,sm);
    os.flush();
    cout<<"pattern"<<endl<<str<<endl;
    cout<<sm.match(pd.s)<<endl;
    
    sm.groups=SPA<Pair<int> >(2);
    sm.groups[0].a=0;
    sm.groups[0].b=1;
    sm.groups[1].a=1;
    sm.groups[1].b=0;
    sm.group_count=2;
    
    SPA<Pair<SP<StringPointer> > > groups(2);
    os.strlen=0;
    write(os,sm);
    os.flush();
    cout<<"pattern"<<endl<<str<<endl;
    SPA<char> data(str);//save to reload later
    cout<<sm.match(pd.s,groups)<<endl;
    cout<<*groups[0].a<<" "<<*groups[0].b<<endl;
    cout<<*groups[1].a<<" "<<*groups[1].b<<endl;

    groups=SPA<Pair<SP<StringPointer> > >(2);
    sm.pattern[0].b.selectionCondition=3;
    os.strlen=0;
    write(os,sm);
    os.flush();
    cout<<"pattern"<<endl<<str<<endl;
    cout<<sm.match(pd.s,groups)<<endl;
    cout<<groups[0].a<<" "<<groups[0].b<<endl;
    cout<<groups[1].a<<" "<<groups[1].b<<endl;
    
    cout<<"input"<<endl<<data<<endl;
    MemoryHypIStream is(data,strlen(&*data));
    read(is,sm);
    
    groups=SPA<Pair<SP<StringPointer> > >(2);
    os.strlen=0;
    write(os,sm);
    os.flush();
    cout<<"pattern"<<endl<<str<<endl;
    cout<<sm.match(pd.s,groups)<<endl;
    cout<<*groups[0].a<<" "<<*groups[0].b<<endl;
    cout<<*groups[1].a<<" "<<*groups[1].b<<endl;
  }

  {
    const char* data=""// ((True or (True and (Not True))) And String is selected and pointing left)
      "3 2 "//and with two elements
        "2 2 "//add a or with 2 elements
            "1 "// add a true
          "3 2 "//add a and with 2 elements
            "1 "//add a true
            "4 1 "//add a not true
        "7 1 0 * 1 1 2 0 0 0 0";// string condition of any length string where all selected and pointing left
    
    cout<<"input"<<endl<<data<<endl;
    MemoryHypIStream is(data,strlen(data));

    SP<Condition> condition;
    read(is,condition);

    cout<<"parsed"<<endl;
    Script s;
    cout<<condition->is(0,s,pd.s)<<endl;

    SPA<char> str;
    MemoryHypOStream os(str);

    write(os,(SP<const Condition>&)condition);
    os.flush();
    cout<<"output"<<endl<<str<<endl;
    
    cout<<"end condition test"<<endl;
  }

  {
    cout<<"editing string"<<endl;
  	StringEdit se(pd.s);
  	StringPointer ps=pd.s.begin();
  	++ps;
  	++ps;
  	++ps;
  	StringPointer pe=ps;
  	++pe;
  	++pe;
  	Dirn d=FORWARD;
  	se.setStringSegment(ps,pe,1,&d);

  	ps=pd.s.begin();
  	++ps;
  	pe=ps;
  	++pe;
  	++pe;
  	d=BACK;
  	se.setStringSegment(ps,pe,1,&d);
  	
  	++pe;
  	++pe;
  	d=LEFT;
  	se.setStringSegment(ps,pe,1,&d);
  	
  	pd.stringUpdated();
    cout<<"edited string - should be straight again"<<endl;
  }
  
  {
    const char* data= 
        " 6 " //six actions
        "2 " //action message (1st)
        " \n 4 " //4 paragraphs
        "  \n daas£4;ca \n  \n" // format string 1
        "| dsadADy6 aa \n dsAD012 SADu12 !%^)*£\"$&  | " // paragaph 1 using |
        " sdpadhyapd ||"// entry 2 with no paragraph contents
        "\ndsaghoaiugsd !  aSDP123 \\\"£OU\n |   ! " //format and paragraph 3
        "oad Asadsadaad sadadydpsoadhaphd \n swdad A "// final format and paragraph 
        "\t\n\t\n 3"//no-op action (2nd)
        "  5" //action next level (3rd)
        " 'http://domain.path/asdadad/adsdadad.dasad?=%20%43' 'adadadaodiy!\"£'    "//url and description
        "\n\n7\n"//selection change action (4th)
        "3 63 0 0 0\t "//applying to all
        "3 63 "//selected if any selectedness and any direction and
        "2 0 0 3 4 "//x is 1 or 3-4 and
        "1 * * "//any y and
        "1 * * "//any z
        "7 3 63 1 0 3 0 0 0 63 0 0 0 \n\n"//selected if not selected for elements with x between 1 and 3 inclusive
        "9 5 "//Action Set String Route with 5 string pattern elements
        "0 10 1 3 63 0 0 0 "//0-10 (not greedy) of any selectedness or direction in any location
        "1 1 0 0 63 0 0 0 "//exactly 1 not selected string element in any direction at any location
        "1 10 0 1 63 0 0 0 "//0-10 (not greedy) of selected in any direction in any location
        "1 1 0 0 63 0 0 0 "//exactly 1 not selected string element in any direction at any location
        "0 10 1 3 63 0 0 0 "//0-10 (not greedy) of any selectedness or direction in any location
        "1 2 2 "//one group to replace which is the 3rd pattern element
        "1 0 "//new route is one UP (then connect)
        "0";//only one match
        
    cout<<"input"<<endl<<data<<endl;
    MemoryHypIStream is(data,strlen(data));

    SPA<SP<Action> > as;
    int count;
    Script s;
    read(is,as,count);
    
    cout<<"parsed "<<count<<endl;

    cout<<as<<endl;
    cout<<as[0]<<endl;    
    
    SP<Action> a(as[0]);
    
    cout<<as<<endl;
    cout<<as[0]<<endl;    
    cout<<a<<endl;

    ScriptResponseWin sr;
    for(int i=0;i<count;++i)
	    as[i]->doWin(sr,pd.s);
	  
    pd.stringUpdated();
    
    cout<<sr.messageCount<<endl;
    cout<<sr.messages->count<<endl;
    cout<<"format \""<<sr.messages->paragraphs[0].a<<"\""<<endl;
    cout<<"paragraph \""<<sr.messages->paragraphs[0].b<<"\""<<endl;
    cout<<"format \""<<sr.messages->paragraphs[1].a<<"\""<<endl;
    cout<<"paragraph \""<<sr.messages->paragraphs[1].b<<"\""<<endl;
    cout<<"format \""<<sr.messages->paragraphs[2].a<<"\""<<endl;
    cout<<"paragraph \""<<sr.messages->paragraphs[2].b<<"\""<<endl;
    cout<<"format \""<<sr.messages->paragraphs[3].a<<"\""<<endl;
    cout<<"paragraph \""<<sr.messages->paragraphs[3].b<<"\""<<endl;
    
    SPA<char> str;
    MemoryHypOStream os(str);

    write(os,(SPA<const SP<const Action> >&)as,count);
    os.flush();
    
    cout<<"output"<<endl<<str<<endl;
  }
  #endif

  while(device->run())
  {
    const irr::u32 now = device->getTimer()->getTime();

    c->run(now);
    
    sm->run();

    driver->beginScene(true, true, irr::SColor(255,100,101,140));

    smgr->drawAll();
    driver->endScene();
  }
  delete ng;
  delete mic;
  delete sm->getMusicSource();
  delete sm->getMusicLoader();
  delete sm;
  device->drop();

  return 0;
}


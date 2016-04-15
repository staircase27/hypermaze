/**
 * @file controllerimp.hh
 * @brief extra classes used to control and interact with the hypermaze
 */
#include "controller.hh"
#ifndef CONTROLLER_IMP_HH_INC
#define CONTROLLER_IMP_HH_INC

class KeyboardController:public Controller{
  bool actionTriggered[KeyMap::A_COUNT];
  irr::u32 actionTime[KeyMap::A_COUNT];
  static const irr::u32 DELAY=400;
  irr::IrrlichtDevice* device;
  FontManager* fm;
  SoundManager* sm;
  MultiInterfaceController* base;

  public:
    KeyMap map;

    virtual bool OnEvent(const irr::SEvent& event);
    virtual bool isTriggered(KeyMap::Action a);

    virtual void run(irr::u32 now);

    KeyboardController(PuzzleDisplay& pd,irr::IrrlichtDevice *device,FontManager* fm,SoundManager* sm,
                        MultiInterfaceController* base):Controller(pd),device(device),fm(fm),sm(sm),base(base){
      for (irr::u32 i=0; i<KeyMap::A_COUNT; ++i){
        actionTriggered[i] = false;
        actionTime[i] = 0;
      }
      map.load(device->getFileSystem());
      KeySpec help=map.getKeySpec(KeyMap::A_CONF);
      if(help.chr==0&&help.key==irr::KEY_KEY_CODES_COUNT)
        map.addMapping(KeySpec(irr::KEY_F1),KeyMap::A_CONF);
    };
};

class HUDController:public Controller{
  enum E_GUIBUTTONIDS{
    ID_UNDO=401,
    ID_HELP,
    ID_GENERATE,
    ID_OPEN,
    ID_SAVE
  };

  bool undoclicked,helpclicked,generateclicked,loadclicked,saveclicked;
  irr::u32 undotime,helptime,generatetime,loadtime,savetime;
  static const irr::u32 DELAY=400;
  irr::IrrlichtDevice* device;
  FontManager* fm;
  SoundManager* sm;
  MultiInterfaceController* base;

  irr::gui::IGUIEnvironment* guienv;
  irr::core::dimension2d<irr::u32> lastdim;

  irr::gui::IGUIButton* undobutton;
  irr::gui::IGUIButton* helpbutton;
  irr::gui::IGUIButton* generatebutton;
  irr::gui::IGUIButton* loadbutton;
  irr::gui::IGUIButton* savebutton;
  irr::gui::IGUIStaticText* scoretext;

  public:
    virtual bool OnEvent(const irr::SEvent& event);

    virtual void run(irr::u32 now);

    virtual void showGUI(bool show);

    HUDController(PuzzleDisplay& pd,irr::IrrlichtDevice *device,FontManager* fm,SoundManager* sm,MultiInterfaceController* base);
};

class MouseSlicerController: public Controller{
  irr::scene::ISceneCollisionManager* collMan;
  SP<Dirn> slice;
  int sliced;
  irr::core::vector3df sliceStart;
  irr::core::position2d<irr::s32> mousePos;
  MultiInterfaceController* base;

  public:
   virtual void run(irr::u32 now);

    virtual bool OnEvent(const irr::SEvent& event);

    MouseSlicerController(PuzzleDisplay& pd,irr::IrrlichtDevice *device,FontManager*,SoundManager* sm,MultiInterfaceController* base):
        Controller(pd),collMan(device->getSceneManager()->getSceneCollisionManager()),slice(),mousePos(0,0),base(base){};
};

class MouseStringDraggerController: public Controller{
  irr::scene::ISceneCollisionManager* collMan;
  irr::scene::ISceneNode* string;
  irr::core::vector3df startPoint;
  irr::core::position2d<irr::s32> mousePos;
  int alloweddirns;
  int dist;
  Dirn currdir;
  SoundManager* sm;
  MultiInterfaceController* base;

  public:
    virtual void run(irr::u32 now);

    virtual bool OnEvent(const irr::SEvent& event);

    MouseStringDraggerController(PuzzleDisplay& pd,irr::IrrlichtDevice *device,FontManager*,SoundManager* sm,MultiInterfaceController* base):
                Controller(pd),collMan(device->getSceneManager()->getSceneCollisionManager()),string(0),
                mousePos(0,0),dist(0),currdir(FORWARD),sm(sm),base(base){};
};

class MouseStringSelectorController: public Controller{
  irr::scene::ISceneCollisionManager* collMan;
  irr::core::position2d<irr::s32> mousePos;
  irr::core::vector3df startPoint;
  irr::scene::ISceneNode* string;
  std::pair<StringPointer,bool> sp;
  bool selected;
  int moved;
  MultiInterfaceController* base;

  public:

    virtual void run(irr::u32 now);

    virtual bool OnEvent(const irr::SEvent& event);

    MouseStringSelectorController(PuzzleDisplay& pd,irr::IrrlichtDevice *device,FontManager*,SoundManager* sm,MultiInterfaceController* base):
        Controller(pd),collMan(device->getSceneManager()->getSceneCollisionManager()),string(0),sp(pd.s->end(),false),base(base){};
};

class HypermazeMultiInterfaceController:public MultiInterfaceController{
  public:
    KeyboardController* kc;
    MouseSlicerController* msc;
    MouseStringDraggerController* mdc;
    MouseStringSelectorController* mssc;
    HUDController* hudc;

    HypermazeMultiInterfaceController(PuzzleDisplay& pd,irr::IrrlichtDevice *device,FontManager* fm,SoundManager* sm):
        MultiInterfaceController(pd),kc(new KeyboardController(pd,device,fm,sm,this)),msc(new MouseSlicerController(pd,device,fm,sm,this)),
        mdc(new MouseStringDraggerController(pd,device,fm,sm,this)),mssc(new MouseStringSelectorController(pd,device,fm,sm,this)),
        hudc(new HUDController(pd,device,fm,sm,this)){};

    virtual bool OnEvent(const irr::SEvent& event){
      return kc->OnEvent(event)||msc->OnEvent(event)||mdc->OnEvent(event)||mssc->OnEvent(event)||hudc->OnEvent(event);
    }

    virtual void run(irr::u32 now){
      kc->run(now);
      msc->run(now);
      mdc->run(now);
      mssc->run(now);
      hudc->run(now);
    }

    virtual void showGUI(bool show){
      hudc->showGUI(show);
    }

    virtual KeyMap& getKeyMap(){
      return kc->map;
    }

    virtual ~HypermazeMultiInterfaceController(){
      delete kc;
      delete msc;
      delete mdc;
      delete mssc;
      delete hudc;
    }
};

#endif

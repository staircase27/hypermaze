#include "gui.hh"

namespace irr{
  using namespace core;
  using namespace scene;
  using namespace io;
  using namespace video;
  using namespace gui;
};

bool BaseGui::OnEvent(const irr::SEvent &event){
  if(oldReceiver && event.EventType == irr::EET_KEY_INPUT_EVENT && !event.KeyInput.PressedDown)
    oldReceiver->OnEvent(event);
  if(oldReceiver && event.EventType == irr::EET_MOUSE_INPUT_EVENT && (event.MouseInput.Event==irr::EMIE_LMOUSE_LEFT_UP || event.MouseInput.Event==irr::EMIE_RMOUSE_LEFT_UP || event.MouseInput.Event==irr::EMIE_MMOUSE_LEFT_UP))
    oldReceiver->OnEvent(event);
  return OnEventImpl(event);
};
void BaseGui::apply(irr::IrrlichtDevice* _device){
  if(device)
    unapply();
  device=_device;
  oldReceiver=device->getEventReceiver();
  device->setEventReceiver(this);
}
void BaseGui::unapply(){
  if(device)
    device->setEventReceiver(oldReceiver);
  device=0;
};
void BaseGui::main(irr::IrrlichtDevice* _device,FontManager* _fm){
  apply(_device);
  fm=_fm;

  irr::IVideoDriver* driver = device->getVideoDriver();
  irr::ISceneManager* smgr = device->getSceneManager();
  irr::IGUIEnvironment *guienv = device->getGUIEnvironment();

  el=new CGUIEmptyElement(guienv,guienv->getRootGUIElement());
  createGUI();

  while(device->run()){

    if(!run())
      break;

    driver->beginScene(true, true, irr::SColor(255,113,113,133));
    smgr->drawAll();
    guienv->drawAll();
    driver->endScene();
  }

  unapply();
  guienv->removeFocus(guienv->getFocus());
  el->remove();
  el->drop();
}

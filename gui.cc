#include "gui.hh"
#include "irrdisp.hh"
#include "maze.hh"
#include "hypioimp.hh"
#include "mazegen.hh"
#include "GUIFormattedText.hh"

using namespace std;

namespace irr{
  using namespace core;
  using namespace scene;
  using namespace io;
  using namespace video;
  using namespace gui;
};

void addMessageToElement(GUIFormattedText* text,const Message& m){
  int buflen=0;
  wchar_t* buf=0;
  for(int i=0;i<m.count;++i){
      int len=strlen(m.paragraphs[i].b);
      if(buflen<len){
          buf=new wchar_t[len+1];
          buflen=len;
      }
      for(int j=0;j<len;++j){
          buf[j]=(wchar_t)m.paragraphs[i].b[j];
      }
      buf[len]=0;
      text->addText(buf);
      //use m.paragraphs[i].a to set the formatting eventually
  }
}

GUIFormattedText* makeElementFromMessage(irr::IGUIEnvironment* guienv,irr::IGUIElement* el,irr::rect<irr::s32> bounds, const Message& m){
  GUIFormattedText* text=new GUIFormattedText(0,guienv,el,0,bounds,true,true);
  text->setOverrideFont(0,guienv->getFont("irrlicht/fonts/Scada26B.xml"));
  text->setAllTextAlignment(irr::EGUIA_CENTER,irr::EGUIA_CENTER);
  addMessageToElement(text,m);
  return text;
}

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
void BaseGui::main(irr::IrrlichtDevice* _device){
  apply(_device);

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


bool GenerateGui::OnEventImpl(const irr::SEvent &event){
  if(event.EventType == irr::EET_GUI_EVENT){
	if(event.GUIEvent.EventType==irr::EGET_BUTTON_CLICKED){
	  if(event.GUIEvent.Caller->getID()==GUI_ID_OK_BUTTON){
		okClicked=true;
		return true;
	  }else{
		cancelClicked=true;
		return true;
	  }
	}
	if(event.GUIEvent.EventType == irr::gui::EGET_EDITBOX_ENTER){
	  okClicked=true;
	  return true;
	}
  }
  if(event.EventType == irr::EET_KEY_INPUT_EVENT && event.KeyInput.Key==irr::KEY_ESCAPE){
	cancelClicked=true;
	return true;
  }
  return false;
};
bool GenerateGui::generate(irr::IrrlichtDevice* _device,PuzzleDisplay& pd){
  this->pd=&pd;
  main(_device);
  return okClicked;
}
void GenerateGui::createGUI(){
  okClicked=cancelClicked=false;

  irr::IVideoDriver* driver = device->getVideoDriver();
  irr::IGUIEnvironment *guienv = device->getGUIEnvironment();

  irr::rect<irr::s32> rect=driver->getViewPort();
  irr::position2d<irr::s32> center=rect.getCenter();
  irr::dimension2d<irr::s32> size=rect.getSize();
  size.Width=min(400,size.Width-10);

  xSize=guienv->addSpinBox(L"5",irr::rect<irr::s32>(center.X-size.Width/2,center.Y-5-32-10-32,
	  center.X+size.Width/2,center.Y-5-32-10),true,el);
  xSize->setDecimalPlaces(0);
  xSize->setRange(3,75);
  xSize->setValue(pd->m.size.X);

  ySize=guienv->addSpinBox(L"5",irr::rect<irr::s32>(center.X-size.Width/2,center.Y-5-32,
	  center.X+size.Width/2,center.Y-5),true,el);
  ySize->setDecimalPlaces(0);
  ySize->setRange(3,75);
  ySize->setValue(pd->m.size.Y);

  zSize=guienv->addSpinBox(L"5",irr::rect<irr::s32>(center.X-size.Width/2,center.Y+5,
	  center.X+size.Width/2,center.Y+5+32),true,el);
  zSize->setDecimalPlaces(0);
  zSize->setRange(3,75);
  zSize->setValue(pd->m.size.Z);

  guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-210,center.Y+5+32+10,center.X+size.Width/2-100,center.Y+5+32+10+32),el,GUI_ID_CANCEL_BUTTON,L"Cancel");
  guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-100,center.Y+5+32+10,center.X+size.Width/2,center.Y+5+32+10+32),el,GUI_ID_OK_BUTTON,L"Generate");

  guienv->setFocus(xSize->getEditBox());

  device->setWindowCaption(L"Generate New Hyper Maze");

}
bool GenerateGui::run(){
  if(cancelClicked)
	return false;
  if(okClicked){
  pd->m=::generate<RandLimitMazeGenHalf<Hunter<RandOrderWalker<DiagonalWalker> > > >(Vector(xSize->getValue(),ySize->getValue(),zSize->getValue()));
  pd->sc=Script();
	return false;
  }
  return true;
}

bool SaveGui::OnEventImpl(const irr::SEvent &event){
  if(event.EventType == irr::EET_GUI_EVENT){
	if(event.GUIEvent.EventType==irr::gui::EGET_BUTTON_CLICKED){
	  if(event.GUIEvent.Caller->getID()==GUI_ID_OK_BUTTON){
		okClicked=true;
		return true;
	  }else{
		cancelClicked=true;
		return true;
	  }
	}
  }
  if(event.EventType == irr::EET_KEY_INPUT_EVENT && event.KeyInput.Key==irr::KEY_ESCAPE){
	cancelClicked=true;
	return true;
  }
  if(event.GUIEvent.EventType == irr::gui::EGET_EDITBOX_ENTER){
	okClicked=true;
	return true;
  }
  return false;
};
bool SaveGui::save(irr::IrrlichtDevice* _device,PuzzleDisplay& pd){
  this->pd=&pd;
  main(_device);
  return okClicked;
}
void SaveGui::createGUI(){
  okClicked=cancelClicked=false;

  irr::IVideoDriver* driver = device->getVideoDriver();
  irr::IGUIEnvironment *guienv = device->getGUIEnvironment();

  irr::rect<irr::s32> rect=driver->getViewPort();
  irr::position2d<irr::s32> center=rect.getCenter();
  irr::dimension2d<irr::s32> size=rect.getSize();
  size.Width=min(400,size.Width-10);

  fileField = guienv->addEditBox(0,irr::rect<irr::s32>(center.X-size.Width/2,center.Y-5-32,center.X+size.Width/2,center.Y-5),true,el);

  guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-210,center.Y+5,center.X+size.Width/2-100,center.Y+5+32),el,GUI_ID_CANCEL_BUTTON,L"Cancel");
  guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-100,center.Y+5,center.X+size.Width/2,center.Y+5+32),el,GUI_ID_OK_BUTTON,L"Save");

  guienv->setFocus(fileField);

  device->setWindowCaption(L"Save Hyper Maze");

}
bool SaveGui::run(){
  if(cancelClicked)
	return false;
  if(okClicked){
	irr::IWriteFile* out=device->getFileSystem()->createAndWriteFile(fileField->getText());
	if(!out)
	  return true;
	IrrHypOStream os(out);
	out->drop();
	write(os,pd->m);
	os.setNextSpace("\n\n");
	write(os,pd->sc);
	return false;
  }
      return true;
    }


bool OpenGui::OnEventImpl(const irr::SEvent &event){
  if(event.EventType == irr::EET_GUI_EVENT){
	if(event.GUIEvent.EventType==irr::gui::EGET_BUTTON_CLICKED){
	  if(event.GUIEvent.Caller->getID()==GUI_ID_OK_BUTTON){
		okClicked=true;
		return true;
	  }else{
		cancelClicked=true;
		return true;
	  }
	}
  }
  if(event.EventType == irr::EET_KEY_INPUT_EVENT && event.KeyInput.Key==irr::KEY_ESCAPE){
	cancelClicked=true;
	return true;
  }
  if(event.GUIEvent.EventType == irr::gui::EGET_EDITBOX_ENTER){
	okClicked=true;
	return true;
  }
  return false;
};
bool OpenGui::open(irr::IrrlichtDevice* _device,PuzzleDisplay& pd){
  this->pd=&pd;
  main(_device);
  return okClicked;
}
void OpenGui::createGUI(){
  okClicked=cancelClicked=false;

  irr::IVideoDriver* driver = device->getVideoDriver();
  irr::IGUIEnvironment *guienv = device->getGUIEnvironment();

  irr::rect<irr::s32> rect=driver->getViewPort();
  irr::position2d<irr::s32> center=rect.getCenter();
  irr::dimension2d<irr::s32> size=rect.getSize();
  size.Width=min(400,size.Width-10);

  fileField = guienv->addEditBox(0,irr::rect<irr::s32>(center.X-size.Width/2,center.Y-5-32,center.X+size.Width/2,center.Y-5),true,el);

  guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-210,center.Y+5,center.X+size.Width/2-100,center.Y+5+32),el,GUI_ID_CANCEL_BUTTON,L"Cancel");
  guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-100,center.Y+5,center.X+size.Width/2,center.Y+5+32),el,GUI_ID_OK_BUTTON,L"Open");

  guienv->setFocus(fileField);

  device->setWindowCaption(L"Open Hyper Maze");
}
bool OpenGui::run(){
  if(cancelClicked)
	return false;
  if(okClicked){
	irr::IReadFile* in=device->getFileSystem()->createAndOpenFile(fileField->getText());
	if(!in)
	  return true;
	IrrHypIStream is(in);
	in->drop();
	read(is,pd->m);
	pd->sc=Script();
	read(is,pd->sc);
	return false;
  }
  return true;
}

bool WinGui::OnEventImpl(const irr::SEvent &event){
  if(event.EventType == irr::EET_GUI_EVENT){
	if(event.GUIEvent.EventType==irr::gui::EGET_BUTTON_CLICKED){
	  switch(event.GUIEvent.Caller->getID()){
		case GUI_ID_OK_BUTTON:
		  okClicked=true;
		  return true;
		case GUI_ID_GENERATE_BUTTON:
		  generateClicked=true;
		  return true;
		case GUI_ID_LOAD_BUTTON:
		  loadClicked=true;
		  return true;
		case GUI_ID_SAVE_BUTTON:
		  saveClicked=true;
		  return true;
	  }
	  return false;
	}
	if(event.GUIEvent.EventType == irr::gui::EGET_EDITBOX_ENTER){
	  okClicked=true;
	  return true;
	}
  }
  if(event.EventType == irr::EET_KEY_INPUT_EVENT && event.KeyInput.Key==irr::KEY_ESCAPE &&
	  (keyblock==0||device->getTimer()->getTime()>keyblock)){
	okClicked=true;
	keyblock=0;
	return true;
  }
  return false;
};


bool WinGui::won(irr::IrrlichtDevice* _device,PuzzleDisplay& pd,const Message& m){
  this->pd=&pd;
  this->m=m;
  main(_device);
  return true;
}

void WinGui::createGUI(){
  okClicked=generateClicked=loadClicked=saveClicked=false;
  keyblock=0;

  irr::IVideoDriver* driver = device->getVideoDriver();
  irr::IGUIEnvironment *guienv = device->getGUIEnvironment();

  irr::rect<irr::s32> rect=driver->getViewPort();
  irr::position2d<irr::s32> center=rect.getCenter();
  irr::dimension2d<irr::s32> size=rect.getSize();
  size.Width=min(600,size.Width-10);
  size.Height=min(600,size.Height-10);

  GUIFormattedText* text=new GUIFormattedText(L"Congratulations!",guienv,el,0,irr::rect<irr::s32>(center.X-size.Width/2,center.Y-size.Height/2,center.X+size.Width/2,center.Y+size.Height/2-10-32-10-32),true,true);
  text->setOverrideFont(0,guienv->getFont("irrlicht/fonts/Scada26B.xml"));
  text->setAllTextAlignment(irr::EGUIA_CENTER,irr::EGUIA_CENTER);
  addMessageToElement(text,m);
  text->addText(L"\nWhat would you like to do now?");
  guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-120,center.Y+size.Height/2-32-32-10,center.X+size.Width/2,center.Y+size.Height/2-32-10),el,GUI_ID_SAVE_BUTTON,L"Save Maze");
  guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-120,center.Y+size.Height/2-32,center.X+size.Width/2,center.Y+size.Height/2),el,GUI_ID_LOAD_BUTTON,L"Load a Maze");
  guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-300,center.Y+size.Height/2-32,center.X+size.Width/2-130,center.Y+size.Height/2),el,GUI_ID_GENERATE_BUTTON,L"Generate New Maze");
  irr::IGUIButton* back=guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-500,center.Y+size.Height/2-32,center.X+size.Width/2-310,center.Y+size.Height/2),el,GUI_ID_OK_BUTTON,L"Back to Current Maze");

  guienv->setFocus(back);

  device->setWindowCaption(L"Hyper Maze Help");
}
bool WinGui::run(){
  if(okClicked){
	return false;
  }
  if(saveClicked){
	el->setVisible(false);
	SaveGui sg;
	sg.save(device,*pd);
	el->setVisible(true);
	saveClicked=false;
	keyblock=device->getTimer()->getTime()+500;
  }
  if(loadClicked){
	el->setVisible(false);
	OpenGui og;
	if(og.open(device,*pd)){
	  pd->mazeUpdated();
	  return false;
	}
	el->setVisible(true);
	loadClicked=false;
	keyblock=device->getTimer()->getTime()+500;
  }
  if(generateClicked){
	el->setVisible(false);
	GenerateGui gg;
	if(gg.generate(device,*pd)){
	  pd->mazeUpdated();
	  return false;
	}
	el->setVisible(true);
	generateClicked=false;
	keyblock=device->getTimer()->getTime()+500;
  }
  return true;
}

bool MessageGui::OnEventImpl(const irr::SEvent &event){
  if(event.EventType == irr::EET_GUI_EVENT && event.GUIEvent.EventType==irr::gui::EGET_BUTTON_CLICKED)
    if(event.GUIEvent.Caller->getID()==GUI_ID_OK_BUTTON){
      okClicked=true;
      return true;
  }
  if(event.EventType == irr::EET_KEY_INPUT_EVENT && event.KeyInput.Key==irr::KEY_ESCAPE){
	okClicked=true;
	return true;
  }
  return false;
};
bool MessageGui::message(irr::IrrlichtDevice* _device,const Message& m){
  this->m=m;
  main(_device);
  return okClicked;
}
void MessageGui::createGUI(){
  okClicked=false;

  irr::IVideoDriver* driver = device->getVideoDriver();
  irr::IGUIEnvironment *guienv = device->getGUIEnvironment();

  irr::rect<irr::s32> rect=driver->getViewPort();
  irr::position2d<irr::s32> center=rect.getCenter();
  irr::dimension2d<irr::s32> size=rect.getSize();
  size.Width=min(400,size.Width-10);
  size.Height=min(600,size.Height-10);
  
  makeElementFromMessage(guienv,el,irr::rect<irr::s32>(center.X-size.Width/2,center.Y-size.Height/2,center.X+size.Width/2,center.Y+size.Height/2-10-32),m);

  guienv->setFocus(guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-120,center.Y+size.Height/2-32,center.X+size.Width/2,center.Y+size.Height/2),el,GUI_ID_OK_BUTTON,L"Ok"));

  device->setWindowCaption(L"Hyper Maze Message");
}
bool MessageGui::run(){
  if(okClicked){
    return false;
  }
  return true;
}

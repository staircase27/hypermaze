#include "gui.hh"#include "irrdisp.hh"#include "../core/maze.hh"#include "../shared/irrhypioimp.hh"#include "../core/mazegen.hh"#include "../irrshared/GUIFormattedText.hh"#include "../irrshared/irrcurl.hh"namespace irr{  using namespace core;  using namespace scene;  using namespace io;  using namespace video;  using namespace gui;};using namespace std;void addMessageToElement(GUIFormattedText* text,FontManager* fm,const Message& m){  int buflen=0;  wchar_t* buf=0;  for(int i=0;i<m.count;++i){      int len=strlen(m.paragraphs[i].b);      if(buflen<len){          buf=new wchar_t[len+1];          buflen=len;      }      for(int j=0;j<len;++j){          buf[j]=(wchar_t)m.paragraphs[i].b[j];      }      buf[len]=0;      int ti=text->addText(buf);      if(strlen(m.paragraphs[i].a)>0){        //format is: size:B?I?:colour:font name        int size=0;        SPA<const char> end=m.paragraphs[i].a;        while(*end!=0&&*end!=':'){          if('0'<=*end&&*end<='9'){            size*=10;            size+=*end-'0';          }          ++end;        }        bool bold=false;        bool italic=false;        if(*end!=0)          ++end;        while(*end!=0&&*end!=':'){          if(*end=='b'||*end=='B')            bold=true;          else if(*end=='i'||*end=='I')            italic=true;          ++end;        }        if(*end!=0)          ++end;        if(*end!=0&&*end!=':'){          int len=0;          int colour=0;          //read colour          while(*end!=0&&*end!=':'){            if(('0'<=*end&&*end<='9')||('A'<=*end&&*end<='F')){              ++len;              colour*=16;              if('0'<=*end&&*end<='9')                colour+=*end-'0';              else                colour+=*end-'A'+10;            }            ++end;          }          if(len>0){            if(len<=2){              if(len==1)                colour==(colour&0xf) | ((colour&0xf)<<4);              colour==(colour&0xff) | ((colour&0xff)<<8) | ((colour&0xff)<<16) | 0xff000000;            }else{              if(len<=4){                colour=(colour&0xf) | ((colour&0xf)<<4) | ((colour&0xf0)<<4) | ((colour&0xf0)<<8)  |                       ((colour&0xf00)<<8) | ((colour&0xf00)<<12)  | ((colour&0xf000)<<12);                len+=3;              }              if(len<=6){                colour|=0xff000000;              }else if(len==7){                colour|=((colour&0x0f000000)<<4);              }            }            text->setOverrideColor(ti,irr::SColor(colour));          }        }        if(*end==0){          text->setOverrideFont(ti,fm->getFont(size,bold,italic));        }else{          //read font name and use          irr::stringc font(&*end);          text->setOverrideFont(ti,fm->getFont(font,size,bold,italic));        }        //use m.paragraphs[i].a to set the formatting using fonts from fm      }  }}GUIFormattedText* makeElementFromMessage(irr::IGUIEnvironment* guienv,FontManager* fm,irr::IGUIElement* el,irr::rect<irr::s32> bounds, const Message& m){  GUIFormattedText* text=new GUIFormattedText(0,guienv,el,0,bounds,true,true);  text->setAllTextAlignment(irr::EGUIA_CENTER,irr::EGUIA_CENTER);  addMessageToElement(text,fm,m);  text->drop();  return text;}bool BaseGui::OnEvent(const irr::SEvent &event){  if(oldReceiver && event.EventType == irr::EET_KEY_INPUT_EVENT && !event.KeyInput.PressedDown)	oldReceiver->OnEvent(event);  if(oldReceiver && event.EventType == irr::EET_MOUSE_INPUT_EVENT && (event.MouseInput.Event==irr::EMIE_LMOUSE_LEFT_UP || event.MouseInput.Event==irr::EMIE_RMOUSE_LEFT_UP || event.MouseInput.Event==irr::EMIE_MMOUSE_LEFT_UP))	oldReceiver->OnEvent(event);  return OnEventImpl(event);};void BaseGui::apply(irr::IrrlichtDevice* _device){  if(device)	unapply();  device=_device;  oldReceiver=device->getEventReceiver();  device->setEventReceiver(this);}void BaseGui::unapply(){  if(device)	device->setEventReceiver(oldReceiver);  device=0;};void BaseGui::main(irr::IrrlichtDevice* _device,FontManager* _fm){  apply(_device);  fm=_fm;  irr::IVideoDriver* driver = device->getVideoDriver();  irr::ISceneManager* smgr = device->getSceneManager();  irr::IGUIEnvironment *guienv = device->getGUIEnvironment();  el=new CGUIEmptyElement(guienv,guienv->getRootGUIElement());  createGUI();  while(device->run()){	if(!run())	  break;	driver->beginScene(true, true, irr::SColor(255,113,113,133));	smgr->drawAll();	guienv->drawAll();	driver->endScene();  }  unapply();  guienv->removeFocus(guienv->getFocus());  el->remove();  el->drop();}bool ErrorGui::OnEventImpl(const irr::SEvent &event){  if(event.EventType == irr::EET_GUI_EVENT && event.GUIEvent.EventType==irr::gui::EGET_BUTTON_CLICKED)    if(event.GUIEvent.Caller->getID()==GUI_ID_OK_BUTTON){      okClicked=true;      return true;  }  if(event.EventType == irr::EET_KEY_INPUT_EVENT && event.KeyInput.Key==irr::KEY_ESCAPE){	okClicked=true;	return true;  }  return false;};bool ErrorGui::error(irr::IrrlichtDevice* _device,FontManager* _fm,const irr::core::stringw msg,irr::core::stringc detail){  Message m;  m.paragraphs=SPA<Pair<SPA<const char> > >(1);  SPA<char> tmp(1);  tmp[0]='\0';  m.paragraphs[0].a=tmp;  tmp=SPA<char>(detail.size()+1);  memcopy(tmp,detail.c_str(),detail.size()+1);  m.paragraphs[0].b=tmp;  m.count=1;  return this->error(_device,_fm,msg,m);}bool ErrorGui::error(irr::IrrlichtDevice* _device,FontManager* _fm,const irr::core::stringw msg,const Message& detail){  this->msg=msg;  this->detail=detail;  main(_device,_fm);  return okClicked;}void ErrorGui::createGUI(){  okClicked=false;  irr::IVideoDriver* driver = device->getVideoDriver();  irr::IGUIEnvironment *guienv = device->getGUIEnvironment();  irr::rect<irr::s32> rect=driver->getViewPort();  irr::position2d<irr::s32> center=rect.getCenter();  irr::dimension2d<irr::s32> size=rect.getSize();  size.Width=min(400,size.Width-10);  size.Height=min(600,size.Height-10);  irr::video::ITexture* tex = device->getVideoDriver()->getTexture("irrlicht/error.png");  irr::dimension2d<irr::u32> imsize;  if(tex!=0){    imsize=tex->getSize();    guienv->addImage(tex,irr::core::position2d<irr::s32>(center.X-size.Width/2,center.Y-size.Height/2),true,el,-1,L"Error");  }  GUIFormattedText* text=new GUIFormattedText(msg.c_str(),guienv,el,0,irr::core::rect<irr::s32>(center.X-size.Width/2+imsize.Width+10,center.Y-size.Height/2,center.X+size.Width/2,center.Y-size.Height/2+imsize.Height),true,true);  text->setOverrideFont(0,fm->getFont(24,true));  text->setAllTextAlignment(irr::gui::EGUIA_CENTER,irr::gui::EGUIA_CENTER);  text->drop();  makeElementFromMessage(guienv,fm,el,irr::rect<irr::s32>(center.X-size.Width/2,center.Y-size.Height/2+imsize.Height+10,center.X+size.Width/2,center.Y+size.Height/2-10-32),this->detail);  guienv->setFocus(guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-120,center.Y+size.Height/2-32,center.X+size.Width/2,center.Y+size.Height/2),el,GUI_ID_OK_BUTTON,L"Ok"));  device->setWindowCaption(L"Hyper Maze: Error");}bool ErrorGui::run(){  if(okClicked){    return false;  }  return true;}bool ConfirmGui::OnEventImpl(const irr::SEvent &event){  if(event.EventType == irr::EET_GUI_EVENT && event.GUIEvent.EventType==irr::gui::EGET_BUTTON_CLICKED){    if(event.GUIEvent.Caller->getID()==GUI_ID_OK_BUTTON){      okClicked=true;      return true;    }    if(event.GUIEvent.Caller->getID()==GUI_ID_CANCEL_BUTTON){      cancelClicked=true;      return true;    }  }  if(event.EventType == irr::EET_KEY_INPUT_EVENT && event.KeyInput.Key==irr::KEY_ESCAPE){	cancelClicked=true;	return true;  }  return false;};bool ConfirmGui::confirm(irr::IrrlichtDevice* _device,FontManager* _fm,const irr::core::stringw msg,irr::core::stringc detail){  Message m;  m.paragraphs=SPA<Pair<SPA<const char> > >(1);  SPA<char> tmp(1);  tmp[0]='\0';  m.paragraphs[0].a=tmp;  tmp=SPA<char>(detail.size()+1);  memcopy(tmp,detail.c_str(),detail.size()+1);  m.paragraphs[0].b=tmp;  m.count=1;  return this->confirm(_device,_fm,msg,m);}bool ConfirmGui::confirm(irr::IrrlichtDevice* _device,FontManager* _fm,const irr::core::stringw msg,const Message& detail){  this->msg=msg;  this->detail=detail;  main(_device,_fm);  return okClicked;}void ConfirmGui::createGUI(){  okClicked=false;  cancelClicked=false;  irr::IVideoDriver* driver = device->getVideoDriver();  irr::IGUIEnvironment *guienv = device->getGUIEnvironment();  irr::rect<irr::s32> rect=driver->getViewPort();  irr::position2d<irr::s32> center=rect.getCenter();  irr::dimension2d<irr::s32> size=rect.getSize();  size.Width=min(400,size.Width-10);  size.Height=min(600,size.Height-10);  irr::video::ITexture* tex = device->getVideoDriver()->getTexture("irrlicht/confirm.png");  irr::dimension2d<irr::u32> imsize;  if(tex!=0){    imsize=tex->getSize();    guienv->addImage(tex,irr::core::position2d<irr::s32>(center.X-size.Width/2,center.Y-size.Height/2),true,el,-1,L"Confirm");  }  if(imsize.Height<50)    imsize.Height=50;  GUIFormattedText* text=new GUIFormattedText(msg.c_str(),guienv,el,0,irr::core::rect<irr::s32>(center.X-size.Width/2+imsize.Width+10,center.Y-size.Height/2,center.X+size.Width/2,center.Y-size.Height/2+imsize.Height),true,true);  text->setOverrideFont(0,fm->getFont(24,true));  text->setAllTextAlignment(irr::gui::EGUIA_CENTER,irr::gui::EGUIA_CENTER);  text->drop();  makeElementFromMessage(guienv,fm,el,irr::rect<irr::s32>(center.X-size.Width/2,center.Y-size.Height/2+imsize.Height+10,center.X+size.Width/2,center.Y+size.Height/2-10-32),this->detail);  guienv->setFocus(guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-120-120-10,center.Y+size.Height/2-32,center.X+size.Width/2-120-10,center.Y+size.Height/2),el,GUI_ID_OK_BUTTON,L"Ok"));  guienv->setFocus(guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-120,center.Y+size.Height/2-32,center.X+size.Width/2,center.Y+size.Height/2),el,GUI_ID_CANCEL_BUTTON,L"Cancel"));  device->setWindowCaption(L"Hyper Maze: Confirm");}bool ConfirmGui::run(){  if(okClicked || cancelClicked){    return false;  }  return true;}bool GenerateGui::OnEventImpl(const irr::SEvent &event){  if(event.EventType == irr::EET_GUI_EVENT){	if(event.GUIEvent.EventType==irr::EGET_BUTTON_CLICKED){	  if(event.GUIEvent.Caller->getID()==GUI_ID_OK_BUTTON){		okClicked=true;		return true;	  }else{		cancelClicked=true;		return true;	  }	}	if(event.GUIEvent.EventType == irr::gui::EGET_EDITBOX_ENTER){	  okClicked=true;	  return true;	}  }  if(event.EventType == irr::EET_KEY_INPUT_EVENT && event.KeyInput.Key==irr::KEY_ESCAPE){	cancelClicked=true;	return true;  }  return false;};bool GenerateGui::generate(irr::IrrlichtDevice* _device,FontManager* _fm,PuzzleDisplay& pd){  this->pd=&pd;  main(_device,_fm);  return okClicked;}void GenerateGui::createGUI(){  okClicked=cancelClicked=false;  irr::IVideoDriver* driver = device->getVideoDriver();  irr::IGUIEnvironment *guienv = device->getGUIEnvironment();  irr::rect<irr::s32> rect=driver->getViewPort();  irr::position2d<irr::s32> center=rect.getCenter();  irr::dimension2d<irr::s32> size=rect.getSize();  size.Width=min(400,size.Width-10);  xSize=guienv->addSpinBox(L"5",irr::rect<irr::s32>(center.X-size.Width/2,center.Y-5-32-10-32,	  center.X+size.Width/2,center.Y-5-32-10),true,el);  xSize->setDecimalPlaces(0);  xSize->setRange(3,75);  xSize->setValue(pd->m.size.X);  ySize=guienv->addSpinBox(L"5",irr::rect<irr::s32>(center.X-size.Width/2,center.Y-5-32,	  center.X+size.Width/2,center.Y-5),true,el);  ySize->setDecimalPlaces(0);  ySize->setRange(3,75);  ySize->setValue(pd->m.size.Y);  zSize=guienv->addSpinBox(L"5",irr::rect<irr::s32>(center.X-size.Width/2,center.Y+5,	  center.X+size.Width/2,center.Y+5+32),true,el);  zSize->setDecimalPlaces(0);  zSize->setRange(3,75);  zSize->setValue(pd->m.size.Z);  guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-210,center.Y+5+32+10,center.X+size.Width/2-100,center.Y+5+32+10+32),el,GUI_ID_CANCEL_BUTTON,L"Cancel");  guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-100,center.Y+5+32+10,center.X+size.Width/2,center.Y+5+32+10+32),el,GUI_ID_OK_BUTTON,L"Generate");  guienv->setFocus(xSize->getEditBox());  device->setWindowCaption(L"Hyper Maze: Generate New Maze");}bool GenerateGui::run(){  if(cancelClicked)	return false;  if(okClicked){  pd->m=::generate<RandLimitMazeGenHalf<Hunter<RandOrderWalker<DiagonalWalker> > > >(Vector(xSize->getValue(),ySize->getValue(),zSize->getValue()));  pd->sc=Script();	return false;  }  return true;}bool SaveGui::OnEventImpl(const irr::SEvent &event){  if(event.EventType == irr::EET_GUI_EVENT){	if(event.GUIEvent.EventType==irr::gui::EGET_BUTTON_CLICKED){	  if(event.GUIEvent.Caller->getID()==GUI_ID_OK_BUTTON){		okClicked=true;		return true;	  }else{		cancelClicked=true;		return true;	  }	}  }  if(event.EventType == irr::EET_KEY_INPUT_EVENT && event.KeyInput.Key==irr::KEY_ESCAPE){	cancelClicked=true;	return true;  }  if(event.GUIEvent.EventType == irr::gui::EGET_EDITBOX_ENTER){	okClicked=true;	return true;  }  return false;};bool SaveGui::save(irr::IrrlichtDevice* _device,FontManager* _fm,PuzzleDisplay& pd){  this->pd=&pd;  main(_device,_fm);  return okClicked;}void SaveGui::createGUI(){  okClicked=cancelClicked=false;  irr::IVideoDriver* driver = device->getVideoDriver();  irr::IGUIEnvironment *guienv = device->getGUIEnvironment();  irr::rect<irr::s32> rect=driver->getViewPort();  irr::position2d<irr::s32> center=rect.getCenter();  irr::dimension2d<irr::s32> size=rect.getSize();  size.Width=min(400,size.Width-10);  fileField = guienv->addEditBox(0,irr::rect<irr::s32>(center.X-size.Width/2,center.Y-5-32,center.X+size.Width/2,center.Y-5),true,el);  guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-210,center.Y+5,center.X+size.Width/2-100,center.Y+5+32),el,GUI_ID_CANCEL_BUTTON,L"Cancel");  guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-100,center.Y+5,center.X+size.Width/2,center.Y+5+32),el,GUI_ID_OK_BUTTON,L"Save");  guienv->setFocus(fileField);  device->setWindowCaption(L"Hyper Maze: Save");}bool SaveGui::run(){  if(cancelClicked)	return false;  if(okClicked){    if(device->getFileSystem()->existFile(fileField->getText())){    el->setVisible(false);    ConfirmGui cg;    bool overwrite = cg.confirm(device,fm,L"File already exists.","Do you want to overwrite the existing file? Existing contents will be lost.");    el->setVisible(true);    if(!overwrite){      device->getGUIEnvironment()->setFocus(fileField);	    return true;    }  }	irr::IWriteFile* out=device->getFileSystem()->createAndWriteFile(fileField->getText());	if(!out){    okClicked=false;    el->setVisible(false);    ErrorGui eg;    eg.error(device,fm,L"Error Opening File","File can't be opened for writing.");    el->setVisible(true);    device->getGUIEnvironment()->setFocus(fileField);	  return true;  }	IrrHypOStream os(out);	out->drop();	bool status=write(os,pd->m);	os.setNextSpace("\n\n");	status&=write(os,pd->sc);	if(!status){    okClicked=false;    el->setVisible(false);    ErrorGui eg;    eg.error(device,fm,L"Error writing maze to file","There was an error while writing to the file. The file may only include a partial or broken level. Please try again.");    el->setVisible(true);	  return true;	}	return false;  }      return true;    }bool OpenGui::OnEventImpl(const irr::SEvent &event){  if(event.EventType == irr::EET_GUI_EVENT){	if(event.GUIEvent.EventType==irr::gui::EGET_BUTTON_CLICKED){	  if(event.GUIEvent.Caller->getID()==GUI_ID_OK_BUTTON){		okClicked=true;		return true;	  }else{		cancelClicked=true;		return true;	  }	}  }  if(event.EventType == irr::EET_KEY_INPUT_EVENT && event.KeyInput.Key==irr::KEY_ESCAPE){	cancelClicked=true;	return true;  }  if(event.GUIEvent.EventType == irr::gui::EGET_EDITBOX_ENTER){	okClicked=true;	return true;  }  return false;};bool OpenGui::open(irr::IrrlichtDevice* _device,FontManager* _fm,PuzzleDisplay& pd){  this->pd=&pd;  main(_device,_fm);  return okClicked;}void OpenGui::createGUI(){  okClicked=cancelClicked=false;  irr::IVideoDriver* driver = device->getVideoDriver();  irr::IGUIEnvironment *guienv = device->getGUIEnvironment();  irr::rect<irr::s32> rect=driver->getViewPort();  irr::position2d<irr::s32> center=rect.getCenter();  irr::dimension2d<irr::s32> size=rect.getSize();  size.Width=min(400,size.Width-10);  fileField = guienv->addEditBox(0,irr::rect<irr::s32>(center.X-size.Width/2,center.Y-5-32,center.X+size.Width/2,center.Y-5),true,el);  guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-210,center.Y+5,center.X+size.Width/2-100,center.Y+5+32),el,GUI_ID_CANCEL_BUTTON,L"Cancel");  guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-100,center.Y+5,center.X+size.Width/2,center.Y+5+32),el,GUI_ID_OK_BUTTON,L"Open");  guienv->setFocus(fileField);  device->setWindowCaption(L"Hyper Maze: Open");}bool OpenGui::run(){  if(cancelClicked)	return false;  if(okClicked){	irr::IReadFile* in=createAndOpen(device->getFileSystem(),fileField->getText());	if(!in){          okClicked=false;          el->setVisible(false);          ErrorGui eg;          if(device->getFileSystem()->existFile(fileField->getText()))            eg.error(device,fm,L"Error Opening File","File exists but can't be opened.");          else            eg.error(device,fm,L"Error Opening File","File doesn't exist.");          el->setVisible(true);          device->getGUIEnvironment()->setFocus(fileField);	  return true;        }	IrrHypIStream is(in);	in->drop();	bool status = read(is,pd->m).ok;	pd->sc=Script();// reset it to blank as a default	status &= read(is,pd->sc).ok;	if(!status){          el->setVisible(false);          ErrorGui eg;          eg.error(device,fm,L"Error Reading File","The level may have been loaded but it may have errors. If it not correct please try again or get a new copy of the level.");          el->setVisible(true);  }  return false;  }  return true;}bool WinGui::OnEventImpl(const irr::SEvent &event){  if(event.EventType == irr::EET_GUI_EVENT){	if(event.GUIEvent.EventType==irr::gui::EGET_BUTTON_CLICKED){	  switch(event.GUIEvent.Caller->getID()){		case GUI_ID_OK_BUTTON:		  okClicked=true;		  return true;		case GUI_ID_GENERATE_BUTTON:		  generateClicked=true;		  return true;		case GUI_ID_NEXT_BUTTON:		  nextClicked=true;		  return true;		case GUI_ID_LOAD_BUTTON:		  loadClicked=true;		  return true;		case GUI_ID_SAVE_BUTTON:		  saveClicked=true;		  return true;	  }	  return false;	}	if(event.GUIEvent.EventType == irr::gui::EGET_EDITBOX_ENTER){	  okClicked=true;	  return true;	}  }  if(event.EventType == irr::EET_KEY_INPUT_EVENT && event.KeyInput.Key==irr::KEY_ESCAPE &&	  (keyblock==0||device->getTimer()->getTime()>keyblock)){	okClicked=true;	keyblock=0;	return true;  }  return false;};bool WinGui::won(irr::IrrlichtDevice* _device,FontManager* _fm,PuzzleDisplay& pd,const Message& m, Pair<SPA<const char> > nextLevel){  this->pd=&pd;  this->m=m;  this->nextLevel=nextLevel;  main(_device,_fm);  return true;}void WinGui::createGUI(){  okClicked=nextClicked=generateClicked=loadClicked=saveClicked=false;  keyblock=0;  irr::IVideoDriver* driver = device->getVideoDriver();  irr::IGUIEnvironment *guienv = device->getGUIEnvironment();  irr::rect<irr::s32> rect=driver->getViewPort();  irr::position2d<irr::s32> center=rect.getCenter();  irr::dimension2d<irr::s32> size=rect.getSize();  size.Width=min(600,size.Width-10);  size.Height=min(600,size.Height-10);  GUIFormattedText* text=new GUIFormattedText(L"Congratulations!",guienv,el,0,irr::rect<irr::s32>(center.X-size.Width/2,center.Y-size.Height/2,center.X+size.Width/2,center.Y+size.Height/2-10-32-10-32),true,true);  text->setOverrideFont(0,fm->getFont(24,true));  text->setAllTextAlignment(irr::EGUIA_CENTER,irr::EGUIA_CENTER);  text->addText((irr::stringw(L"Score: ")+=pd->sp.getScore()).c_str());  addMessageToElement(text,fm,m);  text->addText(L"\nWhat would you like to do now?");  text->drop();    irr::IGUIButton* def=guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-320,center.Y+size.Height/2-32-32-10,center.X+size.Width/2-130,center.Y+size.Height/2-32-10),el,GUI_ID_OK_BUTTON,L"Back to Current Maze");  guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-120,center.Y+size.Height/2-32-32-10,center.X+size.Width/2,center.Y+size.Height/2-32-10),el,GUI_ID_SAVE_BUTTON,L"Save Maze");  guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-120,center.Y+size.Height/2-32,center.X+size.Width/2,center.Y+size.Height/2),el,GUI_ID_LOAD_BUTTON,L"Load a Maze");  guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-320,center.Y+size.Height/2-32,center.X+size.Width/2-130,center.Y+size.Height/2),el,GUI_ID_GENERATE_BUTTON,L"Generate New Maze");  if(!nextLevel.a.isnull()){    irr::stringw label(L"Next Level");    if(!nextLevel.b.isnull()){      label.append(L": ");      label.append(irr::stringw(&*nextLevel.b));    }    def=guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-600,center.Y+size.Height/2-32,center.X+size.Width/2-330,center.Y+size.Height/2),el,GUI_ID_NEXT_BUTTON,label.c_str());    def->setToolTipText(irr::stringw(&*nextLevel.a).c_str());  }  guienv->setFocus(def);  device->setWindowCaption(L"Hyper Maze: Congratulations");}bool WinGui::run(){  if(okClicked){	return false;  }  if(nextClicked){    nextClicked=false;    irr::IReadFile* in=createAndOpen(device->getFileSystem(),&*nextLevel.a);    if(!in)      return true;    IrrHypIStream is(in);    in->drop();    read(is,pd->m);    pd->sc=Script();    read(is,pd->sc);    return false;  }  if(saveClicked){	el->setVisible(false);	SaveGui sg;	sg.save(device,fm,*pd);	el->setVisible(true);	saveClicked=false;	keyblock=device->getTimer()->getTime()+500;  }  if(loadClicked){	el->setVisible(false);	OpenGui og;	if(og.open(device,fm,*pd)){	  pd->mazeUpdated();	  return false;	}	el->setVisible(true);	loadClicked=false;	keyblock=device->getTimer()->getTime()+500;  }  if(generateClicked){	el->setVisible(false);	GenerateGui gg;	if(gg.generate(device,fm,*pd)){	  pd->mazeUpdated();	  return false;	}	el->setVisible(true);	generateClicked=false;	keyblock=device->getTimer()->getTime()+500;  }  return true;}bool MessageGui::OnEventImpl(const irr::SEvent &event){  if(event.EventType == irr::EET_GUI_EVENT && event.GUIEvent.EventType==irr::gui::EGET_BUTTON_CLICKED)    if(event.GUIEvent.Caller->getID()==GUI_ID_OK_BUTTON){      okClicked=true;      return true;  }  if(event.EventType == irr::EET_KEY_INPUT_EVENT && event.KeyInput.Key==irr::KEY_ESCAPE){	okClicked=true;	return true;  }  return false;};bool MessageGui::message(irr::IrrlichtDevice* _device,FontManager* _fm,const Message& m){  this->m=m;  main(_device,_fm);  return okClicked;}void MessageGui::createGUI(){  okClicked=false;  irr::IVideoDriver* driver = device->getVideoDriver();  irr::IGUIEnvironment *guienv = device->getGUIEnvironment();  irr::rect<irr::s32> rect=driver->getViewPort();  irr::position2d<irr::s32> center=rect.getCenter();  irr::dimension2d<irr::s32> size=rect.getSize();  size.Width=min(400,size.Width-10);  size.Height=min(600,size.Height-10);  makeElementFromMessage(guienv,fm,el,irr::rect<irr::s32>(center.X-size.Width/2,center.Y-size.Height/2,center.X+size.Width/2,center.Y+size.Height/2-10-32),m);  guienv->setFocus(guienv->addButton(irr::rect<irr::s32>(center.X+size.Width/2-120,center.Y+size.Height/2-32,center.X+size.Width/2,center.Y+size.Height/2),el,GUI_ID_OK_BUTTON,L"Ok"));  device->setWindowCaption(L"Hyper Maze: Message");}bool MessageGui::run(){  if(okClicked){    return false;  }  return true;}
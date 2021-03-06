/**
 * @file GUIFormattedText.cc
 * @brief Implementation of GUIFormattedText.hh
 */

#include "GUIFormattedText.hh"
namespace irr{
  using namespace gui;
  using namespace core;
}

GUIFormattedText::GUIFormattedText(const wchar_t* text, irr::IGUIEnvironment* environment,
        irr::IGUIElement* parent, irr::s32 id, const irr::rect<irr::s32>& rectangle,
        bool background, bool border):
        irr::IGUIElement(EGUIET_FORMATTED_TEXT,environment,parent,id,rectangle),
        paragraphs(),layoutNeeded(true),lastLayoutSkinFont(0),defaultOverrideFont(0),
        defaultTextAlignmentHorizontal(irr::EGUIA_CENTER),defaultTextAlignmentVertical(irr::EGUIA_CENTER),
        background(background),border(border){
  setText(text);
}

GUIFormattedText::~GUIFormattedText(){
  while(Children.begin()!=Children.end()){
    (*Children.begin())->remove();
  }
  paragraphs.clear();
  if(defaultOverrideFont)
    defaultOverrideFont->drop();
  if(lastLayoutSkinFont)
    lastLayoutSkinFont->drop();
}

void GUIFormattedText::draw(){
  irr::IGUISkin* skin = Environment->getSkin();
  if(layoutNeeded)
    layout();
  else{
    if (skin && skin->getFont()!=lastLayoutSkinFont)
      layout();
  }
  if (skin){
    irr::EGUI_DEFAULT_COLOR bgCol = irr::EGDC_WINDOW;

    if (!border && background)
    {
      skin->draw2DRectangle(this, skin->getColor(bgCol), AbsoluteRect, &AbsoluteClippingRect);
    }
    if (border)
    {
      skin->draw3DSunkenPane(this, skin->getColor(bgCol), false, background, AbsoluteRect, &AbsoluteClippingRect);
    }
  }

  IGUIElement::draw();
}

void GUIFormattedText::setText( const wchar_t* text){
  // delete all children
  while(Children.begin()!=Children.end()){
    (*Children.begin())->remove();
  }
  paragraphs.clear();
  if(text)
    addText(text);
}

int GUIFormattedText::addText(const wchar_t* text,int i){
  if(i<0)
    i+=paragraphs.size()+1;
  if(i<0 || (unsigned int)i>paragraphs.size())
    return -1;
  irr::IGUIStaticText* par=Environment->addStaticText(text,
      irr::rect<irr::s32>(irr::position2di(0,0),RelativeRect.getSize()),false,true,this,-1,false);
  if(!par)
    return -1;
  par->setOverrideFont(defaultOverrideFont);
  par->setTextAlignment(defaultTextAlignmentHorizontal,defaultTextAlignmentVertical);
  if(i==0)
    paragraphs.push_front(par);
  else if(i==paragraphs.size())
    paragraphs.push_back(par);
  else
    paragraphs.insert_before(paragraphs.begin()+i,par);
  layoutNeeded=true;
  return i;
}

void GUIFormattedText::removeText(int i){
  if(i<0)
    i+=paragraphs.size();
  if(i<0 || (unsigned int)i>=paragraphs.size())
    return;
  irr::list<irr::IGUIStaticText*>::Iterator it=paragraphs.begin()+i;
  (*it)->remove();
  paragraphs.erase(it);
  layoutNeeded=true;
}

void GUIFormattedText::removeAllButText(int i){
  if(i<0)
    i+=paragraphs.size();
  if(i<0 || (unsigned int)i>=paragraphs.size())
    return;
  irr::list<irr::IGUIStaticText*>::Iterator leaveit=paragraphs.begin()+i;
  irr::IGUIStaticText* leave=*leaveit;
  for(irr::list<irr::IGUIStaticText*>::Iterator it=paragraphs.begin();it!=paragraphs.end();++it)
    if(it!=leaveit)
      (*it)->remove();
  paragraphs.clear();
  paragraphs.push_back(leave);
  layoutNeeded=true;
}

void GUIFormattedText::clearText(){
  for(irr::list<irr::IGUIStaticText*>::Iterator it=paragraphs.begin();it!=paragraphs.end();++it)
    (*it)->remove();
  paragraphs.clear();
  layoutNeeded=true;
}

unsigned int GUIFormattedText::paragraphCount(){
  return paragraphs.size();
}

void GUIFormattedText::setOverrideFont(int i,irr::IGUIFont* font){
  if(i<0)
    i+=paragraphs.size();
  if(i<0 || (unsigned int)i>=paragraphs.size())
    return;
  irr::list<irr::IGUIStaticText*>::Iterator it=paragraphs.begin()+i;
  (*it)->setOverrideFont(font);
  layoutNeeded=true;
}

void GUIFormattedText::setDefaultOverrideFont(irr::IGUIFont * font){
  if(defaultOverrideFont)
    defaultOverrideFont->drop();
  defaultOverrideFont=font;
  if(defaultOverrideFont)
    defaultOverrideFont->grab();
}

void GUIFormattedText::setAllOverrideFont(irr::IGUIFont * font){
  setDefaultOverrideFont(font);
  irr::list<irr::IGUIStaticText*>::Iterator it = paragraphs.begin();
  for (; it != paragraphs.end(); ++it)
    (*it)->setOverrideFont(font);
  layoutNeeded=true;
}

irr::IGUIFont* GUIFormattedText::getOverrideFont(int i){
  if(i<0)
    i+=paragraphs.size();
  if(i<0 || (unsigned int)i>=paragraphs.size())
    return 0;
  irr::list<irr::IGUIStaticText*>::Iterator it=paragraphs.begin()+i;
  return (*it)->getOverrideFont();
}

irr::IGUIFont* GUIFormattedText::getDefaultOverrideFont(){
  return defaultOverrideFont;
}

irr::IGUIFont* GUIFormattedText::getActiveFont(int i){
  irr::IGUIFont* font=getOverrideFont(i);
  if ( font )
    return font;
  irr::IGUISkin* skin = Environment->getSkin();
  if (skin)
    return skin->getFont();
  return 0;
}

irr::IGUIFont* GUIFormattedText::getDefaultActiveFont(){
  if ( defaultOverrideFont )
    return defaultOverrideFont;
  irr::IGUISkin* skin = Environment->getSkin();
  if (skin)
    return skin->getFont();
  return 0;
}

void GUIFormattedText::setOverrideColor (int i,irr::video::SColor color){
  if(i<0)
    i+=paragraphs.size();
  if(i<0 || (unsigned int)i>=paragraphs.size())
    return;
  irr::list<irr::IGUIStaticText*>::Iterator it=paragraphs.begin()+i;
  (*it)->setOverrideColor(color);
}

void GUIFormattedText::enableOverrideColor (int i,bool enable){
  if(i<0)
    i+=paragraphs.size();
  if(i<0 || (unsigned int)i>=paragraphs.size())
    return;
  irr::list<irr::IGUIStaticText*>::Iterator it=paragraphs.begin()+i;
  (*it)->enableOverrideColor(enable);
}

bool GUIFormattedText::isOverrideColorEnabled (int i){
  if(i<0)
    i+=paragraphs.size();
  if(i<0 || (unsigned int)i>=paragraphs.size())
    return false;
  irr::list<irr::IGUIStaticText*>::Iterator it=paragraphs.begin()+i;
  return (*it)->isOverrideColorEnabled();
}


irr::video::SColor GUIFormattedText::getOverrideColor (int i) const{
  if(i<0)
    i+=paragraphs.size();
  if(i<0 || (unsigned int)i>=paragraphs.size())
    return irr::video::SColor(101,255,255,255);
  irr::list<irr::IGUIStaticText*>::ConstIterator it=paragraphs.begin()+i;
  return (*it)->getOverrideColor();
}


void GUIFormattedText::setTextAlignment(int i,irr::EGUI_ALIGNMENT horizontal, irr::EGUI_ALIGNMENT vertical){
  if(i<0)
    i+=paragraphs.size();
  if(i<0 || (unsigned int)i>=paragraphs.size())
    return;
  irr::list<irr::IGUIStaticText*>::Iterator it=paragraphs.begin()+i;
  (*it)->setTextAlignment(horizontal,vertical);
}

void GUIFormattedText::setDefaultTextAlignment(irr::EGUI_ALIGNMENT horizontal, irr::EGUI_ALIGNMENT vertical){
  defaultTextAlignmentHorizontal=horizontal;
  defaultTextAlignmentVertical=vertical;
}

void GUIFormattedText::setAllTextAlignment(irr::EGUI_ALIGNMENT horizontal, irr::EGUI_ALIGNMENT vertical){
  setDefaultTextAlignment(horizontal,vertical);
  irr::list<irr::IGUIStaticText*>::Iterator it = paragraphs.begin();
  for (; it != paragraphs.end(); ++it)
    (*it)->setTextAlignment(horizontal,vertical);
}

void GUIFormattedText::setDrawBackground(bool draw){
  background = draw;
}


void GUIFormattedText::setDrawBorder(bool draw){
  border = draw;
}

void GUIFormattedText::layout(){
  irr::list<irr::IGUIStaticText*>::Iterator it = paragraphs.begin();
  irr::s32 totalHeight=0;
  for (; it != paragraphs.end(); ++it)
    totalHeight+=(*it)->getTextHeight();
  irr::rect<irr::s32> rect=irr::rect<irr::s32>(irr::position2di(0,0),RelativeRect.getSize());
  if (defaultTextAlignmentVertical == irr::EGUIA_CENTER){
    rect.UpperLeftCorner.Y = rect.getCenter().Y - (totalHeight / 2);
  }else if (defaultTextAlignmentVertical == irr::EGUIA_LOWERRIGHT){
    rect.UpperLeftCorner.Y = rect.LowerRightCorner.Y - totalHeight;
  }
  for (it = paragraphs.begin(); it != paragraphs.end(); ++it){
    irr::s32 height=(*it)->getTextHeight();
    rect.LowerRightCorner.Y=rect.UpperLeftCorner.Y+height;
    (*it)->setRelativePosition(rect);
    rect.UpperLeftCorner.Y+=height+GAP;
  }
  layoutNeeded=false;
  irr::IGUISkin* skin = Environment->getSkin();
  if (skin){
    if(lastLayoutSkinFont)
      lastLayoutSkinFont->drop();
    lastLayoutSkinFont=skin->getFont();
    lastLayoutSkinFont->grab();
  }
}


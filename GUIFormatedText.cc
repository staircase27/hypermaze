#include "GUIFormatedText.hh"

GUIFormattedText::GUIFormattedText(const wchar_t* text, bool border, irr::IGUIEnvironment* environment,
        irr::IGUIElement* parent, irr::s32 id, const irr::rect<irr::s32>& rectangle, bool background):
        irr::IGUIElement(EGUIET_FORMATTED_TEXT,environment,parent,id,rectangle),
        paragraphs(),layoutNeeded(true),lastLayoutSkinFont(0),defaultOverrideFont(0),
        defaultTextAlignmentHorizontal(irr::EGUIA_CENTER),defaultTextAlignmentVertical(irr::EGUIA_CENTER){
  setText(text);
}

GUIFormattedText::~GUIFormattedText(){
  if(defaultOverrideFont)
    defaultOverrideFont->drop();
}
    
void GUIFormattedText::draw(){
  if(layoutNeeded)
    layout();
  else{
   	irr::IGUISkin* skin = Environment->getSkin();
  	if (skin)
      if(skin->getFont()!=lastLayoutSkinFont)
        layout();
  }
  IGUIElement::draw();
}
    
void GUIFormattedText::setText( const wchar_t* text){
  // delete all childrenparagraphs.size()
  while(Children.begin()!=Children.end()){
    (*Children.begin())->remove();
  }
  paragraphs.clear();
  addText(text);
}

int GUIFormattedText::addText(const wchar_t* text,int i){
  if(i==-1)
    i=paragraphs.size();
  if(i<0 || i>paragraphs.size())
    return -1;
  irr::IGUIStaticText* par=Environment->addStaticText(text,
      irr::rect<irr::s32>(irr::position2di(0,0),RelativeRect.getSize()),false,true,this,-1,false);
  if(!par)
    return -1;
  par->setOverrideFont(defaultOverrideFont);
  par->setTextAlignment(defaultTextAlignmentHorizontal,defaultTextAlignmentVertical);
  paragraphs.insert_before(paragraphs.begin()+i,par);
  layoutNeeded=true;
  return i;
}
    
void GUIFormattedText::removeText(int i){
  if(i<0 || i>=paragraphs.size())
    return;
  irr::list<irr::IGUIStaticText*>::Iterator it=paragraphs.begin()+i;
  (*it)->remove();
  paragraphs.erase(it);
  layoutNeeded=true;
}
    
void GUIFormattedText::setOverrideFont(int i,irr::IGUIFont* font){
  if(i<0 || i>=paragraphs.size())
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
  if(i<0 || i>=paragraphs.size())
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
     
void GUIFormattedText::setTextAlignment(int i,irr::EGUI_ALIGNMENT horizontal, irr::EGUI_ALIGNMENT vertical){
  if(i<0 || i>=paragraphs.size())
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

void GUIFormattedText::layout(){
  irr::s32 pos=0;
  irr::list<irr::IGUIStaticText*>::Iterator it = paragraphs.begin();
  for (; it != paragraphs.end(); ++it){
    irr::s32 height=(*it)->getTextHeight();
    irr::rect<irr::s32> rect=irr::rect<irr::s32>(irr::position2di(0,0),RelativeRect.getSize());
    rect.UpperLeftCorner.Y-=pos;
    rect.LowerRightCorner.Y=rect.UpperLeftCorner.Y-height;
    
    pos+=height+10;
  }
}


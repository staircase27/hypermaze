/* 
 * File:   FontManager.cc
 * Author: armstrong
 * 
 * Created on December 5, 2014, 10:14 AM
 */

#include "fonts.hh"

#ifdef IOSTREAM
#include <iostream>
using namespace std;
#endif

FontFamily* VirtualFontSet::resolve(FontManager& m){
  return m.getFontFamily(realname);
}
void FontManager::load(irr::stringw font){
#ifdef IOSTREAM
  cout<<"loading font: "<<font.c_str()<<endl;
#endif
  fonts.set(font,0);
}
FontManager::~FontManager(){
  for(irr::map<irr::stringw,FontSet*>::Iterator it=fonts.getIterator();
          !it.atEnd();it++){
    delete it->getValue();
  }
}
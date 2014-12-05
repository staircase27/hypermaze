/* 
 * File:   FontManager.hh
 * Author: armstrong
 *
 * Created on December 5, 2014, 10:14 AM
 */

#include "irrlicht.h"

#ifndef FONTMANAGER_HH
#define	FONTMANAGER_HH

namespace irr{
  using namespace core;
}

class FontFamily;
class FontManager;

class FontSet{
  public:
    virtual FontFamily* resolve(FontManager&)=0;
    virtual ~FontSet()=0;
};

class FontFamily: public FontSet{
  private:
    irr::map<int,irr::gui::IGUIFont*> regularfonts;
    irr::map<int,irr::gui::IGUIFont*> boldfonts;
    irr::map<int,irr::gui::IGUIFont*> italicfonts;
    irr::map<int,irr::gui::IGUIFont*> bolditalicfonts;
    
    inline irr::map<int,irr::gui::IGUIFont*>& getfonts(bool bold,bool italic){
      if(bold)
        if(italic)
          return bolditalicfonts;
        else
          return boldfonts;
      else
        if(italic)
          return italicfonts;
        else
          return regularfonts;
    }
  
  public:
    FontFamily* resolve(FontManager&){
      return this;
    }
    irr::gui::IGUIFont* getFont(int size,bool bold=false,bool italic=false){
      irr::map<int,irr::gui::IGUIFont*>& fonts=getfonts(bold,italic);
      irr::map<int,irr::gui::IGUIFont*>::Node n=fonts.getRoot();
      while()
      return 0;
    }
    
    ~FontFamily(){};
};

class VirtualFontSet{
  private:
    irr::stringw realname;
  public:
    VirtualFontSet(irr::stringw name){
      
    }
    
    FontFamily* resolve(FontManager& m);
    ~VirtualFontSet(){}
};

class FontManager {
  private:
    irr::map<irr::stringw,FontSet*> fonts;

    void load(irr::stringw font);

  public:
    FontManager(){};
    ~FontManager();
    

    inline FontFamily* getFontFamily(irr::stringw font){
      irr::map<irr::stringw,FontSet*>::Node* n=fonts.find(font);
      if(n==0){
        load(font);
        n=fonts.find(font);
      }
      if(n==0 || n->getValue()==0)
        return 0;
      else
        return n->getValue()->resolve(*this);
    }
    
  

};

#endif	/* FONTMANAGER_HH */


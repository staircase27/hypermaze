/*
 * File:   FontManager.hh
 * Author: armstrong
 *
 * Created on December 5, 2014, 10:14 AM
 */

#include "irrlicht.h"

#ifndef FONTMANAGER_HH
#define	FONTMANAGER_HH

#if IRRLICHT_VERSION_MAJOR<=1 || IRRLICHT_VERSION_MINOR<=9
#define IRRFONTFIX
#endif

#ifdef IRRFONTFIX
extern const char* font_prefix;
#endif

class FontFamily;
class FontManager;

class FontSet{
  public:
    virtual FontFamily* resolve(FontManager&)=0;
    virtual ~FontSet(){};
};

class FontFamily: public FontSet{
  private:
    irr::core::map<int,irr::io::path> regularfonts;
    irr::core::map<int,irr::io::path> boldfonts;
    irr::core::map<int,irr::io::path> italicfonts;
    irr::core::map<int,irr::io::path> bolditalicfonts;

    irr::core::map<int,irr::io::path>& getfonts(bool bold,bool italic);

  public:
    FontFamily* resolve(FontManager&){
      return this;
    }
    irr::io::path getFontName(int size,bool bold=false,bool italic=false);

    bool addFont(irr::io::path fontname,int size,bool bold=false,bool italic=false);

    ~FontFamily(){};
};

class VirtualFontSet:public FontSet{
  private:
    irr::core::stringc realname;
  public:
    VirtualFontSet(irr::core::stringc name):realname(name){}
    FontFamily* resolve(FontManager& m);
    ~VirtualFontSet(){}
};

class FontManager {
  private:
    irr::io::IFileSystem* fs;
    irr::gui::IGUIEnvironment* gui;

    irr::core::map<irr::core::stringc,FontSet*> fonts;

    void load(irr::core::stringc font);

  public:
    FontManager(irr::io::IFileSystem* fs,irr::gui::IGUIEnvironment* gui):fs(fs),gui(gui){fs->grab();gui->grab();};
    ~FontManager();


    inline FontFamily* getFontFamily(irr::core::stringc font);

    inline irr::gui::IGUIFont* getFont(int size,bool bold=false,bool italic=false){
      return getFont("default",size,bold,italic);
    }
    irr::gui::IGUIFont* getFont(irr::core::stringc font,int size,bool bold=false,bool italic=false);


};

#endif	/* FONTMANAGER_HH */


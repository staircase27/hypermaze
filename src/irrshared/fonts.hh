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

namespace irr{
  using namespace core;
  using namespace io;
  using namespace gui;
}

class FontFamily;
class FontManager;

class FontSet{
  public:
    virtual FontFamily* resolve(FontManager&)=0;
    virtual ~FontSet(){};
};

class FontFamily: public FontSet{
  private:
    irr::map<int,irr::path> regularfonts;
    irr::map<int,irr::path> boldfonts;
    irr::map<int,irr::path> italicfonts;
    irr::map<int,irr::path> bolditalicfonts;

    irr::map<int,irr::path>& getfonts(bool bold,bool italic);

  public:
    FontFamily* resolve(FontManager&){
      return this;
    }
    irr::path getFontName(int size,bool bold=false,bool italic=false);

    bool addFont(irr::path fontname,int size,bool bold=false,bool italic=false);

    ~FontFamily(){};
};

class VirtualFontSet:public FontSet{
  private:
    irr::stringc realname;
  public:
    VirtualFontSet(irr::stringc name):realname(name){}
    FontFamily* resolve(FontManager& m);
    ~VirtualFontSet(){}
};

class FontManager {
  private:
    irr::IFileSystem* fs;
    irr::IGUIEnvironment* gui;

    irr::map<irr::stringc,FontSet*> fonts;

    void load(irr::stringc font);

  public:
    FontManager(irr::IFileSystem* fs,irr::IGUIEnvironment* gui):fs(fs),gui(gui){fs->grab();gui->grab();};
    ~FontManager();


    inline FontFamily* getFontFamily(irr::stringc font);

    inline irr::IGUIFont* getFont(int size,bool bold=false,bool italic=false){
      return getFont("default",size,bold,italic);
    }
    irr::IGUIFont* getFont(irr::stringc font,int size,bool bold=false,bool italic=false);


};

#endif	/* FONTMANAGER_HH */


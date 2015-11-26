/**
 * @file fonts.hh
 * @brief The font management system for hypermaze
 *
 * Fonts are stored in the irrlicht/fonts/ directory in the form of xml files.
 * See FontManager::load for details. For irrlicht version < 1.9
 * a work round is needed as they can't load fonts from virtual filesystems correctly.
 * If the workround is needed then IRRFONTFIX is defined by this header. For this to
 * work the code that mounts the virtual filesystem must fill in the font_prefix variable.
 *
 */
#include "irrlicht.h"

#ifndef FONTMANAGER_HH
#define	FONTMANAGER_HH

#if IRRLICHT_VERSION_MAJOR<=1 || IRRLICHT_VERSION_MINOR<=9
#define IRRFONTFIX
#endif

#ifdef IRRFONTFIX
/// The location that all the fonts are stored under
/**
 * Used to work around a bug in the font loading code that means you can't load from a virtual filesystem.
 * It should be set to the path to the virtual filesystem
 */
extern const char* font_prefix;
#endif

class FontFamily;
class FontManager;

/// An object that resolves to a concrete FontFamily
class FontSet{
  public:
    ///Resolve this FontSet to it's concrete FontFamily
    /**
     * @param fm the font manager that is used to resolve this FontSet
     * @return a concrete FontFamily or Null if it can't resolve
     */
    virtual FontFamily* resolve(FontManager& fm)=0;
    ///virtual destuctor so concrete implementations can tidy up when deleted.
    virtual ~FontSet(){};
};
/// A concrete font family i.e. a set of version of the same font for different styles and shapes.
class FontFamily: public FontSet{
  private:
    irr::core::map<int,irr::io::path> regularfonts;///< Map of size to path for regular fonts
    irr::core::map<int,irr::io::path> boldfonts;///< Map of size to path for bold fonts
    irr::core::map<int,irr::io::path> italicfonts;///< Map of size to path for italic fonts
    irr::core::map<int,irr::io::path> bolditalicfonts;///< Map of size to path for bold italic fonts

    /// Get the relevent size to path map for the type of fonts requested
    /**
     * @param bold should the font be bold
     * @param italic should the font be italic
     * @return the relevent font map
     */
    irr::core::map<int,irr::io::path>& getfonts(bool bold,bool italic);

  public:
    /**
     * @copydoc FontSet::resolve
     * Resolves to itself as it's already a Font Family
     */
    FontFamily* resolve(FontManager& fm){
      return this;
    }
    /// Get the path for the font requested.
    /**
     * If the exact size requested isn't available the best match is provided.
     * If no fonts of the relevent type are available this may return the empty path.
     * @param size the size of font requested
     * @param bold if the font should be bold
     * @param italic if the font should be italic
     * @return the path to the best match font
     */
    irr::io::path getFontName(int size,bool bold=false,bool italic=false);

    ///Add a font to this family
    /**
     * @param fontname the path to the new font
     * @param size the size of the new font
     * @param bold if the new font is bold
     * @param italic if the new font is italic
     * @return true if a new font was actually added
     */
    bool addFont(irr::io::path fontname,int size,bool bold=false,bool italic=false);

    ///Destroy this font family and all data it contains
    ~FontFamily(){};
};

/// A FontSet that is an alias to another FontSet.
class VirtualFontSet:public FontSet{
  private:
    irr::core::stringc realname; ///< The name of the font this points to
  public:
    ///Construct a new virtual font set
    /**
     * @param name the name this should point to
     */
    VirtualFontSet(irr::core::stringc name):realname(name){}
    /// Use the font manager to resolve the pointed to name to a concrete font.
    /**
     * @param m the font manager to resolve this alias using
     * @return the FontFamily this resolves to or Null
     */
    FontFamily* resolve(FontManager& m);
    ///Destroy this virtual font set and all data it contains
    ~VirtualFontSet(){}
};

/// An object that manages font data.
/**
 * This is the main used class for the font system. This class takes a request for a
 * font and resolves it to an actual font and then loads the font and returns it.
 */
class FontManager {
  private:
    irr::io::IFileSystem* fs; ///< The file system to use to find fonts
    irr::gui::IGUIEnvironment* gui; ///< The Gui environment used to load fonts

    irr::core::map<irr::core::stringc,FontSet*> fonts; ///< A map of font names to actual fonts

    /// Load a font set from the filesystem
    /**
     * @param font the name of the font to load
    */
    void load(irr::core::stringc font);

  public:
    ///Construct a FontManager that uses the specified filesystem and gui to find and load fonts
    /**
     * @param fs the filesyetem to find fonts with
     * @param gui the gui environment used to load fonts
     */
    FontManager(irr::io::IFileSystem* fs,irr::gui::IGUIEnvironment* gui);
    /// Destry the font manager and all data in it
    ~FontManager();

    /// Get the FontFamily for a name
    /**
     * @warning fonts may not be possible to load except via the getFont methods below
     * @param font the name of the font to load
     * @return the FontFamily if available or Null
     */
    inline FontFamily* getFontFamily(irr::core::stringc font);

    /// A convinience function to get a default font quickly
    /**
     * @param size the size of font to get
     * @param bold if a bold font is requested
     * @param italic if an italic font is requested
     * @return the font is available or Null
     */
    inline irr::gui::IGUIFont* getFont(int size,bool bold=false,bool italic=false){
      return getFont("default",size,bold,italic);
    }

    ///Get a font for the given name, size and type.
    /**
     * This resolves any virtual fonts and gets a concrete family, loading any that aren't
     * loaded, finds the best match and requests it from the gui environment which will
     * load them if not already loaded.
     * @param font the name of the font to get
     * @param size the size of font to get
     * @param bold if a bold font is requested
     * @param italic if an italic font is requested
     * @return the font is available or Null
     */
    irr::gui::IGUIFont* getFont(irr::core::stringc font,int size,bool bold=false,bool italic=false);


};

#endif	/* FONTMANAGER_HH */


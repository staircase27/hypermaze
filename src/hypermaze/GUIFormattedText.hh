#ifndef GUIFormatted_TEXT_HH__INC
#define GUIFormatted_TEXT_HH__INC

#include "irrlicht.h"

namespace irr{
  using namespace core;
  using namespace scene;
  using namespace io;
  using namespace video;
  using namespace gui;
};

#define EGUIET_FORMATTED_TEXT ((irr::gui::EGUI_ELEMENT_TYPE)(irr::EGUIET_COUNT+10))

class GUIFormattedText: irr::IGUIElement{

  protected:
    
    irr::list<irr::IGUIStaticText*> paragraphs;
    bool layoutNeeded;
    irr::IGUIFont* lastLayoutSkinFont;
    irr::IGUIFont* defaultOverrideFont;
    irr::EGUI_ALIGNMENT defaultTextAlignmentHorizontal;
    irr::EGUI_ALIGNMENT defaultTextAlignmentVertical;
    
    bool background;
    bool border;
    
    static const int GAP=10;
    
    void layout();
  
  public:
  
    GUIFormattedText(const wchar_t* text, irr::IGUIEnvironment* environment,
        irr::IGUIElement* parent, irr::s32 id, const irr::rect<irr::s32>& rectangle,
        bool background = true, bool border = true);

    virtual ~GUIFormattedText();
    
    virtual void draw();
    
    virtual void setText( const wchar_t* text);

    virtual int addText(const wchar_t* text,int i=-1);
    
    virtual void removeText(int i);
    
    virtual void setOverrideFont(int i=-1,irr::IGUIFont* font=0);
    
    virtual void setDefaultOverrideFont(irr::IGUIFont * font=0);

    virtual void setAllOverrideFont(irr::IGUIFont * font=0);
    
    virtual irr::IGUIFont* getOverrideFont(int i=-1);
    
    virtual irr::IGUIFont* getDefaultOverrideFont();
    
    virtual irr::IGUIFont* getActiveFont(int i=-1);

    virtual irr::IGUIFont* getDefaultActiveFont();
    
    virtual void setOverrideColor (int i,irr::SColor color);
    
    virtual void enableOverrideColor (int i,bool enable);
    
    virtual irr::SColor getOverrideColor (int i) const;
      
    virtual void setTextAlignment(int i,irr::EGUI_ALIGNMENT horizontal, irr::EGUI_ALIGNMENT vertical);

    virtual void setDefaultTextAlignment(irr::EGUI_ALIGNMENT horizontal, irr::EGUI_ALIGNMENT vertical);

    virtual void setAllTextAlignment(irr::EGUI_ALIGNMENT horizontal, irr::EGUI_ALIGNMENT vertical);
    
    virtual void setDrawBackground(bool draw);

    virtual void setDrawBorder(bool draw);

};
#endif


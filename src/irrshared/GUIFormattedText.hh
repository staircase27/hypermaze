#ifndef GUIFormatted_TEXT_HH__INC
#define GUIFormatted_TEXT_HH__INC

#include "irrlicht.h"

#define EGUIET_FORMATTED_TEXT ((irr::gui::EGUI_ELEMENT_TYPE)(irr::gui::EGUIET_COUNT+10))

class GUIFormattedText:public irr::gui::IGUIElement{

  protected:

    irr::core::list<irr::gui::IGUIStaticText*> paragraphs;
    bool layoutNeeded;
    irr::gui::IGUIFont* lastLayoutSkinFont;
    irr::gui::IGUIFont* defaultOverrideFont;
    irr::gui::EGUI_ALIGNMENT defaultTextAlignmentHorizontal;
    irr::gui::EGUI_ALIGNMENT defaultTextAlignmentVertical;

    bool background;
    bool border;

    static const int GAP=10;

    void layout();

  public:

    GUIFormattedText(const wchar_t* text, irr::gui::IGUIEnvironment* environment,
        irr::gui::IGUIElement* parent, irr::s32 id, const irr::core::rect<irr::s32>& rectangle,
        bool background = true, bool border = true);

    virtual ~GUIFormattedText();

    virtual void draw();

    virtual void setText( const wchar_t* text);

    virtual int addText(const wchar_t* text,int i=-1);

    virtual void removeText(int i);

    virtual void setOverrideFont(int i=-1,irr::gui::IGUIFont* font=0);

    virtual void setDefaultOverrideFont(irr::gui::IGUIFont * font=0);

    virtual void setAllOverrideFont(irr::gui::IGUIFont * font=0);

    virtual irr::gui::IGUIFont* getOverrideFont(int i=-1);

    virtual irr::gui::IGUIFont* getDefaultOverrideFont();

    virtual irr::gui::IGUIFont* getActiveFont(int i=-1);

    virtual irr::gui::IGUIFont* getDefaultActiveFont();

    virtual void setOverrideColor (int i,irr::video::SColor color);

    virtual void enableOverrideColor (int i,bool enable);

    virtual irr::video::SColor getOverrideColor (int i) const;

    virtual void setTextAlignment(int i,irr::gui::EGUI_ALIGNMENT horizontal, irr::gui::EGUI_ALIGNMENT vertical);

    virtual void setDefaultTextAlignment(irr::gui::EGUI_ALIGNMENT horizontal, irr::gui::EGUI_ALIGNMENT vertical);

    virtual void setAllTextAlignment(irr::gui::EGUI_ALIGNMENT horizontal, irr::gui::EGUI_ALIGNMENT vertical);

    virtual void setDrawBackground(bool draw);

    virtual void setDrawBorder(bool draw);

};
#endif


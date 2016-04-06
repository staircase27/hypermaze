/**
 * @file GUIFormattedText.hh
 * @brief A formatted text GUI Element for irrlicht.
 *
 * A GUI Element that contains multiple paragraphs of text each optionally
 * with their own colour, font and alignment and laying them out in the
 * space available.
 *
 */
#ifndef GUIFormatted_TEXT_HH__INC
#define GUIFormatted_TEXT_HH__INC

#include "irrlicht.h"

/// The ID for a Formatted Text GUI Element
#define EGUIET_FORMATTED_TEXT ((irr::gui::EGUI_ELEMENT_TYPE)(irr::gui::EGUIET_COUNT+10))

/// A formatted text GUI Element
/**
 * This GUI Element contains pargraphs each of which have their own
 * font, colour and alignment and lays them out in the space available
 * for this widget.
 */
class GUIFormattedText:public irr::gui::IGUIElement{

  protected:
    irr::core::list<irr::gui::IGUIStaticText*> paragraphs; ///< The actual paragraphs of text
    bool layoutNeeded; ///< Flag for if a layout is needed at next redraw
    irr::gui::IGUIFont* lastLayoutSkinFont; ///< The skin font at least layout
    irr::gui::IGUIFont* defaultOverrideFont; ///< The default override font (used to create new paragraphs)
    /// The default horizontal alignment (used to create new paragraphs)
    irr::gui::EGUI_ALIGNMENT defaultTextAlignmentHorizontal;
    /// The default vertical alignment (used to create new paragraphs and to layout
    /// the entire set of paragraphs in the space provided)
    irr::gui::EGUI_ALIGNMENT defaultTextAlignmentVertical;

    bool background; ///< If this element should show it's background
    bool border; ///< If this element should show it's border

    static const int GAP=10; ///< The gap between paragraphs

    /// Layout the paragraphs. Called automatically by draw when needed.
    void layout();

  public:
    /// Create a new formatted text GUI Element
    /**
     * @param text an optional initial paragraph (pass null for initially empty)
     * @param environment the GUI Environment that this should be created as part of
     * @param parent the parent for this GUI Element
     * @param id the id for this GUI Element
     * @param rectangle the location for this element with respect to it's parent
     * @param background if this element should draw it's background
     * @param border if this element should draw it's border
     */
    GUIFormattedText(const wchar_t* text, irr::gui::IGUIEnvironment* environment,
        irr::gui::IGUIElement* parent, irr::s32 id, const irr::core::rect<irr::s32>& rectangle,
        bool background = true, bool border = true);

    /// Destroy this element and release the resources used
    virtual ~GUIFormattedText();

    /// Draw this element.
    /**
     * If needed this relays out the paragraphs and then renders them each in turn.
     */
    virtual void draw();

    /// Set the text in this element
    /**
     * This deletes any current paragraphs and replaces with a single new paragraph.
     * Can also be used to clear the paragraphs by passing NULL for the new text
     * @param text the new text for the element or NULL to clear.
     */
    virtual void setText( const wchar_t* text);

    /// Add a new paragraph of text at the specified position
    /**
     * @param text the text of the new paragraph
     * @param i the index to insert the new paragraph at. Can be negative to index backwards from the end
     * @return the index of the paragraph. Can be used to access the paragraph later.
     */
    virtual int addText(const wchar_t* text,int i=-1);

    /// Remove a paragraph of text from the specified position
    /**
     * @param i the index of the paragraph to remove. Can be negative to index backwards from the end
     */
    virtual void removeText(int i);

    /// Remove all but the paragraph of text at the specified position
    /**
     * @param i the index of the paragraph to leave. Can be negative to index backwards from the end
     */
    virtual void removeAllButText(int i);

    /// Remove all text
    virtual void clearText();

    /// The count of paragraphs in this element
    /**
     * @return the number of paragraphs
     */
    virtual unsigned int paragraphCount();

    /// Set the override front for a specified paragraph
    /**
     * @param i the index of the paragraph to change. Can be negative to index backwards from the end
     * @param font the new override font. Can be NULL to reset to default font
     */
    virtual void setOverrideFont(int i=-1,irr::gui::IGUIFont* font=0);

    /// Set the default override front to use for new paragraphs
    /**
     * @param font the new override font. Can be NULL to reset to default font
     */
    virtual void setDefaultOverrideFont(irr::gui::IGUIFont * font=0);

    /// Set the override front for all paragraphs
    /**
     * @param font the new override font. Can be NULL to reset to default font
     */
    virtual void setAllOverrideFont(irr::gui::IGUIFont * font=0);

    /// Get the override front for a specified paragraph
    /**
     * @param i the index of the paragraph to query. Can be negative to index backwards from the end
     * @return the current override font. Can be NULL to show this paragraph uses the default font
     */
    virtual irr::gui::IGUIFont* getOverrideFont(int i=-1);

    /// Get the default override front to use for new paragraphs
    /**
     * @return the current default override font. Can be NULL to show new paragraphs use the default font
     */
    virtual irr::gui::IGUIFont* getDefaultOverrideFont();

    /// Get the front used to render a specified paragraph
    /**
     * @param i the index of the paragraph to query. Can be negative to index backwards from the end
     * @return the front used
     */
    virtual irr::gui::IGUIFont* getActiveFont(int i=-1);

    /// Get the default front used to render new paragraphs
    /**
     * @return the front used
     */
    virtual irr::gui::IGUIFont* getDefaultActiveFont();

    /// Set the override colour for a specified paragraph
    /**
     * @param i the index of the paragraph to change. Can be negative to index backwards from the end
     * @param color the new override colour
     */
    virtual void setOverrideColor (int i,irr::video::SColor color);

    /// Get the override colour for a specified paragraph
    /**
     * @param i the index of the paragraph to query. Can be negative to index backwards from the end
     * @return the current override colour
     */
    virtual irr::video::SColor getOverrideColor (int i) const;

    /// Set if the specified paragraph should render in the override colour of the skin colour
    /**
     * @param i the index of the paragraph to change. Can be negative to index backwards from the end
     * @param enable if the override colour should be enabled
     */
    virtual void enableOverrideColor (int i,bool enable);

    /// Get if the specified paragraph will render in the override colour of the skin colour
    /**
     * @param i the index of the paragraph to query. Can be negative to index backwards from the end
     * @return if the override colour should be enabled
     */
    virtual bool isOverrideColorEnabled (int i);

    /// Set the text alignment for the specified paragraph
    /**
     * @param i the index of the paragraph to change. Can be negative to index backwards from the end
     * @param horizontal the horizontal alignment
     * @param vertical the vertical aligntment
     */
    virtual void setTextAlignment(int i,irr::gui::EGUI_ALIGNMENT horizontal, irr::gui::EGUI_ALIGNMENT vertical);

    /// Set the default text alignment
    /**
     * The default alignment is used for new paragraphs and the overall horizontal alignment of the
     * paragraphs within this element's space
     * @param horizontal the horizontal alignment
     * @param vertical the vertical aligntment
     */
    virtual void setDefaultTextAlignment(irr::gui::EGUI_ALIGNMENT horizontal, irr::gui::EGUI_ALIGNMENT vertical);

    /// Set the text alignment used for all paragraphs
    /**
     * This sets the alignment for each existing paragraph and the default
     * @param horizontal the horizontal alignment
     * @param vertical the vertical aligntment
     */
    virtual void setAllTextAlignment(irr::gui::EGUI_ALIGNMENT horizontal, irr::gui::EGUI_ALIGNMENT vertical);

    /// Sets if the element should draw it's background
    /**
     * @param draw if the background should be drawn
     */
    virtual void setDrawBackground(bool draw);

    /// Sets if the element should draw it's border
    /**
     * @param draw if the border should be drawn
     */
    virtual void setDrawBorder(bool draw);

};
#endif


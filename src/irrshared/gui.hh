/**
 * @file gui.hh
 * @brief The GUIs for the hypermaze
 * This header defines a few different GUIElements and classes used for
 * the hypermaze guis and the hypermaze gui classes for each type of gui.
 */
#include "irrlicht.h"
#include "../irrshared/fonts.hh"

#ifndef GUI_HH_INC
#define GUI_HH_INC

/// A convenience function needed in generating guis. Returns the lower of the two arguments.
/**
 * @tparam T the type of the arguments. If they have the same type will be deduced.
 * @param a the first argument
 * @param b the second argument
 * @return b if b is less than a else a
 */
template <typename T> inline const T& min(const T& a,const T& b){
  return (b<a)?b:a;
}

/// The id for empty GUI element used to contain other elements
static const int MGUIET_EMPTY = irr::gui::EGUIET_COUNT+10;

/// An empty GUI element used to contains other elements
class CGUIEmptyElement : public irr::gui::IGUIElement
{
  public:
    /// Construct a new empty GUI element
    /**
     * @param environment the GUI environment that the element is attached to
     * @param parent the parent of the new element
     */
    CGUIEmptyElement(irr::gui::IGUIEnvironment* environment, irr::gui::IGUIElement* parent)
        :irr::gui::IGUIElement((irr::gui::EGUI_ELEMENT_TYPE)MGUIET_EMPTY, environment, parent,
                                -1, irr::core::rect<irr::s32>(0,0,100000,10000)) {}

    /// Get the name of this element
    /**
     * @return the const string "empty"
     */
    virtual const irr::c8* getTypeName() const   { return "empty"; }

    /// Check if a point is inside this element.
    /**
     * @param point the point to check
     * @return always returns false. Nothing should count as inside this element
     */
    virtual bool isPointInside(const irr::core::position2d<irr::s32>& point) const { return false; }

    /// Bring an element to the font
    /**
     * Overridden to also bring this to front of parent
     * @param element the element to bring to the front
     * @return True if successful, false if not.
     */
    virtual bool bringToFront(IGUIElement* element)
    {
      bool result = IGUIElement::bringToFront(element);
      if ( Parent )
      {
        Parent->bringToFront(this);
      }
      return result;
    }
};
/// Base class for guis.
/**
 * This class sets up a base element for all components of the gui to be placed in,
 * event receiver code and a main event loop to take over from the normal event loop
 */
class BaseGui : irr::IEventReceiver{
  protected:
    irr::IrrlichtDevice* device; ///< The irrlicht device. Needed
    FontManager* fm;
    irr::IEventReceiver* oldReceiver;
    CGUIEmptyElement* el;

    BaseGui():device(0),fm(0){};

    bool OnEvent(const irr::SEvent &event);
    virtual bool OnEventImpl(const irr::SEvent &event)=0;

    void apply(irr::IrrlichtDevice* _device);
    void unapply();

    virtual void createGUI()=0;

    virtual bool run()=0;

    void main(irr::IrrlichtDevice* _device,FontManager* _fm);

    virtual ~BaseGui(){};
};
#endif

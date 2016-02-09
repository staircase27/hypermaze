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
 * event receiver code and a main event loop to take over from the normal event loop.
 *
 * To use create a subclass that implements the virtual methods and
 * with a public method that the users call. This public method must store any parameters that
 * the gui needs then call main. Once main returns it should do any tidy up it needs and
 * then return any output you want to return.
 */
class BaseGui : private irr::IEventReceiver{
  private:
    irr::IrrlichtDevice* device; ///< The irrlicht device. Needed for access to irrlicht system
    FontManager* fm; ///< Used to access the fonts used by the gui
    /// The old event receiver. Stored so it can be restored at end and selected events can be forwarded on
    irr::IEventReceiver* oldReceiver;
    CGUIEmptyElement* el; /// The top level element that all components from this gui should be added to.

    /// Event receiver.
    /**
     * Passes key up and mouse button up events on to the original event receiver
     * (so they don't resume thinking keys are still down) and then pass events
     * on to OnEventImpl
     * @param event The event that needs processing.
     * @return True of the event was processed.
     */
    bool OnEvent(const irr::SEvent &event);

    /// Setup the gui on the specified device
    void apply(irr::IrrlichtDevice* _device);
    /// Remove the gui on the specified device
    void unapply();

  protected:

    /// Construct a new BaseGui. Called by child.
    BaseGui():device(0),fm(0){};

    /// Access the irrlicht device. Needed for access to irrlicht system
    /**
     * @return the current irrlicht device in use
     */
    inline irr::IrrlichtDevice* getDevice(){
      return device;
    }
    /// Access the FontManager which provides access to the fonts used by the gui
    /**
     * @return the current FontManager in use
     */
    inline FontManager* getFontManager(){
      return fm;
    }
    /// Access the top level element that all components from this gui should be added to.
    /**
     * @return this gui's top level element
     */
    inline CGUIEmptyElement* getTopElement(){
      return el;
    }

    /// Event receiver for the gui.
    /**
     * Function that should be implemented to process events.
     * @param event The event that needs processing.
     * @return True of the event was processed.
     */
    virtual bool OnEventImpl(const irr::SEvent &event)=0;

    /// Create the gui and setup all the elements
    /**
     * Should create all elements as descendants of el
     */
    virtual void createGUI()=0;

    /// Called one per event loop to do any processing the gui
    /// needs to do but was too long to do either regularly or
    /// was too long for the event receiver.
    /**
     * @return false if the gui is finished.
     */
    virtual bool run()=0;

    /// The main function that needs to be called to start the gui system
    /**
     * @param _device the irrlicht device so the gui can access the irrlicht gui systems
     * @param _fm the font manager to request fonts from
     */
    void main(irr::IrrlichtDevice* _device,FontManager* _fm);

    /// Virtual destructor so subclasses can override
    virtual ~BaseGui(){};
};
#endif

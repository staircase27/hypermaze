/**
 * @file keymap.hh
 * @brief The keymap for the hypermaze
 */
#include "irrlicht.h"
#include "../irrshared/platformcompat.hh"
#include "../core/dirns.hh"
#include <iostream>
#include <map>

#ifndef KEYMAP_HH_INC
#define KEYMAP_HH_INC

/// Structure to describe a key press
struct KeySpec{
  wchar_t chr; ///< The character corresponding to the key or 0 if there is no character for the key
  irr::EKEY_CODE key; ///< The keycode corresponding to the key if chr is 0 otherwise irr::KEY_KEY_CODES_COUNT

  bool shift; ///< is shift pressed
  bool control; ///< is control pressed
  /// Create an empty keyspec that doesn't correspond to any valid key press
  KeySpec():chr(0),key(irr::KEY_KEY_CODES_COUNT),shift(false),control(false){};
  /// Create a keyspec
  /**
   * @param key the keycode for the key
   * @param shift is shift pressed too
   * @param control is control pressed too
   */
  KeySpec(irr::EKEY_CODE key,bool shift=false,bool control=false,wchar_t chr=0):key(key),shift(shift),control(control),chr(chr){};
  /// Create a keyspec that corresponds to a keyspec but with modifiers changed
  /**
   * @param ks the keyspec to copy
   * @param shift is shift pressed too
   * @param control is control pressed too
   */
  KeySpec(const KeySpec& ks,bool shift,bool control):chr(ks.chr),key(ks.key),shift(shift),control(control){};

  /// Create a total ordering to enable this to be a key in a map
  /**
   * @note the details of the ordering shouldn’t be relied on.
   * @param o the other keyspec to compare to
   * @return true if this is considered strictly less than o else false
   */
  inline bool operator<(const KeySpec& o) const{
    if(key<o.key)
      return true;
    if(key>o.key)
      return false;
    if(!shift&&o.shift)
      return true;
    if(shift&&!o.shift)
      return false;
    if((!control)&&o.control)
      return true;
    return false;
  }
  /// Compare two keyspecs for equality
  /**
   * @note this is equivalent to !(a<b || b<a)
   * @param o the other keyspec to compare to
   * @return true if the keyspecs are the same
   */
  inline bool operator ==(const KeySpec& o) const{
    return key==o.key&&shift==o.shift&&control==o.control;
  }
};

/// A keymap
/**
 * This class contains a mapping from keyspecs to actions and from
 * actions to keyspecs and ensures that there is only one (or no) keyspec for each
 * action and only one (or no) action for each keyspec.
 *
 * If a KeySpec is requested but none is specified then the default KeySpec() will be
 * returned and equivalently if a @link KeyMap::Action Action @endlink is requested but
 * non is specified then the @link KeyMap::A_NONE A_NONE @endlink is returned
 */
class KeyMap{
  public:
    /// The actions available in the keymap
    enum Action{
      A_NONE=0, ///< A do nothing action
      A_MOVE_UP,A_MOVE_DOWN,
      A_MOVE_LEFT,A_MOVE_RIGHT,
      A_MOVE_FORWARD,A_MOVE_BACK,
      A_SLIDE_START_OUT,A_SLIDE_START_IN,
      A_SLIDE_END_IN,A_SLIDE_END_OUT,
      A_SLICE_UP_IN,A_SLICE_UP_OUT,
      A_SLICE_DOWN_IN,A_SLICE_DOWN_OUT,
      A_SLICE_LEFT_IN,A_SLICE_LEFT_OUT,
      A_SLICE_RIGHT_IN,A_SLICE_RIGHT_OUT,
      A_SLICE_FORWARD_IN,A_SLICE_FORWARD_OUT,
      A_SLICE_BACK_IN,A_SLICE_BACK_OUT,
      A_UNDO,
      A_GENERATE,A_LOAD,
      A_SAVE,A_CONF,
      A_COUNT ///< The count of actions
      };

    /// list of the slice actions along with what direction they slice and if in or out
    static const std::pair<Action,std::pair<Dirn,bool> > sliceActions[12];
    /// list of the slide the selection actions along with if they slide the string
    static const std::pair<Action,std::pair<bool,bool> > slideActions[4];
    /// list of the move the string actions along with direction they move the string
    static const std::pair<Action,Dirn> moveActions[6];
    /// an ordered list of the actions along with a name for them to show in gui's
    static const std::pair<Action,const wchar_t*> actionNames[A_COUNT-1];
  private:
    /// The map from keyspecs to actions
    std::map<KeySpec,Action> keyMap;
    /// The map from actions to keyspecs
    std::map<Action,KeySpec> revMap;
  public:
    /// Add a mapping to the keymap
    /**
     * This adds the mapping key -> a while ensuring that the conditions
     * for the keymap are kept valid. This may involve removing a previous
     * mapping for key or a
     * @param key the keyspec to add a mapping for
     * @param a the action to add a mapping for
     * @return the old keyspec for a and the old action for key
     */
    inline std::pair<KeySpec,Action> addMapping(KeySpec key,Action a){
      std::pair<KeySpec,Action> old(revMap[a],keyMap[key]);
      keyMap.erase(old.first);
      revMap.erase(old.second);
      keyMap[key]=a;
      revMap[a]=key;
      return old;
    }
    /// Get the action triggered by a keyspec
    /**
     * This also checks for the keyspec but without any modifiers present
     * @param key the keyspec to look up
     * @return The action for key if set or @link KeyMap::A_NONE A_NONE @endlink
     */
    inline Action getTriggeredAction(KeySpec key){
      Action a=keyMap[key];
      if(a == A_NONE && key.shift){
        a=keyMap[KeySpec(key,false,key.control)];
      }
      if(a == A_NONE && key.control){
        a=keyMap[KeySpec(key,key.shift,false)];
        if(a == A_NONE && key.shift){
          a=keyMap[KeySpec(key,false,false)];
        }
      }
      return a;
    }
    /// Get the action for a keyspec
    /**
     * @param key the keyspec to look up
     * @return The action for key if set or @link KeyMap::A_NONE A_NONE @endlink
     */
    inline Action getAction(KeySpec key){
      return keyMap[key];
    }
    /// Get the keyspec for an action
    /**
     * @param a the action to look up
     * @return The keyspec for a if set or the default KeySpec()
     */
    inline KeySpec getKeySpec(Action a){
      return revMap[a];
    }
    /// remove the mapping for a keyspec
    /**
     * @param key the keyspec to remove the mapping of
     * @return the action that key previously mapped to
     */
    inline Action removeMapping(KeySpec key){
      Action old=keyMap[key];
      keyMap.erase(key);
      revMap.erase(old);
      return old;
    }
    /// remove the mapping for an action
    /**
     * @param a the action to remove the mapping of
     * @return the keyspec that a previously mapped to
     */
    inline KeySpec removeMapping(Action a){
      KeySpec old(revMap[a]);
      revMap.erase(a);
      keyMap.erase(old);
      return old;
    }
    /// Get the keyspec to action map
    /**
     * @return the keyspec to action map
     */
    inline const std::map<KeySpec,Action>& getKeyMap() const{
      return keyMap;
    }
    /// Get the action to keyspec map
    /**
     * @return the action to keyspec map
     */
    inline const std::map<Action,KeySpec>& getRevMap() const{
      return revMap;
    }

  private:
    /// Parse some data into the keymap
    /**
     * Will only return once as much as possible of the data has been processed
     * @param data the data to parse
     * @param length the length of data available
     * @return the amount of data processed
     */
    irr::u32 doparse(char* data,irr::u32 length);

  public:
    /// Load the KeyMap from it's config file
    /**
     * Will try to load the keymap from a file called keymap.conf in the folders
     * returned by the following folders in order
     *     -# getUserConfigPath()
     *     -# getSystemConfigPath()
     *     -# getDefaultConfigPath()
     *     .
     * On a valid system at least the last should exist
     * @param fs the irrlicht file system object to allow access to the filesystem
     * @return true of the keymap was loaded successfully
     */
    bool load(irr::io::IFileSystem* fs);
    /// Save the KeyMap to it's config file
    /**
     * Will try to save the keymap to the file called keymap.conf in the folder
     * returned by getUserConfigPath()
     * @param fs the irrlicht file system object to allow access to the filesystem
     * @return true of the file was saved successfully
     */
    bool save(irr::io::IFileSystem* fs);
};

#endif

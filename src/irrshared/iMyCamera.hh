// Based on code Copyright (C) 2002-2011 Nikolaus Gebhardt
/**
 * @file iMyCamera.hh
 * @brief A Camera for use with the hypermaze.
 *
 * This camera has a fixed target and spins around and zooms in and out from that target
 */
#ifndef __C_SCENE_NODE_ANIMATOR_MY_CAMERA_H_INCLUDED__
#define __C_SCENE_NODE_ANIMATOR_MY_CAMERA_H_INCLUDED__

#include "ICameraSceneNode.h"
#include "vector2d.h"

namespace irr
{

namespace gui
{
    class ICursorControl;
}

}

/// Create a camera setup to spin around and zoom towards a fixed target point.
/**
 * The controls are right click and drag to spin around the target point and scroll wheel to zoom in and out.
 * Holding space key down causes all speeds to be decreased by a factor of 10
 * @param smgr The scene manager this camera should be created using
 * @param cursorControl the cursor control to use for panning around the target
 * @param parent the parent element for the camera. Can be used to move the camera say to track an object.
 * @param target the coordinates of the target (relative to parent)
 * @param position the position of the camera (relative to parent)
 * @param rotateSpeedX the X (horizontal) panning speed in units of degrees per fraction of screen width
 * @param rotateSpeedY the Y (vertical) panning speed in units of degrees per fraction of screen height
 * @param zoomSpeed the zoom speed in units of distance per wheel unit
 * @param id the id of the camera
 * @param makeActive should we make the camera the active camera
 * @return the camera node in case you need to make and further setup.
 */
irr::scene::ICameraSceneNode* addCameraSceneNodeMy(
    irr::scene::ISceneManager* smgr, irr::gui::ICursorControl * cursorControl,
    irr::scene::ISceneNode* parent=0, irr::core::vector3df target=irr::core::vector3df(),
    irr::core::vector3df position=irr::core::vector3df(),
    irr::f32 rotateSpeedX=-500.f, irr::f32 rotateSpeedY=-200.f,irr::f32 zoomSpeed=0.03f,
    irr::s32 id=-1, bool makeActive=true);

#endif

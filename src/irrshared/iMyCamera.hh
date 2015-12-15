// Based on code Copyright (C) 2002-2011 Nikolaus Gebhardt
/**
 * @file iMyCamera.hh
 * @brief A Camera for use with the hypermaze.
 *
 * This camera has a fixed target and spins around and zooms in and out from that target
 */
#ifndef __C_SCENE_NODE_ANIMATOR_CAMERA_MAYA_H_INCLUDED__
#define __C_SCENE_NODE_ANIMATOR_CAMERA_MAYA_H_INCLUDED__

#include "ICameraSceneNode.h"
#include "vector2d.h"

namespace irr
{

namespace gui
{
    class ICursorControl;
}

}

///
irr::scene::ICameraSceneNode* addCameraSceneNodeMy(irr::scene::ISceneManager* smgr,irr::gui::ICursorControl * CursorControl,
    irr::scene::ISceneNode* parent=0 ,irr::core::vector3df target=irr::core::vector3df(),
    irr::core::vector3df position=irr::core::vector3df(),irr::f32 rotateSpeedX=-500.f, irr::f32 rotateSpeedY=-200.f,
    irr::f32 zoomSpeed=0.03f, irr::f32 translationSpeed=100.f, irr::s32 id=-1, bool makeActive=true);
#endif

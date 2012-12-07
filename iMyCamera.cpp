// Copyright (C) 2002-2011 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include <iostream>
#include <ISceneManager.h>
#include "iMyCamera.hh"
#include <ICursorControl.h>
#include <ICameraSceneNode.h>
#include <SViewFrustum.h>

using namespace irr;
using namespace std;
//! constructor
CSceneNodeAnimatorCameraMy::CSceneNodeAnimatorCameraMy(gui::ICursorControl* cursor,
    f32 rotateSpeedX, f32 rotateSpeedY, f32 zoomSpeed, f32 translateSpeed)
    : CursorControl(cursor), OldCamera(0), MousePos(0.5f, 0.5f),MouseWheel(0),
    ZoomSpeed(zoomSpeed), RotateSpeedX(rotateSpeedX), RotateSpeedY(rotateSpeedY), TranslateSpeed(translateSpeed),
    Zoom(1), RotX(0.0f), RotY(0.0f),
    Rotating(false), Moving(false), Translating(false),Fine(false)
{
    if (CursorControl)
    {
        CursorControl->grab();
        MousePos = CursorControl->getRelativePosition();
    }

    allKeysUp();
}


//! destructor
CSceneNodeAnimatorCameraMy::~CSceneNodeAnimatorCameraMy()
{
    if (CursorControl)
        CursorControl->drop();
}


//! It is possible to send mouse and key events to the camera. Most cameras
//! may ignore this input, but camera scene nodes which are created for
//! example with scene::ISceneManager::addMayaCameraSceneNode or
//! scene::ISceneManager::addMeshViewerCameraSceneNode, may want to get this input
//! for changing their position, look at target or whatever.
bool CSceneNodeAnimatorCameraMy::OnEvent(const SEvent& event)
{
    if (event.EventType == irr::EET_KEY_INPUT_EVENT && event.KeyInput.Key == KEY_SPACE){
      Fine=event.KeyInput.PressedDown;
      return true;
    }
    if (event.EventType != EET_MOUSE_INPUT_EVENT)
        return false;

    switch(event.MouseInput.Event)
    {
    case EMIE_LMOUSE_PRESSED_DOWN:
        MouseKeys[0] = true;
        break;
    case EMIE_RMOUSE_PRESSED_DOWN:
        MouseKeys[2] = true;
        break;
    case EMIE_MMOUSE_PRESSED_DOWN:
        MouseKeys[1] = true;
        break;
    case EMIE_LMOUSE_LEFT_UP:
        MouseKeys[0] = false;
        break;
    case EMIE_RMOUSE_LEFT_UP:
        MouseKeys[2] = false;
        break;
    case EMIE_MMOUSE_LEFT_UP:
        MouseKeys[1] = false;
        break;
    case EMIE_MOUSE_MOVED:
        MousePos = CursorControl->getRelativePosition();
        break;
    case EMIE_MOUSE_WHEEL:
        MouseWheel+=event.MouseInput.Wheel;
        break;
    case EMIE_LMOUSE_DOUBLE_CLICK:
    case EMIE_RMOUSE_DOUBLE_CLICK:
    case EMIE_MMOUSE_DOUBLE_CLICK:
    case EMIE_LMOUSE_TRIPLE_CLICK:
    case EMIE_RMOUSE_TRIPLE_CLICK:
    case EMIE_MMOUSE_TRIPLE_CLICK:
    case EMIE_COUNT:
        return false;
    }
    return true;
}


//! OnAnimate() is called just before rendering the whole scene.
void CSceneNodeAnimatorCameraMy::animateNode(scene::ISceneNode *node, u32 timeMs)
{
    //LM = Rotate around camera pivot
    //Scroll Wheel= Dolly forth/back in view direction (speed % distance camera pivot - max distance to pivot)
    //Rm = Move on camera plane (Screen center is about the mouse pointer, depending on move speed)
    //Space slows movement by a factor of 10

    if (!node || node->getType() != scene::ESNT_CAMERA)
        return;

    scene::ICameraSceneNode* camera = static_cast<scene::ICameraSceneNode*>(node);

    // If the camera isn't the active camera, and receiving input, then don't process it.
    if (!camera->isInputReceiverEnabled())
        return;

    scene::ISceneManager * smgr = camera->getSceneManager();
    if (smgr && smgr->getActiveCamera() != camera)
        return;

    if (OldCamera != camera)
    {
        LastCameraTarget = Target = camera->getTarget();
        core::vector3df offset=camera->getTarget()-camera->getPosition();
        Zoom=offset.getLength();
        f32 Y=offset.Y;
        offset.Y=0;
        RotY=atan2(Y,offset.getLength())*core::RADTODEG;
        RotX=atan2(offset.Z,offset.X)*core::RADTODEG;
        OldCamera = camera;
    }
    else
    {
        Target += camera->getTarget() - LastCameraTarget;
    }

    float SpeedFactor=Fine?0.1:1;
    
    if (MouseWheel!=0)
    {
        const f32 old = Zoom;
        Zoom = Zoom + MouseWheel * ZoomSpeed*SpeedFactor;
        MouseWheel=0;

        if (Zoom < 0)
            Zoom = old;
    }
    // Translation ---------------------------------

    const core::vector3df upVector(camera->getUpVector());
    const core::vector3df target = camera->getTarget();

    core::vector3df pos = camera->getPosition();
    core::vector3df tvectX = pos - target;
    tvectX = tvectX.crossProduct(upVector);
    tvectX.normalize();

    const scene::SViewFrustum* const va = camera->getViewFrustum();
    core::vector3df tvectY = (va->getFarLeftDown() - va->getFarRightDown());
    tvectY = tvectY.crossProduct(upVector.Y > 0 ? pos - target : target - pos);
    tvectY.normalize();

    if (Translating)
    {
        Target += tvectX * (TranslateStart.X - MousePos.X)*TranslateSpeed*SpeedFactor*Zoom +
                     tvectY * (TranslateStart.Y - MousePos.Y)*TranslateSpeed*SpeedFactor*Zoom;
        Translating = false;
    }
    if (isMouseKeyDown(2))
    {
        TranslateStart = MousePos;
        Translating = true;
    }

    // Rotation ------------------------------------

    if (Rotating)
    {
        RotX += (RotateStart.X - MousePos.X) * RotateSpeedX*SpeedFactor;
        RotY += (RotateStart.Y - MousePos.Y) * RotateSpeedY*SpeedFactor;
        if(RotY>90)
          RotY=90;
        if(RotY<-90)
          RotY=-90;
        Rotating=false;
    }
    if (isMouseKeyDown(0))
    {
        RotateStart = MousePos;
        Rotating = true;
    }

    pos = Target;
    pos.X += Zoom;

    pos.rotateXYBy(RotY, Target);
    pos.rotateXZBy(-RotX, Target);

    camera->setPosition(pos);
    camera->setTarget(Target);

    pos.set(0,1,0);
    pos.rotateXYBy(-RotY);
    pos.rotateXZBy(-RotX+180.f);
    camera->setUpVector(pos);
    LastCameraTarget = camera->getTarget();
}


bool CSceneNodeAnimatorCameraMy::isMouseKeyDown(s32 key) const
{
    return MouseKeys[key];
}


void CSceneNodeAnimatorCameraMy::allKeysUp()
{
    for (s32 i=0; i<3; ++i)
        MouseKeys[i] = false;
}


//! Sets the rotation speed
void CSceneNodeAnimatorCameraMy::setRotateSpeedX(f32 speed)
{
    RotateSpeedX = speed;
}
//! Sets the rotation speed
void CSceneNodeAnimatorCameraMy::setRotateSpeedY(f32 speed)
{
    RotateSpeedY = speed;
}


//! Sets the movement speed
void CSceneNodeAnimatorCameraMy::setMoveSpeed(f32 speed)
{
    TranslateSpeed = speed;
}


//! Sets the zoom speed
void CSceneNodeAnimatorCameraMy::setZoomSpeed(f32 speed)
{
    ZoomSpeed = speed;
}


//! Set the distance
void CSceneNodeAnimatorCameraMy::setDistance(f32 distance)
{
    Zoom=distance;
}

        
//! Gets the rotation speed
f32 CSceneNodeAnimatorCameraMy::getRotateSpeedX() const
{
    return RotateSpeedX;
}

//! Gets the rotation speed
f32 CSceneNodeAnimatorCameraMy::getRotateSpeedY() const
{
    return RotateSpeedY;
}


// Gets the movement speed
f32 CSceneNodeAnimatorCameraMy::getMoveSpeed() const
{
    return TranslateSpeed;
}


//! Gets the zoom speed
f32 CSceneNodeAnimatorCameraMy::getZoomSpeed() const
{
    return ZoomSpeed;
}


//! Returns the current distance, i.e. orbit radius
f32 CSceneNodeAnimatorCameraMy::getDistance() const
{
    return Zoom;
}


scene::ISceneNodeAnimator* CSceneNodeAnimatorCameraMy::createClone(scene::ISceneNode* node, scene::ISceneManager* newManager)
{
    CSceneNodeAnimatorCameraMy * newAnimator =
        new CSceneNodeAnimatorCameraMy(CursorControl, RotateSpeedX, RotateSpeedY, ZoomSpeed, TranslateSpeed);
    return newAnimator;
}

scene::ICameraSceneNode* addCameraSceneNodeMy(scene::ISceneManager* smgr,gui::ICursorControl * CursorControl,
                                              scene::ISceneNode* parent ,core::vector3df target,core::vector3df position,
                                              f32 rotateSpeedX, f32 rotateSpeedY, f32 zoomSpeed, f32 translationSpeed, s32 id,
                                              bool makeActive)
{
    scene::ICameraSceneNode* node = smgr->addCameraSceneNode(parent, position,
            target, id, makeActive);
    if (node)
    {
        scene::ISceneNodeAnimator* anm = new CSceneNodeAnimatorCameraMy(CursorControl,
            rotateSpeedX, rotateSpeedY, zoomSpeed, translationSpeed);

        node->addAnimator(anm);
        anm->drop();
    }

    return node;
}    

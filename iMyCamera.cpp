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

    //! Special scene node animator for FPS cameras
    /** This scene node animator can be attached to a camera to make it act
    like a 3d modelling tool camera
    */
    class CSceneNodeAnimatorCameraMy : public scene::ISceneNodeAnimator 
    {
    public:
        //! Constructor
        CSceneNodeAnimatorCameraMy(gui::ICursorControl* cursor, f32 rotateSpeedX = -500.f, f32 rotateSpeedY = -200.f, 
            f32 zoomSpeed = 0.03f, f32 translationSpeed = 100.f);

        //! Destructor
        virtual ~CSceneNodeAnimatorCameraMy();

        //! Animates the scene node, currently only works on cameras
        virtual void animateNode(scene::ISceneNode* node, u32 timeMs);

        //! Event receiver
        virtual bool OnEvent(const SEvent& event);

        //! Returns the speed of movement in units per millisecond
        virtual f32 getMoveSpeed() const;

        //! Sets the speed of movement in units per millisecond
        virtual void setMoveSpeed(f32 moveSpeed);

        //! Returns the rotation speed
        virtual f32 getRotateSpeedX() const;

        //! Set the rotation speed
        virtual void setRotateSpeedX(f32 rotateSpeed);

        //! Returns the rotation speed
        virtual f32 getRotateSpeedY() const;

        //! Set the rotation speed
        virtual void setRotateSpeedY(f32 rotateSpeed);

        //! Returns the zoom speed
        virtual f32 getZoomSpeed() const;

        //! Set the zoom speed
        virtual void setZoomSpeed(f32 zoomSpeed);

        //! Returns the current distance, i.e. orbit radius
        virtual f32 getDistance() const;

        //! Set the distance
        virtual void setDistance(f32 distance);

        //! This animator will receive events when attached to the active camera
        virtual bool isEventReceiverEnabled() const
        {
            return true;
        }

        //! Returns type of the scene node
        virtual scene::ESCENE_NODE_ANIMATOR_TYPE getType() const 
        {
            return scene::ESNAT_CAMERA_MAYA;
        }

        //! Creates a clone of this animator.
        /** Please note that you will have to drop
        (IReferenceCounted::drop()) the returned pointer after calling
        this. */
        virtual scene::ISceneNodeAnimator* createClone(scene::ISceneNode* node, scene::ISceneManager* newManager=0);

    private:

        void allKeysUp();
        void animate();
        bool isMouseKeyDown(s32 key) const;

        bool MouseKeys[3];
        int MouseWheel;

        gui::ICursorControl *CursorControl;
        scene::ICameraSceneNode* OldCamera;
        core::vector3df Target;
        core::vector3df LastCameraTarget;   // to find out if the camera target was moved outside this animator
        core::position2df RotateStart;
        core::position2df ZoomStart;
        core::position2df TranslateStart;
        core::position2df MousePos;
        f32 ZoomSpeed;
        f32 RotateSpeedX;
        f32 RotateSpeedY;
        f32 TranslateSpeed;
        f32 Zoom;
        f32 RotX, RotY;
        bool Rotating;
        bool Moving;
        bool Translating;
        bool Fine;
    };

    


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
    if (isMouseKeyDown(2))
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

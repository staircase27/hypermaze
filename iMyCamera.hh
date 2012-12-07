// Copyright (C) 2002-2011 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

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

using namespace irr;

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

    
    scene::ICameraSceneNode* addCameraSceneNodeMy(scene::ISceneManager* smgr,gui::ICursorControl * CursorControl,
                                                  scene::ISceneNode* parent=0 ,core::vector3df target=core::vector3df(),core::vector3df position=core::vector3df(),
        f32 rotateSpeedX=-500.f, f32 rotateSpeedY=-200.f, f32 zoomSpeed=0.03f, f32 translationSpeed=100.f, s32 id=-1,
        bool makeActive=true);
#endif

// Based on code Copyright (C) 2002-2011 Nikolaus Gebhardt
/**
 * @file iMyCamera.cpp
 * @brief Implementation of iMyCamera.hh
 */
#include "iMyCamera.hh"

#include <ISceneManager.h>
#include <ICursorControl.h>
#include <ICameraSceneNode.h>
#include <SViewFrustum.h>

using namespace irr;
using namespace std;
/// Special scene node animator for the camera for hypermaze
/**
 * Animates a camera with fixed target that can rotate around the target and zoom in an out
 */
class CSceneNodeAnimatorCameraMy : public scene::ISceneNodeAnimator
{
public:
  /// Construct a new CSceneNodeAnimatorCameraMy
  /**
   * @param cursorControl the cursor control to use for panning around the target
   * @param rotateSpeedX the X (horizontal) panning speed in units of degrees per fraction of screen width
   * @param rotateSpeedY the Y (vertical) panning speed in units of degrees per fraction of screen height
   * @param zoomSpeed the zoom speed in units of distance per wheel unit
   */
  CSceneNodeAnimatorCameraMy(gui::ICursorControl* cursorControl,
      f32 rotateSpeedX = -500.f, f32 rotateSpeedY = -200.f, f32 zoomSpeed = 0.03f);

    /// Delete this animator and release any resources held
    virtual ~CSceneNodeAnimatorCameraMy();

    /// Animates the scene node (currently only works on cameras)
    /**
     * Updates the position and orientation of the camera based on events received previously.
     * @param node the camera node this is attached to.
     * @param timeMs the current time in milliseconds
     */
    virtual void animateNode(scene::ISceneNode* node, u32 timeMs);

    /// Event receiver
    /**
     * We use space pressed and released events, mouse click and drag events and mouse scroll events.
     * @param event the event to process
     * @return if we have processed the event or others should try.
     */
    virtual bool OnEvent(const SEvent& event);

    /// Returns the rotation speed
    /**
     * @return the horizontal rotation speed
     */
    virtual f32 getRotateSpeedX() const;

    /// Set the rotation speed
    /**
     * @param rotateSpeed the new horizontal rotation speed
     */
    virtual void setRotateSpeedX(f32 rotateSpeed);

    /// Returns the rotation speed
    /**
     * @return the vertical rotation speed
     */
    virtual f32 getRotateSpeedY() const;

    /// Set the rotation speed
    /**
     * @param rotateSpeed the new vertical rotation speed
     */
    virtual void setRotateSpeedY(f32 rotateSpeed);

    /// Returns the zoom speed
    /**
     * @return the zoom speed
     */
    virtual f32 getZoomSpeed() const;

    /// Set the zoom speed
    /**
     * @param zoomSpeed the new zoom speed
     */
    virtual void setZoomSpeed(f32 zoomSpeed);

    /// Returns the current distance, i.e. orbit radius
    /**
     * @return the current distance
     */
    virtual f32 getDistance() const;

    /// Set the distance
    /**
     * @param distance the new distance
     */
    virtual void setDistance(f32 distance);

    /// This animator will receive events when attached to the active camera
    /**
     * @return true as we do support receiving events
     */
    virtual bool isEventReceiverEnabled() const
    {
        return true;
    }

    /// Returns type of the scene node
    /**
     * @return the type of the scene node
     */
    virtual scene::ESCENE_NODE_ANIMATOR_TYPE getType() const
    {
        return scene::ESNAT_CAMERA_MAYA;
    }

    /// Creates a clone of this animator.
    /**
     * Please note that you will have to drop (IReferenceCounted::drop()) the returned pointer after calling this.
     * @param node the new node to attach the clone to.
     * @param newManager the new scene manager to work with.
     * @return the cloned animator
     */
    virtual scene::ISceneNodeAnimator* createClone(scene::ISceneNode* node, scene::ISceneManager* newManager=0);

private:

    bool indrag; ///< true if we are in a drag (the camera will pan when onAnimate is called)
    int mouseWheel; ///< The number of mouse wheel ticks scrolled
    bool fine; ///< if the movements should be fine (i.e. space is help down)

    gui::ICursorControl *cursorControl; ///< The cursor control used to get mouse position
    scene::ICameraSceneNode* currentCamera; ///< The current camera

    f32 rotateSpeedX; ///< The horizontal rotation speed
    f32 rotateSpeedY; ///< The vertical rotation speed
    f32 zoomSpeed; ///< The zoom speed

    core::dimension2d<u32> lastdim; ///< The dimension of the screen last time we animated
    core::position2df rotateStart; ///< The location of the mouse of the current rotation
    core::position2df mousePos; ///< The current mouse position

    f32 zoom; ///< The current zoom distance
    f32 rotX; ///< The X rotation
    f32 rotY; ///< The Y rotation
    bool rotating; ///< Flag for if we are in a rotation

};

CSceneNodeAnimatorCameraMy::CSceneNodeAnimatorCameraMy(gui::ICursorControl* cursorControl,
    f32 rotateSpeedX, f32 rotateSpeedY, f32 zoomSpeed)
    : cursorControl(cursorControl), currentCamera(0), mousePos(0.5f, 0.5f),mouseWheel(0),
    zoomSpeed(zoomSpeed), rotateSpeedX(rotateSpeedX), rotateSpeedY(rotateSpeedY),
    zoom(1), rotX(0.0f), rotY(0.0f),
    rotating(false), fine(false), indrag(false)
{
    if (cursorControl)
    {
        cursorControl->grab();
        mousePos = cursorControl->getRelativePosition();
    }
}

CSceneNodeAnimatorCameraMy::~CSceneNodeAnimatorCameraMy()
{
    if (cursorControl)
        cursorControl->drop();
}

bool CSceneNodeAnimatorCameraMy::OnEvent(const SEvent& event)
{
    if (event.EventType == irr::EET_KEY_INPUT_EVENT && event.KeyInput.Key == KEY_SPACE){
      fine=event.KeyInput.PressedDown;
      return true;
    }
    if (event.EventType != EET_MOUSE_INPUT_EVENT)
        return false;

    switch(event.MouseInput.Event)
    {
      case EMIE_RMOUSE_PRESSED_DOWN:
        indrag = true;
        break;
      case EMIE_RMOUSE_LEFT_UP:
        indrag = false;
        break;
      case EMIE_MOUSE_MOVED:
        mousePos = cursorControl->getRelativePosition();
        break;
      case EMIE_MOUSE_WHEEL:
        mouseWheel+=event.MouseInput.Wheel;
        break;
      default:
       return false;
    }
  return true;
}

void CSceneNodeAnimatorCameraMy::animateNode(scene::ISceneNode *node, u32 timeMs)
{
    //RM = Rotate around camera pivot
    //Scroll Wheel= Dolly forth/back in view direction (speed % distance camera pivot - max distance to pivot)
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

    // Check if camera has changed or screen has changed size and process the changes.
    if (currentCamera != camera){
        core::vector3df offset=camera->getTarget()-camera->getPosition();
        zoom=offset.getLength();
        f32 Y=offset.Y;
        offset.Y=0;
        rotY=atan2(Y,offset.getLength())*core::RADTODEG;
        rotX=atan2(offset.Z,offset.X)*core::RADTODEG;
        currentCamera = camera;
        if(smgr)
          lastdim=smgr->getVideoDriver()->getScreenSize();
    }else if(smgr){
      core::dimension2d<u32> dim=smgr->getVideoDriver()->getScreenSize();
      if(dim!=lastdim){
        if(dim.Height!=0&&dim.Width!=0){
          camera->setFOV(2.*atan(tan(camera->getFOV()/2.0)*dim.Height/lastdim.Height));
          camera->setAspectRatio(dim.Width*1.0/dim.Height);
          lastdim=dim;
        }
      }
    }

    float SpeedFactor=fine?0.1:1;

    bool changed = false;

    if (mouseWheel!=0)
    {
        const f32 old = zoom;
        zoom += mouseWheel * zoomSpeed*SpeedFactor;
        mouseWheel=0;

        if (zoom < 0)
            zoom = old;
        else
            changed = true;
    }

    const core::vector3df target = camera->getTarget();

    if (rotating){
        rotX += (rotateStart.X - mousePos.X) * rotateSpeedX*SpeedFactor;
        rotY += (rotateStart.Y - mousePos.Y) * rotateSpeedY*SpeedFactor;
        if(rotY>90)
          rotY=90;
        if(rotY<-90)
          rotY=-90;
        rotating=false;
        changed=true;
    }

    if (indrag){
        rotateStart = mousePos;
        rotating = true;
    }

    if(changed){
      core::vector3df pos = target;
      pos.X += zoom;
      pos.rotateXYBy(rotY, target);
      pos.rotateXZBy(-rotX, target);
      camera->setPosition(pos);

      pos.set(0,1,0);
      pos.rotateXYBy(-rotY);
      pos.rotateXZBy(-rotX+180.f);
      camera->setUpVector(pos);
    }
}

void CSceneNodeAnimatorCameraMy::setRotateSpeedX(f32 speed)
{
    rotateSpeedX = speed;
}

void CSceneNodeAnimatorCameraMy::setRotateSpeedY(f32 speed)
{
    rotateSpeedY = speed;
}

void CSceneNodeAnimatorCameraMy::setZoomSpeed(f32 speed)
{
    zoomSpeed = speed;
}

void CSceneNodeAnimatorCameraMy::setDistance(f32 distance)
{
    zoom=distance;
}

f32 CSceneNodeAnimatorCameraMy::getRotateSpeedX() const
{
    return rotateSpeedX;
}

f32 CSceneNodeAnimatorCameraMy::getRotateSpeedY() const
{
    return rotateSpeedY;
}

f32 CSceneNodeAnimatorCameraMy::getZoomSpeed() const
{
    return zoomSpeed;
}

f32 CSceneNodeAnimatorCameraMy::getDistance() const
{
    return zoom;
}

scene::ISceneNodeAnimator* CSceneNodeAnimatorCameraMy::createClone(scene::ISceneNode* node, scene::ISceneManager* newManager)
{
    CSceneNodeAnimatorCameraMy * newAnimator =
        new CSceneNodeAnimatorCameraMy(cursorControl, rotateSpeedX, rotateSpeedY, zoomSpeed);
    return newAnimator;
}

irr::scene::ICameraSceneNode* addCameraSceneNodeMy(
    irr::scene::ISceneManager* smgr, irr::gui::ICursorControl * cursorControl,
    irr::scene::ISceneNode* parent, irr::core::vector3df target,
    irr::core::vector3df position,
    irr::f32 rotateSpeedX, irr::f32 rotateSpeedY,irr::f32 zoomSpeed,
    irr::s32 id, bool makeActive){
    scene::ICameraSceneNode* node = smgr->addCameraSceneNode(parent, position,
            target, id, makeActive);
    if (node)
    {
        node->setFOV(1.256637061*0.6);
        scene::ISceneNodeAnimator* anm = new CSceneNodeAnimatorCameraMy(cursorControl,
            rotateSpeedX, rotateSpeedY, zoomSpeed);

        node->addAnimator(anm);
        anm->drop();
    }

    return node;
}

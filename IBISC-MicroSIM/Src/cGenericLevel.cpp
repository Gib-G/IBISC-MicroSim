#include "cGenericLevel.h"

using namespace std;

#define RESOURCE_PATH(p)    (char*)((m_resourceRoot+string(p)).c_str())


//===========================================================================
/*!
    Constructor of cGenericDemo.
*/
//===========================================================================
cGenericLevel::cGenericLevel(const string a_resourceRoot,
    const int a_numDevices,
    shared_ptr<cGenericHapticDevice> a_hapticDevice0,
    shared_ptr<cGenericHapticDevice> a_hapticDevice1)
{   
       
    bool fileload;

    defaultPos = cVector3d(0, 0, 0);

    lastFrame = glfwGetTime();

    moveSpeed = 1;

    // display is not mirrored
    m_mirroredDisplay = false;

    // torque gain
    m_torqueGain = 2.0;

    // initialize tool radius
    m_toolRadius = 0.0025;

    // create world
    m_world = new cWorld();

    // set background color
    m_world->m_backgroundColor.set(0.0f, 0.0f, 0.0f);

    // set shadow level
    m_world->setShadowIntensity(0.3);
    m_world->setUseShadowCasting(true);

    // create camera
    m_camera = new cCamera(m_world);
    m_world->addChild(m_camera);

    // position and oriente the camera
    m_camera->set(cVector3d(0.3, 0.0, 0.3),    // camera position (eye)
        cVector3d(0.0, 0.0, 0.3),    // lookat position (target)
        cVector3d(0.0, 0.0, 1.0));   // direction of the "up" vector

// set the near and far clipping planes of the camera
// anything in front/behind these clipping planes will not be rendered
    m_camera->setClippingPlanes(0.01, 20.0);
    m_camera->setUseMultipassTransparency(true);

    /*// create a positional light
    m_light0 = new cSpotLight(m_world);
    m_world->addChild(m_light0);                      // attach light to camera
    m_light0->setEnabled(true);                       // enable light source
    m_light0->setLocalPos(0.4, 0.4, 0.3);             // position the light source
    m_light0->setDir(-0.4, -0.4, -0.3);               // define the direction of the light beam
    m_light0->m_ambient.set(0.6, 0.6, 0.6);
    m_light0->m_diffuse.set(0.8, 0.8, 0.8);
    m_light0->m_specular.set(0.8, 0.8, 0.8);
    m_light0->m_shadowMap->setEnabled(true);
    m_light0->setCutOffAngleDeg(30);
    m_light0->m_shadowMap->setQualityHigh();

    // create a directional light
    cDirectionalLight* m_light1 = new cDirectionalLight(m_world);
    m_world->addChild(m_light1);                   // attach light to camera
    m_light1->setEnabled(true);                    // enable light source
    m_light1->setDir(-1.0, 0.0, -1.0);             // define the direction of the light beam
    m_light1->m_ambient.set(0.0, 0.0, 0.0);
    m_light1->m_diffuse.set(0.4, 0.4, 0.4);
    m_light1->m_specular.set(0.0, 0.0, 0.0);*/

    // create a ground
    m_ground = new cMesh();
    m_world->addChild(m_ground);

    cCreatePlane(m_ground, 0.2, 0.3);
    m_ground->m_material->setGrayLevel(0.5);
    m_ground->m_material->setStiffness(1000);
    m_ground->m_material->setDynamicFriction(0.2);
    m_ground->m_material->setStaticFriction(0.2);
    m_ground->createAABBCollisionDetector(m_toolRadius);
    m_ground->setShowEnabled(false);


}

//===========================================================================
/*!
    Update haptics.
*/
//===========================================================================
void cGenericLevel::updateHaptics()
{
   
}


//===========================================================================
/*!
    Set device offset.
*/
//===========================================================================


//===========================================================================
/*!
    Set torque gain.
*/
//===========================================================================
void cGenericLevel::setTorqueGain(double a_torqueGain)
{
    m_torqueGain = a_torqueGain;
}


void cGenericLevel::ZoomCamera() {
    movementVector.zero();
    if (Zoom_In == 0) {
        movementVector = m_camera->getUpVector();
        movementVector.negate();
    }
    else if (Zoom_Out == 0) {
        movementVector = m_camera->getUpVector();
    }
    movementVector.mul(deltaTime);
    movementVector.mul(moveSpeed);
    m_camera->translate(movementVector);
}
//===========================================================================
/*
    Software License Agreement (BSD License)
    Copyright (c) 2003-2016, CHAI3D
    (www.chai3d.org)

    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above
    copyright notice, this list of conditions and the following
    disclaimer in the documentation and/or other materials provided
    with the distribution.

    * Neither the name of CHAI3D nor the names of its contributors may
    be used to endorse or promote products derived from this software
    without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
    FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
    COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
    ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE. 

    \author    <http://www.chai3d.org>
    \author    Francois Conti
    \version   3.2.0 $Rev: 1928 $
*/
//===========================================================================

//---------------------------------------------------------------------------
#include "CGenericDemo.h"
using namespace std;
//---------------------------------------------------------------------------
#include "CODE.h"
//---------------------------------------------------------------------------

//------------------------------------------------------------------------------
// DECLARED MACROS
//------------------------------------------------------------------------------
// convert to resource path
#define RESOURCE_PATH(p)    (char*)((a_resourceRoot+string(p)).c_str())


//===========================================================================
/*!
    Constructor of cGenericDemo.
*/
//===========================================================================
cGenericDemo::cGenericDemo(const int a_numDevices,
                           shared_ptr<cGenericHapticDevice> a_hapticDevice0,
                           shared_ptr<cGenericHapticDevice> a_hapticDevice1)
                           //bool a_visualization)
{
    //m_visualization = a_visualization;

    // display is not mirrored
    m_mirroredDisplay = false;

    // torque gain
    m_torqueGain = 2.0;

    // camera orbit angle step
    m_angleStep = 5.0;

    // initialize tool radius
    m_toolRadius = 0.0025;

    // create world
    m_world = new cWorld();;

    //if (a_visualization) {
        // set background color
        m_world->m_backgroundColor.setWhite();

        // set shadow level
        m_world->setShadowIntensity(0.3);
        // create camera
        m_camera = new cCamera(m_world);

        // create orbit axes
        cameraOrbitAxisY = new cGenericObject();
        cameraOrbitAxisY->setLocalPos(0.0, 0.0, 0.0);
        cameraOrbitAxisZ = new cGenericObject();
        cameraOrbitAxisZ->setLocalPos(0.0, 0.0, 0.0);

        // add hierarchical relations
        cameraOrbitAxisZ->addChild(m_camera);
        cameraOrbitAxisY->addChild(cameraOrbitAxisZ);
        m_world->addChild(cameraOrbitAxisY);

        // position and oriente the camera
        m_camera->set(cVector3d(0.4, 0.0, 0.3),    // camera position (eye)
            cVector3d(0.0, 0.0, 0.0),    // lookat position (target)
            cVector3d(0.0, 0.0, 1.0));   // direction of the "up" vector

        // set the near and far clipping planes of the camera
        // anything in front/behind these clipping planes will not be rendered
        m_camera->setClippingPlanes(0.01, 4.0);
        m_camera->setUseMultipassTransparency(true);
        m_camera->setStereoEyeSeparation(0.005);
        m_camera->setStereoFocalLength(0.7);

        // create a positional light
        m_light0 = new cSpotLight(m_world);
        m_world->addChild(m_light0);                      // attach light to camera
        m_light0->setEnabled(true);                       // enable light source
        m_light0->setLocalPos(0.4, 0.4, 0.3);             // position the light source
        m_light0->setDir(-0.4, -0.4, -0.3);               // define the direction of the light beam
        m_light0->m_ambient.set(0.6, 0.6, 0.6);
        m_light0->m_diffuse.set(0.8, 0.8, 0.8);
        m_light0->m_specular.set(0.8, 0.8, 0.8);
        m_light0->m_shadowMap->setEnabled(true);
        m_light0->setCutOffAngleDeg(40);
        m_light0->m_shadowMap->setQualityVeryHigh();

        // create a directional light
        cDirectionalLight* m_light1 = new cDirectionalLight(m_world);
        m_world->addChild(m_light1);                   // attach light to camera
        m_light1->setEnabled(true);                    // enable light source
        m_light1->setDir(-1.0, 0.0, -1.0);             // define the direction of the light beam
        m_light1->m_ambient.set(0.3, 0.3, 0.3);
        m_light1->m_diffuse.set(0.6, 0.6, 0.6);
        m_light1->m_specular.set(0.0, 0.0, 0.0);

        // create a background
        cBackground* background = new cBackground();
        m_camera->m_backLayer->addChild(background);

        // set background properties
        background->setCornerColors(cColorf(1.00, 1.00, 1.00),
            cColorf(1.00, 1.00, 1.0),
            cColorf(0.90, 0.90, 0.90),
            cColorf(0.90, 0.90, 0.90));

        // create a font
        cFontPtr font = NEW_CFONTCALIBRI20();

        // create a label to display the haptic and graphic rate of the simulation
        labelRate = new cLabel(font);
        m_camera->m_frontLayer->addChild(labelRate);
    //}

    // create central sphere of 5mm at worldPosition
    cMaterial matZero;
    matZero.setGreen();

    cMesh* zero = new cMesh();
    cCreateSphere(zero, 0.005);
    zero->setMaterial(matZero);
    zero->setLocalPos(0.0, 0.0, 0.0);
    m_world->addChild(zero);

    // create scene axis
    m_sceneAxis = new cGenericObject();
    m_sceneAxis->setLocalPos(0.0, 0.0, 0.0);
    m_world->addChild(m_sceneAxis);

    cMesh* zero1 = zero->copy();
    m_sceneAxis->addChild(zero1);

    // create a ground
    /*m_ground = new cMesh();
    m_world->addChild(m_ground);
    
    cCreatePlane(m_ground, 0.2, 0.3);
    m_ground->m_material->setGrayLevel(0.5);
    m_ground->m_material->setStiffness(1000);
    m_ground->m_material->setDynamicFriction(0.2);
    m_ground->m_material->setStaticFriction(0.2);
    m_ground->createAABBCollisionDetector(m_toolRadius);
    m_ground->setShowEnabled(false);

    // create a base
    m_base = new cMultiMesh();
    m_world->addChild(m_base);
    
    bool fileload = m_base->loadFromFile(("../resources/models/base/base.obj"));
    if (!fileload)
    {
        fileload = m_base->loadFromFile("../../../bin/resources/models/base/base.obj");
    }
    if (!fileload)
    {
        printf("Error - 3D Model failed to load correctly.\n");
    }

    m_base->scale(0.001);
    m_base->setShowFrame(false);
    m_base->setShowBoundaryBox(false);
    m_base->setLocalPos(-0.05, 0.0, 0.001);
    m_base->setUseDisplayList(true);*/

    // create tools
    m_tools[0] = NULL;
    m_tools[1] = NULL;
    m_tool0 = NULL;
    m_tool1 = NULL;

    // create offset axes
    /*axis_tool0 = new cGenericObject();
    m_world->addChild(axis_tool0);
    axis_tool0->setLocalPos(0.0, -0.11, 0.04);
    axis_tool1 = new cGenericObject();
    m_world->addChild(axis_tool1);
    axis_tool1->setLocalPos(0.0, 0.11, 0.04);*/

    m_numTools = cMin(a_numDevices, 2);

    cMesh* mesh = new cMesh();
    cMatrix3d rot;
    rot.identity();
    rot.rotateAboutLocalAxisDeg(cVector3d(1,0,0), 90);
    cCreateRing(mesh, 0.001, 0.005, 4, 16, cVector3d(0,0,0), rot);
    mesh->m_material->setWhite();
    mesh->setTransparencyLevel(0.4f);
    

	/*cMesh* meshAux = new cMesh();
	cCreateRing(meshAux, 0.001, 0.005, 4, 16, cVector3d(0, 0, 0), rot);
	meshAux->m_material->setRed();
	meshAux->setTransparencyLevel(0.4f);*/


    if (m_numTools > 0)
    {
        //m_tool0 = new cToolForcep(m_world);
        m_tool0 = new cToolGripper(m_world);
        //axis_tool0->addChild(m_tool0);
        m_world->addChild(m_tool0);//
        m_tool0->setHapticDevice(shared_ptr<cGenericHapticDevice>(a_hapticDevice0));
        m_tool0->setRadius(m_toolRadius);
        m_tool0->enableDynamicObjects(true);
        m_tool0->setWaitForSmallForce(true);
        m_tool0->start();
        m_tool0->setWaitForSmallForce(true);
        m_tools[0] = m_tool0;

        cMesh* mesh0 = mesh->copy();
        m_tool0->m_hapticPointThumb->m_sphereProxy->addChild(mesh0);
        
        cMesh* mesh1 = mesh->copy();
        m_tool0->m_hapticPointFinger->m_sphereProxy->addChild(mesh1);

        /*cMesh* metaProxyFinger;		//TODO : make copies of the same meshes...
        metaProxyFinger = new cMesh();
        cCreateSphere(metaProxyFinger, 0.0025);
        metaProxyFinger->m_material->setBlueCyan();
        metaProxyFinger->setLocalPos(0.0, 0.0, 0.02);
        m_world->addChild(metaProxyFinger);

        cMesh* metaProxyThumb;
        metaProxyThumb = new cMesh();
        cCreateSphere(metaProxyThumb, 0.0025);
        metaProxyThumb->m_material->setBlueCyan();
        metaProxyThumb->setLocalPos(0.0, 0.0, 0.02);
        m_world->addChild(metaProxyThumb);*/

        //m_tool0->metaProxyFinger = metaProxyFinger;
        //m_tool0->metaProxyThumb = metaProxyThumb;
    }

    if (m_numTools > 1)
    {
        //m_tool1 = new cToolForcep(m_world);
        m_tool1 = new cToolGripper(m_world);
        //axis_tool1->addChild(m_tool1);
        m_world->addChild(m_tool1);
        m_tool1->setHapticDevice(a_hapticDevice1);
        m_tool1->setRadius(m_toolRadius);
        m_tool1->enableDynamicObjects(true);
        m_tool1->setWaitForSmallForce(true);
        m_tool1->start();
        m_tool1->setWaitForSmallForce(true);
        m_tools[1] = m_tool1;

        cMesh* mesh0 = mesh->copy();
        m_tool1->m_hapticPointThumb->m_sphereProxy->addChild(mesh0);

        cMesh* mesh1 = mesh->copy();
        m_tool1->m_hapticPointFinger->m_sphereProxy->addChild(mesh1);

        /*cMesh* metaProxyFinger;
        metaProxyFinger = new cMesh();
        cCreateSphere(metaProxyFinger, 0.0025);
        metaProxyFinger->m_material->setBlueCyan();
        metaProxyFinger->setLocalPos(0.0, 0.0, 0.02);
        m_world->addChild(metaProxyFinger);

        cMesh* metaProxyThumb;
        metaProxyThumb = new cMesh();
        cCreateSphere(metaProxyThumb, 0.0025);
        metaProxyThumb->m_material->setBlueCyan();
        metaProxyThumb->setLocalPos(0.0, 0.0, 0.02);
        m_world->addChild(metaProxyThumb);

        m_tool1->metaProxyFinger = metaProxyFinger;
        m_tool1->metaProxyThumb = metaProxyThumb;*/
    }

    for (int i = 0; i < m_numTools; i++) {
        m_tools[i]->setForcesOFF();
    }

    // create an ODE world to simulate dynamic bodies
    m_ODEWorld = new cODEWorld(m_world);
    //m_world->addChild(m_ODEWorld);
    m_sceneAxis->addChild(m_ODEWorld);

    // set some gravity
    m_ODEWorld->setGravity(cVector3d(0.0, 0.0, -9.81));
    m_ODEWorld->setLinearDamping(0.01);
    m_ODEWorld->setAngularDamping(0.01);

    // we create a static plane
    m_ODEGPlane = new cODEGenericBody(m_ODEWorld);
    m_ODEGPlane->createStaticPlane(cVector3d(0.0, 0.0, 0.0), cVector3d(0.0, 0.0, 1.0));

    cMaterial matObs;
    matObs.setGrayLevel(0.3);
    matObs.setStiffness(800);
    //matObs.setDynamicFriction(0.1);
    //matObs.setStaticFriction(0.1);

    cMesh* wall = new cMesh();
    cCreatePlane(wall, 1, 1);
    wall->createAABBCollisionDetector(m_toolRadius);
    wall->setMaterial(matObs);
    m_ODEGPlane->setImageModel(wall);
}


//===========================================================================
/*!
    Update graphics.

    \param  a_width  Width of viewport.
    \param  a_height  Height of viewport.
*/
//===========================================================================
void cGenericDemo::updateGraphics(int a_width, int a_height) 
{
    // update haptic and graphic rate data
    labelRate->setText(cStr(freqCounterHaptics.getFrequency(), 0) + " Hz");

    // update position of label
    labelRate->setLocalPos((int)(0.5 * (a_width - labelRate->getWidth())), 15);

    // update shadow maps (if any)
    m_world->updateShadowMaps(false, m_mirroredDisplay);

    // render view
    m_camera->renderView(a_width, a_height); 
}


//===========================================================================
/*!
    Update haptics.
*/
//===========================================================================
void cGenericDemo::updateHaptics() 
{ 
    // compute global reference frames for each object
    m_world->computeGlobalPositions(true);
    
    /*// get serial values for potentiometers
    valuesRaw[LEFT_TOOL] = valuesRaw[RIGHT_TOOL] = 0;
    m_serialController->GetLastFilteredValues(valuesRaw);

    // calculate opening angle of tools from serial values
    anglesCalculated[LEFT_TOOL] = MAX_ABSOLUT_OPENING / DATA_RESOLUTION * valuesRaw[LEFT_TOOL] + INITIAL_OPENING_RIGHT;
    anglesCalculated[RIGHT_TOOL] = -MAX_ABSOLUT_OPENING / DATA_RESOLUTION * valuesRaw[RIGHT_TOOL] + MAX_ABSOLUT_OPENING + INITIAL_OPENING_LEFT;

    if (m_numTools > 0) m_tools[0]->setType(1);
    if (m_numTools > 1) m_tools[1]->setType(-1);
    */

    // update positions
    for (int i=0; i<m_numTools; i++)
    {
        m_tools[i]->updateFromDevice();
        cMatrix3d rot = m_tools[i]->getDeviceGlobalRot();

        m_tools[i]->m_hapticPointFinger->m_sphereProxy->setLocalRot(rot);//
        m_tools[i]->m_hapticPointThumb->m_sphereProxy->setLocalRot(rot);//

        /*m_tools[i]->m_hapticPointFinger->m_sphereProxy->setLocalRot(rot);
        m_tools[i]->m_hapticPointThumb->m_sphereProxy->setLocalRot(rot);
        m_tools[i]->m_hapticPointAuxFinger->m_sphereProxy->setLocalRot(rot);
        m_tools[i]->m_hapticPointAuxThumb->m_sphereProxy->setLocalRot(rot);
        
		m_tools[i]->setGripperAngleDeg(anglesCalculated[i]);*/
    }
   
    // compute interaction forces
    for (int i=0; i<m_numTools; i++)
    {
        m_tools[i]->computeInteractionForces();
    }
   
    // apply forces to haptic devices
    for (int i=0; i<m_numTools; i++)
    {
        m_tools[i]->setDeviceGlobalTorque(m_torqueGain * m_tools[i]->getDeviceGlobalTorque());
        m_tools[i]->applyToDevice();
    }

    // apply forces to ODE objects
    for (int i=0; i<m_numTools; i++)
    {
        // for each interaction point of the tool we look for any contact events
        // with the environment and apply forces accordingly
        int numInteractionPoints = m_tools[i]->getNumHapticPoints();
        for (int j=0; j<numInteractionPoints; j++)
        {
            // get pointer to next interaction point of tool
            cHapticPoint* interactionPoint = m_tools[i]->getHapticPoint(j);

            // check all contact points
            int numContacts = interactionPoint->getNumCollisionEvents();
            for (int k=0; k<numContacts; k++)
            {
                cCollisionEvent* collisionEvent = interactionPoint->getCollisionEvent(k);

                // given the mesh object we may be touching, we search for its owner which
                // could be the mesh itself or a multi-mesh object. Once the owner found, we
                // look for the parent that will point to the ODE object itself.
                cGenericObject* object = collisionEvent->m_object->getOwner()->getOwner();

                // cast to ODE object
                cODEGenericBody* ODEobject = dynamic_cast<cODEGenericBody*>(object);

                // if ODE object, we apply interaction forces
                if (ODEobject != NULL)
                {
                    //if (isObjectAvailableForMovement(ODEobject))
                        ODEobject->addExternalForceAtPoint(-0.3 * interactionPoint->getLastComputedForce(),
                                                       collisionEvent->m_globalPos);
                }
            }
        }
    }

    // retrieve simulation time and compute next interval
    double time = simClock.getCurrentTimeSeconds();
    double nextSimInterval = cClamp(time, 0.0001, 0.001);

    // reset clock
    simClock.reset();
    simClock.start();

    // signal frequency counter
    freqCounterHaptics.signal(1);

    // update simulation
    m_ODEWorld->updateDynamics(nextSimInterval);

    ///
    if (m_updateCubes) {
        updateDynamicsCube(m_cube, m_state);
        m_updateCubes = false;
    }
}


//===========================================================================
/*!
    Set device offset.
*/
//===========================================================================
void cGenericDemo::setOffset(double a_offset)
{
    if (m_tool0 != NULL)
    {
        cVector3d pos(0.0, a_offset, 0.0);
        m_tool0->setLocalPos(pos);
    }
    if (m_tool1 != NULL)
    {
        cVector3d pos(0.0,-a_offset, 0.0);
        m_tool1->setLocalPos(pos);
    }
}


//===========================================================================
/*!
    Set torque gain.
*/
//===========================================================================
void cGenericDemo::setTorqueGain(double a_torqueGain)
{
    m_torqueGain = a_torqueGain;
}

/*
//===========================================================================
//Set orbit camera angle step.
//===========================================================================
void cGenericGameScene::setAngleStep(double a_angleStep)
{
    m_torqueGain = a_angleStep;
}

//===========================================================================
//Set serial controller.
//===========================================================================
void cGenericGameScene::setSerialController(SerialController* a_serialController)
{
    m_serialController = a_serialController;
}
*/

//===========================================================================
/*!
Update position of camera
*/
//===========================================================================
void cGenericDemo::updateCamera(int dest) {
    switch (dest) {
    case 8:
        cameraOrbitAxisY->rotateAboutLocalAxisDeg(cVector3d(0, 1, 0), m_angleStep);
        break;
    case 2:
        cameraOrbitAxisY->rotateAboutLocalAxisDeg(cVector3d(0, 1, 0), -m_angleStep);
        break;
    case 4:
        cameraOrbitAxisZ->rotateAboutLocalAxisDeg(cVector3d(0, 0, 1), m_angleStep);
        break;
    case 6:
        cameraOrbitAxisZ->rotateAboutLocalAxisDeg(cVector3d(0, 0, 1), -m_angleStep);
        break;
    }
}

void cGenericDemo::UpdateGameScenePosition(cVector3d a_pos) {
    m_sceneAxis->setLocalPos(a_pos);
}

void cGenericDemo::SetHapticForces(bool enable) {
    for (int i = 0; i < m_numTools; i++) {
        if (enable)
            m_tools[i]->setForcesON();
        else
            m_tools[i]->setForcesOFF();
    }
}

int cGenericDemo::updateDynamics(int a_cube, bool a_state)
{
    m_cube = a_cube;
    m_state = a_state;
    m_updateCubes = true;
    return 0;
}

void cGenericDemo::getPointsProxysPositions(double outPosArray[])
{
    if (m_tool0 != NULL)
    {
        outPosArray[0] = m_tool0->m_hapticPointFinger->getGlobalPosProxy().x();
        outPosArray[1] = m_tool0->m_hapticPointFinger->getGlobalPosProxy().y();
        outPosArray[2] = m_tool0->m_hapticPointFinger->getGlobalPosProxy().z();
        outPosArray[3] = m_tool0->m_hapticPointThumb->getGlobalPosProxy().x();
        outPosArray[4] = m_tool0->m_hapticPointThumb->getGlobalPosProxy().y();
        outPosArray[5] = m_tool0->m_hapticPointThumb->getGlobalPosProxy().z();
    }
    else {
        outPosArray[0] = 0;
        outPosArray[1] = 0;
        outPosArray[2] = 0;
        outPosArray[3] = 0;
        outPosArray[4] = 0;
        outPosArray[5] = 0;
    }

    if (m_tool1 != NULL)
    {
        outPosArray[6] = m_tool1->m_hapticPointFinger->getGlobalPosProxy().x();
        outPosArray[7] = m_tool1->m_hapticPointFinger->getGlobalPosProxy().y();
        outPosArray[8] = m_tool1->m_hapticPointFinger->getGlobalPosProxy().z();
        outPosArray[9] = m_tool1->m_hapticPointThumb->getGlobalPosProxy().x();
        outPosArray[10] = m_tool1->m_hapticPointThumb->getGlobalPosProxy().y();
        outPosArray[11] = m_tool1->m_hapticPointThumb->getGlobalPosProxy().z();
    }
    else {
        outPosArray[6] = 0;
        outPosArray[7] = 0;
        outPosArray[8] = 0;
        outPosArray[9] = 0;
        outPosArray[10] = 0;
        outPosArray[11] = 0;
    }
}

void cGenericDemo::getPointsGoalsPositions(double outPosArray[])
{
    if (m_tool0 != NULL)
    {
        outPosArray[0] = m_tool0->m_hapticPointFinger->getGlobalPosGoal().x();
        outPosArray[1] = m_tool0->m_hapticPointFinger->getGlobalPosGoal().y();
        outPosArray[2] = m_tool0->m_hapticPointFinger->getGlobalPosGoal().z();
        outPosArray[3] = m_tool0->m_hapticPointThumb->getGlobalPosGoal().x();
        outPosArray[4] = m_tool0->m_hapticPointThumb->getGlobalPosGoal().y();
        outPosArray[5] = m_tool0->m_hapticPointThumb->getGlobalPosGoal().z();
    }
    else {
        outPosArray[0] = 0;
        outPosArray[1] = 0;
        outPosArray[2] = 0;
        outPosArray[3] = 0;
        outPosArray[4] = 0;
        outPosArray[5] = 0;
    }

    if (m_tool1 != NULL)
    {
        outPosArray[6] = m_tool1->m_hapticPointFinger->getGlobalPosGoal().x();
        outPosArray[7] = m_tool1->m_hapticPointFinger->getGlobalPosGoal().y();
        outPosArray[8] = m_tool1->m_hapticPointFinger->getGlobalPosGoal().z();
        outPosArray[9] = m_tool1->m_hapticPointThumb->getGlobalPosGoal().x();
        outPosArray[10] = m_tool1->m_hapticPointThumb->getGlobalPosGoal().y();
        outPosArray[11] = m_tool1->m_hapticPointThumb->getGlobalPosGoal().z();
    }
    else {
        outPosArray[6] = 0;
        outPosArray[7] = 0;
        outPosArray[8] = 0;
        outPosArray[9] = 0;
        outPosArray[10] = 0;
        outPosArray[11] = 0;
    }
}

void cGenericDemo::getDevicesPositions(double outPosArray[])
{
    if (m_tool0 != NULL)
    {
        outPosArray[0] = m_tool0->getDeviceGlobalPos().x();
        outPosArray[1] = m_tool0->getDeviceGlobalPos().y();
        outPosArray[2] = m_tool0->getDeviceGlobalPos().z();
        outPosArray[3] = m_tool0->getGlobalPos().x();
        outPosArray[4] = m_tool0->getGlobalPos().y();
        outPosArray[5] = m_tool0->getGlobalPos().z();
    }
    else {
        outPosArray[0] = 0;
        outPosArray[1] = 0;
        outPosArray[2] = 0;
        outPosArray[3] = 0;
        outPosArray[4] = 0;
        outPosArray[5] = 0;
    }

    if (m_tool1 != NULL)
    {
        outPosArray[6] = m_tool1->getDeviceGlobalPos().x();
        outPosArray[7] = m_tool1->getDeviceGlobalPos().y();
        outPosArray[8] = m_tool1->getDeviceGlobalPos().z();
        outPosArray[9] = m_tool1->getGlobalPos().x();
        outPosArray[10] = m_tool1->getGlobalPos().y();
        outPosArray[11] = m_tool1->getGlobalPos().z();
    }
    else {
        outPosArray[6] = 0;
        outPosArray[7] = 0;
        outPosArray[8] = 0;
        outPosArray[9] = 0;
        outPosArray[10] = 0;
        outPosArray[11] = 0;
    }
}


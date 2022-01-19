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
    \version   3.2.0 $Rev: 1869 $
*/
//===========================================================================

//---------------------------------------------------------------------------
#include "CDemo1.h"
using namespace std;
//---------------------------------------------------------------------------

//===========================================================================
/*!
    Constructor of cDemo1.
*/
//===========================================================================
cDemo1::cDemo1(const int a_numDevices,
               shared_ptr<cGenericHapticDevice> a_hapticDevice0,
               shared_ptr<cGenericHapticDevice> a_hapticDevice1):cGenericDemo(a_numDevices, a_hapticDevice0, a_hapticDevice1)
{
    cMaterial matObs;
    matObs.setGrayLevel(0.3);
    matObs.setStiffness(1500);

    m_ODEWall = new cODEGenericBody(m_ODEWorld);
    cMesh* wall = new cMesh();
    cCreateBox(wall, 0.30, 0.018, 0.08);
    m_ODEWall->createDynamicBox(0.30, 0.018, 0.08, true);
    wall->createAABBCollisionDetector(m_toolRadius);
    wall->setMaterial(matObs);
    m_ODEWall->setImageModel(wall);
    m_ODEWall->setLocalPos(0.0, 0.0, 0.04);

    // create a new ODE object that is automatically added to the ODE world
    m_ODEBody0 = new cODEGenericBody(m_ODEWorld);
    m_ODEBody1 = new cODEGenericBody(m_ODEWorld);
    m_ODEBody2 = new cODEGenericBody(m_ODEWorld);
    m_ODEBody3 = new cODEGenericBody(m_ODEWorld);
    m_ODEBody4 = new cODEGenericBody(m_ODEWorld);
    m_ODEBody5 = new cODEGenericBody(m_ODEWorld);

    // create a virtual mesh  that will be used for the geometry
    // representation of the dynamic body
    cMesh* object0 = new cMesh();
    cMesh* object1 = new cMesh();
    cMesh* object2 = new cMesh();
    cMesh* object3 = new cMesh();
    cMesh* object4 = new cMesh();
    cMesh* object5 = new cMesh();

    // crate a cube mesh
    double boxSize = 0.02;
    cCreateBox(object0, boxSize, boxSize, boxSize);
    cCreateBox(object1, boxSize, boxSize, boxSize);
    cCreateBox(object2, boxSize, boxSize, boxSize);
    cCreateBox(object3, boxSize, boxSize, boxSize);
    cCreateBox(object4, boxSize, boxSize, boxSize);
    cCreateBox(object5, boxSize, boxSize, boxSize);

    object0->createAABBCollisionDetector(m_toolRadius);
    object1->createAABBCollisionDetector(m_toolRadius);
    object2->createAABBCollisionDetector(m_toolRadius);
    object3->createAABBCollisionDetector(m_toolRadius);
    object4->createAABBCollisionDetector(m_toolRadius);
    object5->createAABBCollisionDetector(m_toolRadius);

    // define some material properties for each cube
    cMaterial matObject;
    matObject.setRed();
    matObject.m_specular.set(0.0, 0.0, 0.0);
    matObject.setStiffness(1000);
    matObject.setDynamicFriction(1.0);
    matObject.setStaticFriction(0.9);

    object0->setMaterial(matObject);
    object1->setMaterial(matObject);
    object2->setMaterial(matObject);
    object3->setMaterial(matObject);
    object4->setMaterial(matObject);
    object5->setMaterial(matObject);

    // add mesh to ODE object
    m_ODEBody0->setImageModel(object0);
    m_ODEBody1->setImageModel(object1);
    m_ODEBody2->setImageModel(object2);
    m_ODEBody3->setImageModel(object3);
    m_ODEBody4->setImageModel(object4);
    m_ODEBody5->setImageModel(object5);

    // create a dynamic model of the ODE object. Here we decide to use a box just like
    // the object mesh we just defined
    m_ODEBody0->createDynamicBox(boxSize, boxSize, boxSize);
    m_ODEBody1->createDynamicBox(boxSize, boxSize, boxSize);
    m_ODEBody2->createDynamicBox(boxSize, boxSize, boxSize);
    m_ODEBody3->createDynamicBox(boxSize, boxSize, boxSize);
    m_ODEBody4->createDynamicBox(boxSize, boxSize, boxSize);
    m_ODEBody5->createDynamicBox(boxSize, boxSize, boxSize);

    // define some mass properties for each cube
    double cubeMass = 0.0295;
    m_ODEBody0->setMass(cubeMass);
    m_ODEBody1->setMass(cubeMass);
    m_ODEBody2->setMass(cubeMass);
    m_ODEBody3->setMass(cubeMass);
    m_ODEBody4->setMass(cubeMass);
    m_ODEBody5->setMass(cubeMass);

    // initialize
    init();
};


//===========================================================================
/*!
    Set stiffness of environment

    \param  a_stiffness  Stiffness value [N/m]
*/
//===========================================================================
void cDemo1::setStiffness(double a_stiffness)
{
    // set objects stiffness
    m_ODEBody0->m_imageModel->setStiffness(a_stiffness);
    m_ODEBody1->m_imageModel->setStiffness(a_stiffness);
    m_ODEBody2->m_imageModel->setStiffness(a_stiffness);
    m_ODEBody3->m_imageModel->setStiffness(a_stiffness);
    m_ODEBody4->m_imageModel->setStiffness(a_stiffness);
    m_ODEBody5->m_imageModel->setStiffness(a_stiffness);

    //m_ODEWall->m_imageModel->setStiffness(a_stiffness);
};


//===========================================================================
/*!
    Initialize position of objects
*/
//===========================================================================
void cDemo1::init()
{
    // set position of each cube
    resetObjectsPositions();
}

int cDemo1::updateDynamicsCube(int a_cube, bool a_state)
{
    cODEGenericBody* cubeToUpdate;

    switch (a_cube) {
    case 0:
        cubeToUpdate = m_ODEBody0;
        break;
    case 1:
        cubeToUpdate = m_ODEBody1;
        break;
    case 2:
        cubeToUpdate = m_ODEBody2;
        break;
    case 3:
        cubeToUpdate = m_ODEBody3;
        break;
    case 4:
        cubeToUpdate = m_ODEBody4;
        break;
    case 5:
        cubeToUpdate = m_ODEBody5;
        break;
	default:
		cubeToUpdate = m_ODEBody0;
		break;
    }

    cVector3d pos = cubeToUpdate->getLocalPos();
    cMatrix3d rot = cubeToUpdate->getLocalRot();

    if (a_state) {
        double h = 0.01125;
        pos.z(h);
        cubeToUpdate->setLocalPos(pos);	// set this position to initial one call resetObjectPosition(a_cube)
        cubeToUpdate->setLocalRot(rot);
        cubeToUpdate->enableDynamics();
        cubeBeingUsed = cubeToUpdate;
    }
    else {
        double h = 0.01125;
        pos.z(h);
        cubeToUpdate->disableDynamics();
        cubeToUpdate->setLocalPos(pos);
        cubeToUpdate->setLocalRot(rot);
    }

    return 0;
};


bool cDemo1::isObjectAvailableForMovement(cODEGenericBody* body) {
    return cubeBeingUsed == body;
}

//===========================================================================
/*!
Set mass of the objects

\param  a_mass value [Kg]
*/
//===========================================================================
void cDemo1::setMass(double a_mass)
{
    // set objects mass
    double cubeMass = a_mass;

    m_ODEBody0->setMass(cubeMass);
    m_ODEBody1->setMass(cubeMass);
    m_ODEBody2->setMass(cubeMass);
    m_ODEBody3->setMass(cubeMass);
    m_ODEBody4->setMass(cubeMass);
    m_ODEBody5->setMass(cubeMass);
};


//===========================================================================
/*!
Initialize position of objects
*/
//===========================================================================
void cDemo1::resetObjectsPositions()
{
    double h = 0.01125;

    // set position of each cube
    m_ODEBody5->setLocalPos(0.0425, -0.1625, h);
    m_ODEBody4->setLocalPos(0.0, -0.1625, h);
    m_ODEBody3->setLocalPos(-0.0425, -0.1625, h);
    m_ODEBody2->setLocalPos(0.0425, 0.1625, h);
    m_ODEBody1->setLocalPos(0.0, 0.1625, h);
    m_ODEBody0->setLocalPos(-0.0425, 0.1625, h);

    cMatrix3d rot;
    rot.identity();
    m_ODEBody0->setLocalRot(rot);
    m_ODEBody1->setLocalRot(rot);
    m_ODEBody2->setLocalRot(rot);
    m_ODEBody3->setLocalRot(rot);
    m_ODEBody4->setLocalRot(rot);
    m_ODEBody5->setLocalRot(rot);

    m_ODEBody5->disableDynamics();
    m_ODEBody4->disableDynamics();
    m_ODEBody3->disableDynamics();
    m_ODEBody2->disableDynamics();
    m_ODEBody1->disableDynamics();
    m_ODEBody0->disableDynamics();
};



//===========================================================================
/*!
Re-Initialize position of object
*/
//===========================================================================
void cDemo1::resetObjectPosition(int c)
{
    double h = 0.01125;
    cMatrix3d rot;
    rot.identity();

    switch (c) {
    case 0:
        m_ODEBody0->setLocalPos(-0.0425, 0.1625, h);
        m_ODEBody0->setLocalRot(rot);
        break;
    case 1:
        m_ODEBody1->setLocalPos(0.0, 0.1625, h);
        m_ODEBody1->setLocalRot(rot);
        break;
    case 2:
        m_ODEBody2->setLocalPos(0.0425, 0.1625, h);
        m_ODEBody2->setLocalRot(rot);
        break;
    case 3:
        m_ODEBody3->setLocalPos(-0.0425, -0.1625, h);
        m_ODEBody3->setLocalRot(rot);
        break;
    case 4:
        m_ODEBody4->setLocalPos(0.0, -0.1625, h);
        m_ODEBody4->setLocalRot(rot);
        break;
    case 5:
        m_ODEBody5->setLocalPos(0.0425, -0.1625, h);
        m_ODEBody5->setLocalRot(rot);
        break;
    }
};

void cDemo1::debugTemp() {
	/// TURN ON/OFF forces
	/*if (flag) {
		m_tool0->setForcesON();
		resetObjectsPositions();
	}
	else {
		m_tool0->setForcesOFF();
	}
	flag = !flag;*/
	/// ///////////////////////

	/// MOVE SCENE OBJECTS
	/*cVector3d newPos = cVector3d(0, 0, ++inc * 0.01);
	m_sceneAxis->setLocalPos(newPos);*/
	/// ///////////////////////
}

int cDemo1::GetRigidObjects(double outInformation[]) {
	int count = 0;
	int c = 0;
	double* rot;

	// m_sceneAxis 
	// parent = NULL
	outInformation[c++] = -1;
	// type AXIS
	outInformation[c++] = 4;
	// position
	outInformation[c++] = m_sceneAxis->getLocalPos().x();
	outInformation[c++] = m_sceneAxis->getLocalPos().y();
	outInformation[c++] = m_sceneAxis->getLocalPos().z();
	// rotation
	rot = GetRotationQuaternion(m_sceneAxis->getLocalRot());
	for (int i = 0; i < 4; i++) {
		outInformation[c++] = rot[i];
	}
	count++;

	// m_platformSpace 
	// parent = m_sceneAxis
	outInformation[c++] = 0;
	// type PLANE
	outInformation[c++] = 0;
	// position
	outInformation[c++] = m_platformSpace->getLocalPos().x();
	outInformation[c++] = m_platformSpace->getLocalPos().y();
	outInformation[c++] = m_platformSpace->getLocalPos().z();
	// rotation
	rot = GetRotationQuaternion(m_platformSpace->getLocalRot());
	for (int i = 0; i < 4; i++) {
		outInformation[c++] = rot[i];
	}
	// scale/dimensions
	outInformation[c++] = 0.60;
	outInformation[c++] = 0.785;
	count++;

	// m_workingSpace 
	// parent = m_sceneAxis
	outInformation[c++] = 0;
	// type PLANE
	outInformation[c++] = 0;
	// position
	outInformation[c++] = m_workingSpace->getLocalPos().x();
	outInformation[c++] = m_workingSpace->getLocalPos().y();
	outInformation[c++] = m_workingSpace->getLocalPos().z();
	// rotation
	rot = GetRotationQuaternion(m_workingSpace->getLocalRot());
	for (int i = 0; i < 4; i++) {
		outInformation[c++] = rot[i];
	}
	// scale/dimensions
	outInformation[c++] = 0.22;
	outInformation[c++] = 0.305;
	count++;

	// m_ODEWall 
	// parent = m_sceneAxis
	outInformation[c++] = 0;
	// type CUBE
	outInformation[c++] = 1;
	// position
	outInformation[c++] = m_ODEWall->getLocalPos().x();
	outInformation[c++] = m_ODEWall->getLocalPos().y();
	outInformation[c++] = m_ODEWall->getLocalPos().z();
	// rotation
	rot = GetRotationQuaternion(m_ODEWall->getLocalRot());
	for (int i = 0; i < 4; i++) {
		outInformation[c++] = rot[i];
	}
	// scale/dimensions
	outInformation[c++] = 0.3;
	outInformation[c++] = 0.018;
	outInformation[c++] = 0.08;
	count++;

	// m_ODEBody0 
	// parent = m_sceneAxis
	outInformation[c++] = 0;
	// type CUBE
	outInformation[c++] = 1;
	// position
	outInformation[c++] = m_ODEBody0->getLocalPos().x();
	outInformation[c++] = m_ODEBody0->getLocalPos().y();
	outInformation[c++] = m_ODEBody0->getLocalPos().z();
	// rotation
	rot = GetRotationQuaternion(m_ODEBody0->getLocalRot());
	for (int i = 0; i < 4; i++) {
		outInformation[c++] = rot[i];
	}
	// scale/dimensions
	outInformation[c++] = 0.021;
	outInformation[c++] = 0.021;
	outInformation[c++] = 0.021;
	count++;

	// m_ODEBody1 
	// parent = m_sceneAxis
	outInformation[c++] = 0;
	// type CUBE
	outInformation[c++] = 1;
	// position
	outInformation[c++] = m_ODEBody1->getLocalPos().x();
	outInformation[c++] = m_ODEBody1->getLocalPos().y();
	outInformation[c++] = m_ODEBody1->getLocalPos().z();
	// rotation
	rot = GetRotationQuaternion(m_ODEBody1->getLocalRot());
	for (int i = 0; i < 4; i++) {
		outInformation[c++] = rot[i];
	}
	// scale/dimensions
	outInformation[c++] = 0.021;
	outInformation[c++] = 0.021;
	outInformation[c++] = 0.021;
	count++;

	// m_ODEBody2 
	// parent = m_sceneAxis
	outInformation[c++] = 0;
	// type CUBE
	outInformation[c++] = 1;
	// position
	outInformation[c++] = m_ODEBody2->getLocalPos().x();
	outInformation[c++] = m_ODEBody2->getLocalPos().y();
	outInformation[c++] = m_ODEBody2->getLocalPos().z();
	// rotation
	rot = GetRotationQuaternion(m_ODEBody2->getLocalRot());
	for (int i = 0; i < 4; i++) {
		outInformation[c++] = rot[i];
	}
	// scale/dimensions
	outInformation[c++] = 0.021;
	outInformation[c++] = 0.021;
	outInformation[c++] = 0.021;
	count++;

	// m_ODEBody3 
	// parent = m_sceneAxis
	outInformation[c++] = 0;
	// type CUBE
	outInformation[c++] = 1;
	// position
	outInformation[c++] = m_ODEBody3->getLocalPos().x();
	outInformation[c++] = m_ODEBody3->getLocalPos().y();
	outInformation[c++] = m_ODEBody3->getLocalPos().z();
	// rotation
	rot = GetRotationQuaternion(m_ODEBody3->getLocalRot());
	for (int i = 0; i < 4; i++) {
		outInformation[c++] = rot[i];
	}
	// scale/dimensions
	outInformation[c++] = 0.021;
	outInformation[c++] = 0.021;
	outInformation[c++] = 0.021;
	count++;

	// m_ODEBody4 
	// parent = m_sceneAxis
	outInformation[c++] = 0;
	// type CUBE
	outInformation[c++] = 1;
	// position
	outInformation[c++] = m_ODEBody4->getLocalPos().x();
	outInformation[c++] = m_ODEBody4->getLocalPos().y();
	outInformation[c++] = m_ODEBody4->getLocalPos().z();
	// rotation
	rot = GetRotationQuaternion(m_ODEBody4->getLocalRot());
	for (int i = 0; i < 4; i++) {
		outInformation[c++] = rot[i];
	}
	// scale/dimensions
	outInformation[c++] = 0.021;
	outInformation[c++] = 0.021;
	outInformation[c++] = 0.021;
	count++;

	// m_ODEBody5 
	// parent = m_sceneAxis
	outInformation[c++] = 0;
	// type CUBE
	outInformation[c++] = 1;
	// position
	outInformation[c++] = m_ODEBody5->getLocalPos().x();
	outInformation[c++] = m_ODEBody5->getLocalPos().y();
	outInformation[c++] = m_ODEBody5->getLocalPos().z();
	// rotation
	rot = GetRotationQuaternion(m_ODEBody5->getLocalRot());
	for (int i = 0; i < 4; i++) {
		outInformation[c++] = rot[i];
	}
	// scale/dimensions
	outInformation[c++] = 0.021;
	outInformation[c++] = 0.021;
	outInformation[c++] = 0.021;
	count++;

	// axis_tool0 
	// parent = NULL
	outInformation[c++] = -1;
	// type AXIS
	outInformation[c++] = 4;
	// position
	outInformation[c++] = axis_tool0->getLocalPos().x();
	outInformation[c++] = axis_tool0->getLocalPos().y();
	outInformation[c++] = axis_tool0->getLocalPos().z();
	// rotation
	rot = GetRotationQuaternion(axis_tool0->getLocalRot());
	for (int i = 0; i < 4; i++) {
		outInformation[c++] = rot[i];
	}
	count++;

	// axis_tool1 
	// parent = NULL
	outInformation[c++] = -1;
	// type AXIS
	outInformation[c++] = 4;
	// position
	outInformation[c++] = axis_tool1->getLocalPos().x();
	outInformation[c++] = axis_tool1->getLocalPos().y();
	outInformation[c++] = axis_tool1->getLocalPos().z();
	// rotation
	rot = GetRotationQuaternion(axis_tool1->getLocalRot());
	for (int i = 0; i < 4; i++) {
		outInformation[c++] = rot[i];
	}
	count++;

	cout << "GetRigidObjects :: package size = " << c << endl;
	return count;
}

int cDemo1::GetInteractObjects(double outInformation[]) {
	int count = 0;	// number of interactuable objects --tools are not counted
	int c = 0;
	double* rot;

	// m_ODEBody0 
	// position
	outInformation[c++] = m_ODEBody0->getLocalPos().x();
	outInformation[c++] = m_ODEBody0->getLocalPos().y();
	outInformation[c++] = m_ODEBody0->getLocalPos().z();
	// rotation
	rot = GetRotationQuaternion(m_ODEBody0->getLocalRot());
	for (int i = 0; i < 4; i++) {
		outInformation[c++] = rot[i];
	}
	count++;

	// m_ODEBody1
	// position
	outInformation[c++] = m_ODEBody1->getLocalPos().x();
	outInformation[c++] = m_ODEBody1->getLocalPos().y();
	outInformation[c++] = m_ODEBody1->getLocalPos().z();
	// rotation
	rot = GetRotationQuaternion(m_ODEBody1->getLocalRot());
	for (int i = 0; i < 4; i++) {
		outInformation[c++] = rot[i];
	}
	count++;

	// m_ODEBody2
	// position
	outInformation[c++] = m_ODEBody2->getLocalPos().x();
	outInformation[c++] = m_ODEBody2->getLocalPos().y();
	outInformation[c++] = m_ODEBody2->getLocalPos().z();
	// rotation
	rot = GetRotationQuaternion(m_ODEBody2->getLocalRot());
	for (int i = 0; i < 4; i++) {
		outInformation[c++] = rot[i];
	}
	count++;

	// m_ODEBody3
	// position
	outInformation[c++] = m_ODEBody3->getLocalPos().x();
	outInformation[c++] = m_ODEBody3->getLocalPos().y();
	outInformation[c++] = m_ODEBody3->getLocalPos().z();
	// rotation
	rot = GetRotationQuaternion(m_ODEBody3->getLocalRot());
	for (int i = 0; i < 4; i++) {
		outInformation[c++] = rot[i];
	}
	count++;

	// m_ODEBody4
	// position
	outInformation[c++] = m_ODEBody4->getLocalPos().x();
	outInformation[c++] = m_ODEBody4->getLocalPos().y();
	outInformation[c++] = m_ODEBody4->getLocalPos().z();
	// rotation
	rot = GetRotationQuaternion(m_ODEBody4->getLocalRot());
	for (int i = 0; i < 4; i++) {
		outInformation[c++] = rot[i];
	}
	count++;

	// m_ODEBody5
	// position
	outInformation[c++] = m_ODEBody5->getLocalPos().x();
	outInformation[c++] = m_ODEBody5->getLocalPos().y();
	outInformation[c++] = m_ODEBody5->getLocalPos().z();
	// rotation
	rot = GetRotationQuaternion(m_ODEBody5->getLocalRot());
	for (int i = 0; i < 4; i++) {
		outInformation[c++] = rot[i];
	}
	count++;

	// HAPTIC DEVICES
	for (int t = 0; t < m_numTools; t++) {		
		// rotation
		rot = GetRotationQuaternion(m_tools[t]->getDeviceGlobalRot());
		for (int i = 0; i < 4; i++) {
			outInformation[c++] = rot[i];
		}

		// fixed angle tool
		// position
		outInformation[c++] = m_tools[t]->m_hapticPointFinger->getLocalPosGoal().x();
		outInformation[c++] = m_tools[t]->m_hapticPointFinger->getLocalPosGoal().y();
		outInformation[c++] = m_tools[t]->m_hapticPointFinger->getLocalPosGoal().z();
		// forceps angles raw and calculated : NOT USED (from my previous app)
		outInformation[c++] = 0; // cRadToDeg(m_tools[t]->getStaticAngleRad());
		outInformation[c++] = 0; // cRadToDeg(m_tools[t]->getRecalculatedStaticAngleRad());

		// moval angle tool
		// position
		outInformation[c++] = m_tools[t]->m_hapticPointThumb->getLocalPosGoal().x();
		outInformation[c++] = m_tools[t]->m_hapticPointThumb->getLocalPosGoal().y();
		outInformation[c++] = m_tools[t]->m_hapticPointThumb->getLocalPosGoal().z();
		// forceps angles raw and calculated : NOT USED (from my previous app)
		outInformation[c++] = 0; // cRadToDeg(m_tools[t]->getGripperAngleRad());
		outInformation[c++] = 0; // cRadToDeg(m_tools[t]->getRecalculatedtGripperAngleRad());
	}
	//cout << "GetInteractObjects :: package size = " << c << endl;

	return c;
}

int cDemo1::GetHapticInformation(double outInformation[]) {
	int c = 0;
	double* rot;
	/*******************************
	for (int t = 0; t < m_numTools; t++) {
		// position
		// m_hapticPoint
		//outInformation[c++] = m_tools[t]->metaProxyDevice->getLocalPos().x();
		//outInformation[c++] = m_tools[t]->metaProxyDevice->getLocalPos().y();
		//outInformation[c++] = m_tools[t]->metaProxyDevice->getLocalPos().z();
		//outInformation[c++] = m_tools[t]->m_hapticPointDevice->getLocalPosGoal().x();
		//outInformation[c++] = m_tools[t]->m_hapticPointDevice->getLocalPosGoal().y();
		//outInformation[c++] = m_tools[t]->m_hapticPointDevice->getLocalPosGoal().z();
		// m_hapticPoint
		outInformation[c++] = m_tools[t]->metaProxyFinger->getLocalPos().x();
		outInformation[c++] = m_tools[t]->metaProxyFinger->getLocalPos().y();
		outInformation[c++] = m_tools[t]->metaProxyFinger->getLocalPos().z();
		outInformation[c++] = m_tools[t]->m_hapticPointFinger->getLocalPosGoal().x();
		outInformation[c++] = m_tools[t]->m_hapticPointFinger->getLocalPosGoal().y();
		outInformation[c++] = m_tools[t]->m_hapticPointFinger->getLocalPosGoal().z();
		// m_hapticPoint
		outInformation[c++] = m_tools[t]->metaProxyThumb->getLocalPos().x();
		outInformation[c++] = m_tools[t]->metaProxyThumb->getLocalPos().y();
		outInformation[c++] = m_tools[t]->metaProxyThumb->getLocalPos().z();
		outInformation[c++] = m_tools[t]->m_hapticPointThumb->getLocalPosGoal().x();
		outInformation[c++] = m_tools[t]->m_hapticPointThumb->getLocalPosGoal().y();
		outInformation[c++] = m_tools[t]->m_hapticPointThumb->getLocalPosGoal().z();
		// rotation
		rot = GetRotationQuaternion(m_tools[t]->getDeviceGlobalRot());
		for (int i = 0; i < 4; i++) {
			outInformation[c++] = rot[i];
		}
		// forceps angles raw and calculated
		//outInformation[c++] = valuesRaw[t];
		//outInformation[c++] = anglesCalculated[t];
		outInformation[c++] = cRadToDeg(m_tools[t]->getGripperAngleRad());
		outInformation[c++] = cRadToDeg(m_tools[t]->getRecalculatedtGripperAngleRad());
		outInformation[c++] = cRadToDeg(m_tools[t]->getGripperAngleRad());
		outInformation[c++] = cRadToDeg(m_tools[t]->getRecalculatedtGripperAngleRad());
	}
	******************************************************/
	return m_numTools;
}

double* cDemo1::GetRotationQuaternion(cMatrix3d mat)
{
	double ret[4];
	ret[3] = cSqrt(cMax(0.0, (1 + mat(0, 0) + mat(1, 1) + mat(2, 2)))) / 2;
	ret[0] = cSqrt(cMax(0.0, (1 + mat(0, 0) - mat(1, 1) - mat(2, 2)))) / 2;
	ret[1] = cSqrt(cMax(0.0, (1 - mat(0, 0) + mat(1, 1) - mat(2, 2)))) / 2;
	ret[2] = cSqrt(cMax(0.0, (1 - mat(0, 0) - mat(1, 1) + mat(2, 2)))) / 2;
	ret[0] *= cSign(ret[0] * (mat(2, 1) - mat(1, 2)));
	ret[1] *= cSign(ret[1] * (mat(0, 2) - mat(2, 0)));
	ret[2] *= cSign(ret[2] * (mat(1, 0) - mat(0, 1)));

	return ret;
}
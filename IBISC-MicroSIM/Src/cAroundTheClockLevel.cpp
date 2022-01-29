#include "cAroundTheClockLevel.h"
#include "cToolGripperLevel.h"

using namespace std;


#define RESOURCE_PATH(p)    (char*)((m_resourceRoot+string(p)).c_str())

cAroundTheClockLevel::cAroundTheClockLevel(const std::string a_resourceRoot,
	const int a_numDevices,
	std::shared_ptr<cGenericHapticDevice> a_hapticDevice0,
	std::shared_ptr<cGenericHapticDevice> a_hapticDevice1) : cToolGripperLevel(a_resourceRoot, a_numDevices, a_hapticDevice0, a_hapticDevice1) {

	double maxStiffness;
	int w;
	defaultPos = cVector3d(1.6, 0, 2.5);

	if (a_numDevices > 0) {
		cHapticDeviceInfo hapticDeviceInfo = a_hapticDevice0->getSpecifications();
		double workspaceScaleFactor = 1;
		maxStiffness = hapticDeviceInfo.m_maxLinearStiffness / workspaceScaleFactor;
	}
	else {
		maxStiffness = 3;
	}

	// create a light source
	light = new cSpotLight(m_world);

	// attach light to camera
	m_world->addChild(light);

	// enable light source
	light->setEnabled(true);

	// position the light source
	light->setLocalPos(0.0, 0.0, 1.2);

	// define the direction of the light beam
	light->setDir(0.0, 0.0, -1.0);

	// set uniform concentration level of light 
	light->setSpotExponent(0.0);

	// enable this light source to generate shadows
	light->setShadowMapEnabled(true);

	// set the resolution of the shadow map
	//light->m_shadowMap->setQualityLow();
	light->m_shadowMap->setQualityVeryHigh();

	// set light cone half angle
	light->setCutOffAngleDeg(45);

	// create an ODE world to simulate dynamic bodies
	ODEWorld = new cODEWorld(m_world);

	// add ODE world as a node inside world
	m_world->addChild(ODEWorld);

	// set some gravity
	ODEWorld->setGravity(cVector3d(0.00, 0.00, -45.81));

	// define damping properties
	ODEWorld->setAngularDamping(0.02);
	ODEWorld->setLinearDamping(0.02);


	//////////////////////////////////////////////////////////////////////////
	// 3 ODE BLOCKS
	//////////////////////////////////////////////////////////////////////////
	// create a new ODE object that is automatically added to the ODE world
	ODEBody0 = new cODEGenericBody(ODEWorld);
	ODEBodytest = new cODEGenericBody(ODEWorld);

	// create a virtual mesh  that will be used for the geometry representation of the dynamic body
	object0 = new cMesh();
	objecttest = new cMesh();

	// create a cube mesh
	double size = 0.40;
	cCreateBox(object0, size * 2, size * 0.1, size * 0.1);
	cCreateBox(objecttest, size * 2, size * 0.1, size * 0.1);
	object0->createAABBCollisionDetector(toolRadius);
	objecttest->createAABBCollisionDetector(toolRadius);


	// define some material properties for each cube
	cMaterial mat0, mat1, mat2;
	mat0.setRedIndian();
	mat0.setStiffness(0.3 * maxStiffness);
	mat0.setDynamicFriction(3);
	mat0.setStaticFriction(3);
	object0->setMaterial(mat0);

	mat1.setBlueRoyal();
	mat1.setStiffness(0.3 * maxStiffness);
	mat1.setDynamicFriction(3);
	mat1.setStaticFriction(3);

	mat2.setGreenDarkSea();
	mat2.setStiffness(0.3 * maxStiffness);
	mat2.setDynamicFriction(3);
	mat2.setStaticFriction(3);
	objecttest->setMaterial(mat2);

	// add mesh to ODE object
	ODEBody0->setImageModel(object0);
	ODEBodytest->setImageModel(objecttest);

	// create a dynamic model of the ODE object. Here we decide to use a box just like
	// the object mesh we just defined
	ODEBody0->createDynamicBox(size * 2, size * 0.1, size * 0.1);
	ODEBodytest->createDynamicBox(size * 2, size * 0.1, size * 0.1);

	// define some mass properties for each cube
	ODEBody0->setMass(0.01);
	ODEBodytest->setMass(0.01);
	InitializeNeedleDetect();
	// set position of each cube
	ODEBody0->setLocalPos(0.0, -0.6, -0.5);
	ODEBodytest->setLocalPos(0.0, 0.6, -0.5);

	for (int i = 0; i < m_numTools; i++) {
		ODEBody2[i] = new cODEGenericBody(ODEWorld);
		object2[i] = new cMesh();
		cCreateBox(object2[i], size, 0.03, 0.03);
		object2[i]->setMaterial(mat2);
		ODEBody2[i]->setImageModel(object2[i]);
		ODEBody2[i]->createDynamicBox(size, size, size);
		ODEBody2[i]->disableDynamics();

		ODEBody3[i] = new cODEGenericBody(ODEWorld);
		object3[i] = new cMesh();
		cCreateBox(object3[i], size, 0.03, 0.03);
		object3[i]->setMaterial(mat2);
		ODEBody3[i]->setImageModel(object3[i]);
		ODEBody3[i]->createDynamicBox(size, size, size);
		ODEBody3[i]->disableDynamics();

	}

	for (int i = 0; i < 12; i++) {
		ODEBody1[i] = new cODEGenericBody(ODEWorld);
		object1[i] = new cMesh();
		cCreateRing(object1[i], 0.01, 0.1);
		object1[i]->createAABBCollisionDetector(toolRadius);
		object1[i]->setMaterial(mat1);
		ODEBody1[i]->setImageModel(object1[i]);
		ODEBody1[i]->createDynamicMesh(true);
		ODEBody1[i]->rotateAboutGlobalAxisRad(1, 0, 1, M_PI);
		ODEBody1[i]->rotateAboutGlobalAxisRad(0, 0, 1, i * 2 * M_PI / 12);
		object1[i]->rotateAboutGlobalAxisRad(0, 0, 1, i * 2 * M_PI / 12);
		ODEBody1[i]->setLocalPos(cos(i * 2 * M_PI / 12), sin(i * 2 * M_PI / 12), 0);
		AddDetectionPlane(i, ODEBody1[i]);
		crossing[i] = false;
		end1[i] = false;
		end2[i] = false;

	}

	//////////////////////////////////////////////////////////////////////////
	// 6 ODE INVISIBLE WALLS
	//////////////////////////////////////////////////////////////////////////

	// we create 6 static walls to contains the 3 cubes within a limited workspace
  //  ODEGPlane0 = new cODEGenericBody(ODEWorld);
	ODEGPlane1 = new cODEGenericBody(ODEWorld);
	// ODEGPlane2 = new cODEGenericBody(ODEWorld);
   //  ODEGPlane3 = new cODEGenericBody(ODEWorld);
	// ODEGPlane4 = new cODEGenericBody(ODEWorld);
   //  ODEGPlane5 = new cODEGenericBody(ODEWorld);

	w = 1.0;
	//ODEGPlane0->createStaticPlane(cVector3d(0.0, 0.0, 2.0 * w), cVector3d(0.0, 0.0, -1.0));
	ODEGPlane1->createStaticPlane(cVector3d(0.0, 0.0, -w), cVector3d(0.0, 0.0, 1.0));
	// ODEGPlane2->createStaticPlane(cVector3d(0.0, w, 0.0), cVector3d(0.0, -1.0, 0.0));
	// ODEGPlane3->createStaticPlane(cVector3d(0.0, -w, 0.0), cVector3d(0.0, 1.0, 0.0));
   //  ODEGPlane4->createStaticPlane(cVector3d(w, 0.0, 0.0), cVector3d(-1.0, 0.0, 0.0));
	// ODEGPlane5->createStaticPlane(cVector3d(-0.8 * w, 0.0, 0.0), cVector3d(1.0, 0.0, 0.0));


	 //////////////////////////////////////////////////////////////////////////
	 // GROUND
	 //////////////////////////////////////////////////////////////////////////

	 // create a mesh that represents the ground
	cMesh* ground = new cMesh();
	ODEWorld->addChild(ground);

	// create a plane
	double groundSize = 3.0;
	cCreatePlane(ground, groundSize, groundSize);

	// position ground in world where the invisible ODE plane is located (ODEGPlane1)
	ground->setLocalPos(0.0, 0.0, -1.0);

	// define some material properties and apply to mesh
	cMaterial matGround;
	matGround.setStiffness(0.3 * maxStiffness);
	matGround.setDynamicFriction(0.2);
	matGround.setStaticFriction(0.0);
	matGround.setWhite();
	matGround.m_emission.setGrayLevel(0.3);
	ground->setMaterial(matGround);

	// setup collision detector
	ground->createAABBCollisionDetector(toolRadius);

}

void cAroundTheClockLevel::moveCamera() {

	cVector3d cameraPos = m_camera->getLocalPos();

	if (pressedKey == GLFW_KEY_W) {
		movementVector = m_camera->getLookVector();
	}
	else if (pressedKey == GLFW_KEY_D) {
		movementVector = m_camera->getRightVector();
	}
	else if (pressedKey == GLFW_KEY_A) {
		movementVector = m_camera->getRightVector();
		movementVector.negate();
	}
	else if (pressedKey == GLFW_KEY_S) {

		movementVector = m_camera->getLookVector();
		movementVector.negate();

	}
	else if (pressedKey == GLFW_KEY_SPACE) {
		movementVector.zero();
		movementVector.z(1);
	}
	else if (pressedKey == GLFW_KEY_LEFT_CONTROL) {
		movementVector.zero();
		movementVector.z(-1);
	}
	else {
		movementVector.zero();
	}

	movementVector.mul(deltaTime);
	movementVector.mul(moveSpeed);
	cameraPos.add(movementVector);
	m_camera->setLocalPos(cameraPos);

}

void cAroundTheClockLevel::keyCallback(GLFWwindow* a_window, int a_key, int a_scancode, int a_action, int a_mods) {

	// filter calls that only include a key press
	if ((a_action != GLFW_PRESS) && (a_action != GLFW_REPEAT))
	{
		pressedKey = -1;
		return;
	}

	// option - exit
	else if ((a_key == GLFW_KEY_ESCAPE) || (a_key == GLFW_KEY_Q))
	{
		glfwSetWindowShouldClose(a_window, GLFW_TRUE);
	}

	pressedKey = a_key;

}

void cAroundTheClockLevel::updateGraphics() {

	currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	moveCamera();


}

void cAroundTheClockLevel::updateHaptics(void)
{
	/////////////////////////////////////////////////////////////////////
		// SIMULATION TIME
		/////////////////////////////////////////////////////////////////////

		// stop the simulation clock
	simClock.stop();
	// read the time increment in seconds
	timeInterval = cClamp(simClock.getCurrentTimeSeconds(), 0.0001, 0.001);
	for (int i = 0; i < 12; i++) {
		ComputeCrossing(DetectSphere, i);
	}

	// restart the simulation clock
	simClock.reset();
	simClock.start();

	// compute global reference frames for each object
	m_world->computeGlobalPositions(true);
	bool caught[MAX_DEVICES];
	int vientdegrip = -1;
	for (int i = 0; i < m_numTools; i++) {
		ODEBody2[i]->setLocalPos(m_tools[i]->m_hapticPointFinger->getLocalPosProxy() + ODEBody2[i]->getLocalRot() * cVector3d(0.2, 0, 0));
		ODEBody2[i]->setLocalRot(m_tools[i]->getDeviceLocalRot());
		ODEBody3[i]->setLocalPos(m_tools[i]->m_hapticPointThumb->getLocalPosProxy() + ODEBody3[i]->getLocalRot() * cVector3d(0.2, 0, 0));
		ODEBody3[i]->setLocalRot(m_tools[i]->getDeviceLocalRot());


		/////////////////////////////////////////////////////////////////////
		// HAPTIC FORCE COMPUTATION
		/////////////////////////////////////////////////////////////////////



		// update position and orientation of m_tools[i]
		m_tools[i]->updateFromDevice();

		// compute interaction forces
		m_tools[i]->computeInteractionForces();

		// send forces to haptic device
		m_tools[i]->applyToDevice();
		caught[i] = true;
		int numInteractionPoints = m_tools[i]->getNumHapticPoints();
		for (int j = 0; j < numInteractionPoints; j++)
		{
			// get pointer to next interaction point of m_tools[i]
			cHapticPoint* interactionPoint = m_tools[i]->getHapticPoint(j);

			if (!interactionPoint->isInContact(object0)) {
				caught[i] = false;
			}
		}
		if (caught[i] && !previousframecaught[i]) {
			vientdegrip = i;
		}
		if (!caught[i]) {
			previousframecaught[i] = false;
		}

	}
	if (vientdegrip != -1) {
		gripperCatchingIndex = vientdegrip;
	}
	if (gripperCatchingIndex != -1 && !caught[gripperCatchingIndex]) {
		gripperCatchingIndex = -1;
		for (int i = 0; i < m_numTools; i++) {
			if (caught[i]) {
				gripperCatchingIndex = i;
			}
		}
	}
	if (gripperCatchingIndex == -1) {
		ODEWorld->setGravity(cVector3d(0, 0, -45.81));

	}
	for (int i = 0; i < m_numTools; i++) {
		/////////////////////////////////////////////////////////////////////
		// DYNAMIC SIMULATION
		/////////////////////////////////////////////////////////////////////

		// for each interaction point of the m_tools[i] we look for any contact events
		// with the environment and apply forces accordingly
		int numInteractionPoints = m_tools[i]->getNumHapticPoints();
		for (int j = 0; j < numInteractionPoints; j++)
		{
			// get pointer to next interaction point of m_tools[i]
			cHapticPoint* interactionPoint = m_tools[i]->getHapticPoint(j);


			if (gripperCatchingIndex == i) {
				if (vientdegrip == i) {
					startrotGripper[i].copyfrom(m_tools[i]->getDeviceGlobalRot());
					startrotCube.copyfrom(ODEBody0->getLocalRot());
					startposGripper[i].copyfrom(m_tools[i]->getDeviceLocalPos());
					startposCube.copyfrom(ODEBody0->getLocalPos());
					previousframecaught[i] = true;
					ODEWorld->setGravity(cVector3d(0, 0, 0));
				}
				else {
					cMatrix3d ObjT0Invert;
					cMatrix3d ArmT;
					cMatrix3d ArmT0Invert;
					cMatrix3d ObjT0;
					ObjT0Invert.copyfrom(startrotCube);
					ObjT0Invert.invert();
					ArmT.copyfrom(m_tools[i]->getDeviceLocalRot());
					ArmT0Invert.copyfrom(startrotGripper[i]);
					ArmT0Invert.invert();
					ObjT0.copyfrom(startrotCube);

					cVector3d rotvec = toAxisAngleVec(ObjT0Invert * ArmT * ArmT0Invert * ObjT0);
					double rotangle = toAxisAngleAngle(ObjT0Invert * ArmT * ArmT0Invert * ObjT0);
					if (isnan(rotangle)) {
						rotangle = 0;
					}
					bool test = true;
					if (test) {
						ODEBody0->setLocalRot(startrotCube);
						ODEBody0->rotateAboutLocalAxisRad(rotvec, rotangle);
					}
					else {
						ODEBody0->setLocalRot(startrotCube * ObjT0Invert * ArmT * ArmT0Invert * ObjT0);
					}
				}

			}
			// check all contact points
			int numContacts = interactionPoint->getNumCollisionEvents();
			for (int k = 0; k < numContacts; k++)
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

					ODEobject->addExternalForceAtPoint(-10 * interactionPoint->getLastComputedForce(),
						collisionEvent->m_globalPos);
				}
			}

		}

	}
	// update simulation
	ODEWorld->updateDynamics(timeInterval);
}

double cAroundTheClockLevel::toAxisAngleAngle(cMatrix3d m) {
	double angle, x, y, z; // variables for result
	double epsilon = 1e-15; // margin to allow for rounding errors
	double epsilon2 = 1e-15; // margin to distinguish between 0 and 180 degrees
	// optional check that input is pure rotation, 'isRotationMatrix' is defined at:
	// https://www.euclideanspace.com/maths/algebra/matrix/orthogonal/rotation/

	if ((abs(m(0, 1) - m(1, 0)) < epsilon)
		&& (abs(m(0, 2) - m(2, 0)) < epsilon)
		&& (abs(m(1, 2) - m(2, 1)) < epsilon)) {
		// singularity found
		// first check for identity matrix which must have +1 for all terms
		//  in leading diagonaland zero in other terms
		if ((abs(m(0, 1) + m(1, 0)) < epsilon2)
			&& (abs(m(0, 2) + m(2, 0)) < epsilon2)
			&& (abs(m(1, 2) + m(2, 1)) < epsilon2)
			&& (abs(m(0, 0) + m(1, 1) + m(2, 2) - 3) < epsilon2)) {
			// this singularity is identity matrix so angle = 0
			return 0; // zero angle, arbitrary axis
		}
		// otherwise this singularity is angle = 180
		angle = M_PI;
		double xx = (m(0, 0) + 1) / 2;
		double yy = (m(1, 1) + 1) / 2;
		double zz = (m(2, 2) + 1) / 2;
		double xy = (m(0, 1) + m(1, 0)) / 4;
		double xz = (m(0, 2) + m(2, 0)) / 4;
		double yz = (m(1, 2) + m(2, 1)) / 4;
		if ((xx > yy) && (xx > zz)) { // m(0,0) is the largest diagonal term
			if (xx < epsilon) {

				x = 0;
				y = 0.7071;
				z = 0.7071;
			}
			else {
				x = sqrt(xx);
				y = xy / x;
				z = xz / x;
			}
		}
		else if (yy > zz) { // m(1,1) is the largest diagonal term
			if (yy < epsilon) {

				x = 0.7071;
				y = 0;
				z = 0.7071;
			}
			else {
				y = sqrt(yy);
				x = xy / y;
				z = yz / y;
			}
		}
		else { // m(2,2) is the largest diagonal term so base result on this
			if (zz < epsilon) {

				x = 0.7071;
				y = 0.7071;
				z = 0;
			}
			else {
				z = sqrt(zz);
				x = xz / z;
				y = yz / z;
			}
		}
		return angle; // return 180 deg rotation
	}
	// as we have reached here there are no singularities so we can handle normally
	double s = sqrt((m(2, 1) - m(1, 2)) * (m(2, 1) - m(1, 2))
		+ (m(0, 2) - m(2, 0)) * (m(0, 2) - m(2, 0))
		+ (m(1, 0) - m(0, 1)) * (m(1, 0) - m(0, 1))); // used to normalise
	if (abs(s) < 0.001) s = 1;
	// prevent divide by zero, should not happen if matrix is orthogonal and should be
	// caught by singularity test above, but I've left it in just in case
	angle = acos((m(0, 0) + m(1, 1) + m(2, 2) - 1) / 2);
	x = (m(2, 1) - m(1, 2)) / s;
	y = (m(0, 2) - m(2, 0)) / s;
	z = (m(1, 0) - m(0, 1)) / s;
	return angle;
}

void cAroundTheClockLevel::InitializeNeedleDetect() {
	double size = 0.4;
	for (int i = 0; i < 5; i++) {
		DetectSphere[i] = new cMesh();
		cCreateSphere(DetectSphere[i], sphereSize);
		ODEBody0->addChild(DetectSphere[i]);
		DetectSphere[i]->translate(size * (-1 + (double)i / (double)2), 0, 0);
	}
}

cVector3d cAroundTheClockLevel::toAxisAngleVec(cMatrix3d m) {
	double angle, x, y, z; // variables for result
	double epsilon = 1e-15; // margin to allow for rounding errors
	double epsilon2 = 1e-15; // margin to distinguish between 0 and 180 degrees
	// optional check that input is pure rotation, 'isRotationMatrix' is defined at:
	// https://www.euclideanspace.com/maths/algebra/matrix/orthogonal/rotation/

	if ((abs(m(0, 1) - m(1, 0)) < epsilon)
		&& (abs(m(0, 2) - m(2, 0)) < epsilon)
		&& (abs(m(1, 2) - m(2, 1)) < epsilon)) {
		// singularity found
		// first check for identity matrix which must have +1 for all terms
		//  in leading diagonaland zero in other terms
		if ((abs(m(0, 1) + m(1, 0)) < epsilon2)
			&& (abs(m(0, 2) + m(2, 0)) < epsilon2)
			&& (abs(m(1, 2) + m(2, 1)) < epsilon2)
			&& (abs(m(0, 0) + m(1, 1) + m(2, 2) - 3) < epsilon2)) {
			// this singularity is identity matrix so angle = 0
			return cVector3d(1, 0, 0); // zero angle, arbitrary axis
		}
		// otherwise this singularity is angle = 180
		angle = M_PI;
		double xx = (m(0, 0) + 1) / 2;
		double yy = (m(1, 1) + 1) / 2;
		double zz = (m(2, 2) + 1) / 2;
		double xy = (m(0, 1) + m(1, 0)) / 4;
		double xz = (m(0, 2) + m(2, 0)) / 4;
		double yz = (m(1, 2) + m(2, 1)) / 4;
		if ((xx > yy) && (xx > zz)) { // m(0,0) is the largest diagonal term
			if (xx < epsilon) {
				x = 0;
				y = 0.7071;
				z = 0.7071;
			}
			else {
				x = sqrt(xx);
				y = xy / x;
				z = xz / x;
			}
		}
		else if (yy > zz) { // m(1,1) is the largest diagonal term
			if (yy < epsilon) {
				x = 0.7071;
				y = 0;
				z = 0.7071;
			}
			else {
				y = sqrt(yy);
				x = xy / y;
				z = yz / y;
			}
		}
		else { // m(2,2) is the largest diagonal term so base result on this
			if (zz < epsilon) {
				x = 0.7071;
				y = 0.7071;
				z = 0;
			}
			else {
				z = sqrt(zz);
				x = xz / z;
				y = yz / z;
			}
		}
		return cVector3d(x, y, z); // return 180 deg rotation
	}
	// as we have reached here there are no singularities so we can handle normally
	double s = sqrt((m(2, 1) - m(1, 2)) * (m(2, 1) - m(1, 2))
		+ (m(0, 2) - m(2, 0)) * (m(0, 2) - m(2, 0))
		+ (m(1, 0) - m(0, 1)) * (m(1, 0) - m(0, 1))); // used to normalise
	if (abs(s) < 0.001) s = 1;
	// prevent divide by zero, should not happen if matrix is orthogonal and should be
	// caught by singularity test above, but I've left it in just in case
	angle = acos((m(0, 0) + m(1, 1) + m(2, 2) - 1) / 2);
	x = (m(2, 1) - m(1, 2)) / s;
	y = (m(0, 2) - m(2, 0)) / s;
	z = (m(1, 0) - m(0, 1)) / s;
	return cVector3d(x, y, z);
}

void cAroundTheClockLevel::AddDetectionPlane(int i, cODEGenericBody* ring) {
	DetectionPlanes[i] = new cMesh();
	ODEWorld->addChild(DetectionPlanes[i]);
	//cCreatePanel(DetectionPlanes[i], 1, 1,.5,8,ring->getLocalPos(),cMatrix3d(1,0,0,0));
	cCreateCylinder(DetectionPlanes[i], 0.001, 0.1, 32U, 1U, 1U, true, true, cVector3d(0, 0, 0), cMatrix3d(1, 0, 1, M_PI));
	DetectionPlanes[i]->m_material->setRed();
	DetectionPlanes[i]->setLocalPos(ring->getLocalPos());
	DetectionPlanes[i]->rotateAboutGlobalAxisRad(0, 0, 1, i * 2 * M_PI / 12);
	DetectionPlanes[i]->createAABBCollisionDetector(0.02);
	DetectionPlanes[i]->setHapticEnabled(false);
	DetectionPlanesFinished[i] = false;
}

void cAroundTheClockLevel::ComputeCrossing(cMesh* spheres[], int i) {
	if (DetectionPlanesFinished[i]) return;
	cVector3d pos0 = spheres[0]->getGlobalPos();
	cVector3d pos1 = spheres[1]->getGlobalPos();
	cVector3d pos2 = spheres[3]->getGlobalPos();
	cVector3d pos3 = spheres[4]->getGlobalPos();
	if (DetectionPlanes[i]->computeCollisionDetection(pos0, pos3, recorder, settings)) {
		crossing[i] = true;
	}
	else {
		if (end1[i] && end2[i]) {
			DetectionPlanes[i]->m_material->m_emission.setGreen();
			DetectionPlanesFinished[i] = true;
		}
		if (!DetectionPlanesFinished[i]) {
			DetectionPlanes[i]->m_material->m_emission.setRed();
			end1[i] = false;
			end2[i] = false;
		}
	}
	if (crossing[i]) {
		if (DetectionPlanes[i]->computeCollisionDetection(pos0, pos1, recorder, settings)) {
			end1[i] = true;
		}
		if (DetectionPlanes[i]->computeCollisionDetection(pos2, pos3, recorder, settings)) {
			end2[i] = true;
		}
	}
}
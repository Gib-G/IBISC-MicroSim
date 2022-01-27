#include "cFirstLevel.h"

using namespace std;


#define RESOURCE_PATH(p)    (char*)((a_resourceRoot+string(p)).c_str())

cFirstLevel::cFirstLevel(const std::string a_resourceRoot,
	const int a_numDevices,
	std::shared_ptr<cGenericHapticDevice> a_hapticDevice0,
	std::shared_ptr<cGenericHapticDevice> a_hapticDevice1,
	cLevelHandler*  a_levelHandler) : cGenericLevel(a_resourceRoot, a_numDevices, a_hapticDevice0, a_hapticDevice1) {

	//--------------------------------------------------------------------------
	// CREATE ENVIRONMENT MAP
	//--------------------------------------------------------------------------

	////////////////////////////////////////////////////////////////////////////
	// CREATE GROUND
	////////////////////////////////////////////////////////////////////////////
	moveSpeed = 10;
	m_levelHandler = a_levelHandler;
	m_second = new cGenericLevel(a_resourceRoot, a_numDevices, a_hapticDevice0, a_hapticDevice1);

	// create a virtual mesh
	cMesh* ground = new cMesh();
	bool fileload;
	// add object to world
	m_world->addChild(ground);

	// set the position of the object at the center of the world
	ground->setLocalPos(0.0, 0.0, -0.3);

	// create cube
	cCreatePlane(ground, 5, 5);
	ground->setUseDisplayList(true);
	ground->deleteCollisionDetector();

	// Since we don't need to see our polygons from both sides, we enable culling.
	ground->setUseCulling(false);

	// disable material properties and lighting
	ground->setUseMaterial(false);


	////////////////////////////////////////////////////////////////////////////
	// CREATE BASE
	////////////////////////////////////////////////////////////////////////////

	// create a virtual mesh
	cMesh* block = new cMesh();

	// add object to world
	m_world->addChild(block);

	// set the position of the object at the center of the world
	block->setLocalPos(0.0, 0.0, -0.30);

	// create cube
	cCreateBox(block, 0.22, 0.26, 0.58);
	block->setUseDisplayList(true);
	block->deleteCollisionDetector();
	block->m_material->setGrayLevel(0.2);


	//--------------------------------------------------------------------------
	// CREATE ENVIRONMENT GLOBE
	//--------------------------------------------------------------------------

	// create a virtual mesh
	cMesh* globe = new cMesh();

	// add object to world
	m_world->addChild(globe);

	// set the position of the object at the center of the world
	globe->setLocalPos(0.0, 0.0, 0.0);

	// create cube
	cCreateSphere(globe, 6.0, 360, 360);
	globe->setUseDisplayList(true);
	globe->deleteCollisionDetector();

	// create a texture
	cTexture2dPtr textureSpace = cTexture2d::create();

	fileload = textureSpace->loadFromFile(RESOURCE_PATH("../resources/images/infinity.jpg"));
	if (!fileload)
	{
#if defined(_MSVC)
		fileload = textureSpace->loadFromFile("../../../bin/resources/images/infinity.jpg");
#endif
	}
	if (!fileload)
	{
		cout << "Error - Texture image failed to load correctly." << endl;
	}

	// apply texture to object
	globe->setTexture(textureSpace);

	// enable texture rendering 
	globe->setUseTexture(true);

	// Since we don't need to see our polygons from both sides, we enable culling.
	globe->setUseCulling(false);

	// disable material properties and lighting
	globe->setUseMaterial(false);

	// create tools
	m_tools[0] = NULL;
	m_tools[1] = NULL;
	m_tool0 = NULL;
	m_tool1 = NULL;

	m_numTools = cMin(a_numDevices, 2);

	cMesh* mesh = new cMesh();
	cMatrix3d rot;
	rot.identity();
	rot.rotateAboutLocalAxisDeg(cVector3d(1, 0, 0), 90);
	cCreateRing(mesh, 0.001, 0.005, 4, 16, cVector3d(0, 0, 0), rot);
	mesh->m_material->setWhite();
	mesh->setTransparencyLevel(0.4f);

	if (m_numTools > 0)
	{
		m_tool0 = new cToolGripper(m_world);
		m_world->addChild(m_tool0);
		m_tool0->setHapticDevice(shared_ptr<cGenericHapticDevice>(a_hapticDevice0));
		m_tool0->setRadius(m_toolRadius);
		m_tool0->enableDynamicObjects(true);
		m_tool0->setWaitForSmallForce(true);
		m_tool0->start();
		m_tools[0] = m_tool0;

		cMesh* mesh0 = mesh->copy();
		m_tool0->m_hapticPointThumb->m_sphereProxy->addChild(mesh0);

		cMesh* mesh1 = mesh->copy();
		m_tool0->m_hapticPointFinger->m_sphereProxy->addChild(mesh1);
	}

	if (m_numTools > 1)
	{
		m_tool1 = new cToolGripper(m_world);
		m_world->addChild(m_tool1);
		m_tool1->setHapticDevice(a_hapticDevice1);
		m_tool1->setRadius(m_toolRadius);
		m_tool1->enableDynamicObjects(true);
		m_tool1->setWaitForSmallForce(true);
		m_tool1->start();
		m_tools[1] = m_tool1;

		cMesh* mesh0 = mesh->copy();
		m_tool1->m_hapticPointThumb->m_sphereProxy->addChild(mesh0);

		cMesh* mesh1 = mesh->copy();
		m_tool1->m_hapticPointFinger->m_sphereProxy->addChild(mesh1);
	}

}

void cFirstLevel::moveCamera() {

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

void cFirstLevel::keyCallback(GLFWwindow* a_window, int a_key, int a_scancode, int a_action, int a_mods) {

	// filter calls that only include a key press
	if ((a_action != GLFW_PRESS) && (a_action != GLFW_REPEAT))
	{
		pressedKey = -1;
		return;
	}
	else if ((a_key == GLFW_KEY_1)) {
		m_levelHandler->setLevel(m_second);
	}

	// option - exit
	else if ((a_key == GLFW_KEY_ESCAPE) || (a_key == GLFW_KEY_Q))
	{
		glfwSetWindowShouldClose(a_window, GLFW_TRUE);
	}

	pressedKey = a_key;

}

void cFirstLevel::updateGraphics() {

	currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	moveCamera();


}

void cFirstLevel::updateHaptics() {
	// compute global reference frames for each object
	m_world->computeGlobalPositions(true);

	// update positions
	for (int i = 0; i < m_numTools; i++)
	{
		m_tools[i]->updateFromDevice();
		cMatrix3d rot = m_tools[i]->getDeviceGlobalRot();
		m_tools[i]->m_hapticPointFinger->m_sphereProxy->setLocalRot(rot);
		m_tools[i]->m_hapticPointThumb->m_sphereProxy->setLocalRot(rot);
	}

	// compute interaction forces
	for (int i = 0; i < m_numTools; i++)
	{
		m_tools[i]->computeInteractionForces();
	}

	// apply forces to haptic devices
	for (int i = 0; i < m_numTools; i++)
	{
		m_tools[i]->setDeviceGlobalTorque(m_torqueGain * m_tools[i]->getDeviceGlobalTorque());
		m_tools[i]->applyToDevice();
	}

	// apply forces to ODE objects
	for (int i = 0; i < m_numTools; i++)
	{
		// for each interaction point of the tool we look for any contact events
		// with the environment and apply forces accordingly
		int numInteractionPoints = m_tools[i]->getNumHapticPoints();
		for (int j = 0; j < numInteractionPoints; j++)
		{
			// get pointer to next interaction point of tool
			cHapticPoint* interactionPoint = m_tools[i]->getHapticPoint(j);

			// check all contact points
			int numContacts = interactionPoint->getNumCollisionEvents();
			for (int k = 0; k < numContacts; k++)
			{
				cCollisionEvent* collisionEvent = interactionPoint->getCollisionEvent(k);

				// given the mesh object we may be touching, we search for its owner which
				// could be the mesh itself or a multi-mesh object. Once the owner found, we
				// look for the parent that will point to the ODE object itself.
				cGenericObject* object = collisionEvent->m_object->getOwner()->getOwner();
				
			}
		}
	}

	// retrieve simulation time and compute next interval
	double time = simClock.getCurrentTimeSeconds();
	double nextSimInterval = cClamp(time, 0.0001, 0.001);

	// reset clock
	simClock.reset();
	simClock.start();

	// update simulation
}
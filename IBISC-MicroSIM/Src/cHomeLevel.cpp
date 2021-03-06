#include "cHomeLevel.h"

using namespace std;


#define RESOURCE_PATH(p)    (char*)((a_resourceRoot+string(p)).c_str())

cHomeLevel::cHomeLevel(const std::string a_resourceRoot,
	const int a_numDevices,
	std::shared_ptr<cGenericHapticDevice> a_hapticDevice0,
	std::shared_ptr<cGenericHapticDevice> a_hapticDevice1,
	cLevelHandler* levelHandler,
	std::string NC) : cToolCursorLevel(a_resourceRoot, a_numDevices, a_hapticDevice0, a_hapticDevice1) {

	defaultPos = cVector3d(2,0,-0.5);
	m_levelHandler = levelHandler;
	m_grid = new cGridLevel(a_resourceRoot, a_numDevices, a_hapticDevice0, a_hapticDevice1,NC);
	m_around = new cAroundTheClockLevel(a_resourceRoot, a_numDevices, a_hapticDevice0, a_hapticDevice1, NC);

	//--------------------------------------------------------------------------
	// CREATE ENVIRONMENT MAP
	//--------------------------------------------------------------------------

	////////////////////////////////////////////////////////////////////////////
	// CREATE GROUND
	////////////////////////////////////////////////////////////////////////////
	moveSpeed = 10;
	double maxStiffness;

	toolRadius = 0.01;

	if (a_numDevices > 0) {
		cHapticDeviceInfo hapticDeviceInfo = a_hapticDevice0->getSpecifications();
		double workspaceScaleFactor = m_tools[0]->getWorkspaceScaleFactor();
		maxStiffness = hapticDeviceInfo.m_maxLinearStiffness / workspaceScaleFactor;
	}
	else {
		maxStiffness = 3;
	}

	//
	//Needle
	//
	bool fileload;
	double size;
	cMatrix3d rot;
	//----------------------------------------------------------------------

	defaultPos = cVector3d(1.94, 0, -0.62);
	defaultRot.setCol0(cVector3d(1,0,0));
	defaultRot.setCol1(cVector3d(0,1,0));
	defaultRot.setCol2(cVector3d(0,0,1));

	light = new cSpotLight(m_world);

	// add light to world
	m_world->addChild(light);

	// enable light source
	light->setEnabled(true);

	// position the light source
	light->setLocalPos(3.5, 2.0, 2.0);

	// define the direction of the light beam
	light->setDir(-3.5, -2.0, -2.0);

	// set light cone half angle
	light->setCutOffAngleDeg(50);

	gridLevelButton = new cMesh();
	m_world->addChild(gridLevelButton);
	cMaterial mat;
	rot.identity();
	rot.rotateAboutGlobalAxisDeg(cVector3d(0, 1, 0), 90);
	cCreatePanel(gridLevelButton, .5, .5, .1, 8, cVector3d(0, 0, 0), rot);
	gridLevelButton->rotateAboutGlobalAxisDeg(cVector3d(1, 0, 0), 90);
	gridLevelButton->translate(cVector3d(-.4, 0.5, -0.45));
	/*
	cFontPtr font = NEW_CFONTCALIBRI20();
	cLabel* label = new cLabel(font);
	label->m_fontColor.setBlack();
	label->setText("Reset");
	gridLevelButton->addChild(label);*/

	gridLevelButton->m_material->setStiffness(0.5 * maxStiffness);
	gridLevelButton->m_material->setStaticFriction(0.30);
	gridLevelButton->m_material->setDynamicFriction(0.30);

	// compute collision detection algorithm
	mat.setHapticTriangleSides(true, true);
	mat.setDynamicFriction(0.2);
	mat.setStaticFriction(0.2);
	mat.setStiffness(0.3 * maxStiffness);
	mat.setTextureLevel(1);
	mat.setHapticTriangleSides(true, false);
	gridLevelButton->m_texture = cTexture2d::create();

	fileload = gridLevelButton->m_texture->loadFromFile(ROOT_DIR "resources/Images/GridButton.png");
	if (!fileload)
	{
#if defined(_MSVC)
		fileload = gridLevelButton->m_texture->loadFromFile(ROOT_DIR "Resources/Images/GridButton.png");
#endif
	}
	if (!fileload)
	{
		cout << "Error - Texture image failed to load correctly." << endl;
	}
	gridLevelButton->m_texture->setEnvironmentMode(GL_DECAL);
	// enable texture rendering 
	gridLevelButton->setUseTexture(true);

	// Since we don't need to see our polygons from both sides, we enable culling.
	gridLevelButton->setUseCulling(false, true);


	// compute collision detection algorithm
	gridLevelButton->createAABBCollisionDetector(toolRadius);

	aroundLevelButton = new cMesh();
	m_world->addChild(aroundLevelButton);
	rot.identity();
	rot.rotateAboutGlobalAxisDeg(cVector3d(0, 1, 0), 90);
	cCreatePanel(aroundLevelButton, .5, .5, .1, 8, cVector3d(0, 0, 0), rot);
	aroundLevelButton->rotateAboutGlobalAxisDeg(cVector3d(1, 0, 0), 90);
	aroundLevelButton->translate(cVector3d(-.4, 0.5, -0.45));
	/*
	cFontPtr font = NEW_CFONTCALIBRI20();
	cLabel* label = new cLabel(font);
	label->m_fontColor.setBlack();
	label->setText("Reset");
	aroundLevelButton->addChild(label);*/

	aroundLevelButton->m_material->setStiffness(0.5 * maxStiffness);
	aroundLevelButton->m_material->setStaticFriction(0.30);
	aroundLevelButton->m_material->setDynamicFriction(0.30);

	// compute collision detection algorithm
	mat.setHapticTriangleSides(true, true);
	mat.setDynamicFriction(0.2);
	mat.setStaticFriction(0.2);
	mat.setStiffness(0.3 * maxStiffness);
	mat.setTextureLevel(1);
	mat.setHapticTriangleSides(true, false);
	aroundLevelButton->m_texture = cTexture2d::create();
	fileload = aroundLevelButton->m_texture->loadFromFile(ROOT_DIR "resources/Images/ATCButton.png");
	if (!fileload)
	{
#if defined(_MSVC)
		fileload = aroundLevelButton->m_texture->loadFromFile(ROOT_DIR "Resources/Images/ATCButton.png");
#endif
	}
	if (!fileload)
	{
		cout << "Error - Texture image failed to load correctly." << endl;
	}
	aroundLevelButton->m_texture->setEnvironmentMode(GL_DECAL);
	// enable texture rendering 
	aroundLevelButton->setUseTexture(true);

	// Since we don't need to see our polygons from both sides, we enable culling.
	aroundLevelButton->setUseCulling(false, true);


	// compute collision detection algorithm
	aroundLevelButton->createAABBCollisionDetector(toolRadius);
	aroundLevelButton->translate(cVector3d(0, -1,0));

	//--------------------------------------------------------------------------
	// CREATE ENVIRONMENT GLOBE
	//--------------------------------------------------------------------------

	// create a virtual mesh
	cMesh* globe = new cMesh();

	// add object to m_world
	m_world->addChild(globe);

	// set the position of the object at the center of the m_world
	globe->setLocalPos(0.0, 0.0, 0.0);

	// create cube
	cCreateSphere(globe, 6.0, 360, 360);
	globe->setUseDisplayList(true);
	globe->deleteCollisionDetector();

	// create a texture
	cTexture2dPtr textureSpace = cTexture2d::create();

	fileload = textureSpace->loadFromFile(ROOT_DIR "resources/Images/grey.jpg");
	if (!fileload)
	{
#if defined(_MSVC)
		fileload = textureSpace->loadFromFile(ROOT_DIR "Resources/Images/grey.jpg");
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

	for (int i = 0; i < m_numTools; i++)
	{
		state[i] = IDLE;
		selectedObject[i] = NULL;
		InitialPos[i] = m_tools[i]->getDeviceGlobalPos();
	}

}

void cHomeLevel::moveCamera() {

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

void cHomeLevel::keyCallback(GLFWwindow* a_window, int a_key, int a_scancode, int a_action, int a_mods) {

	// filter calls that only include a key press
	if ((a_action != GLFW_PRESS) && (a_action != GLFW_REPEAT))
	{
		pressedKey = -1;
		return;
	}

	pressedKey = a_key;

}

void cHomeLevel::updateGraphics() {

	currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	moveCamera();


}

void cHomeLevel::updateHaptics() {

	/////////////////////////////////////////////////////////////////////
		// SIMULATION TIME
		/////////////////////////////////////////////////////////////////////

		// stop the simulation simClock
	simClock.stop();

	// read the time increment in seconds
	double timeInterval = simClock.getCurrentTimeSeconds();

	// restart the simulation simClock
	simClock.reset();
	simClock.start();

	for (int i = 0; i < m_numTools; i++)
	{

		/////////////////////////////////////////////////////////////////////
		// HAPTIC FORCE COMPUTATION
		/////////////////////////////////////////////////////////////////////

		// compute global reference frames for each object
		m_world->computeGlobalPositions(true);

		// update position and orientation of tool
		m_tools[i]->updateFromDevice();

		// compute interaction forces
		m_tools[i]->computeInteractionForces();

		// get interaction forces magnitude
		//force[i] = 5 * tool[i]->getDeviceGlobalForce().length();
		// send forces to haptic device
		m_tools[i]->applyToDevice();

		// get status of user switch
		bool button = m_tools[i]->getUserSwitch(0);
		/////////////////////////////////////////////////////////////////////
	// INTERACTION WITH gridLevelButton
	/////////////////////////////////////////////////////////////////////
		if (m_tools[i]->isInContact(gridLevelButton) && button == true) {
			m_levelHandler->setLevel(m_grid);
		}
		else if (m_tools[i]->isInContact(aroundLevelButton) && button == true) {
			m_levelHandler->setLevel(m_around);
		}
	}

}

#include <chai3d.h>
//------------------------------------------------------------------------------
#include <GLFW/glfw3.h>
//------------------------------------------------------------------------------
#include <COculus.h>
#include <CBullet.h>
//------------------------------------------------------------------------------
using namespace chai3d;
using namespace std;
//------------------------------------------------------------------------------
// maximum number of devices supported by this application
const int MAX_DEVICES = 2;
//------------------------------------------------------------------------------
//BULLET 
//------------------------------------------------------------------------------
cBulletStaticPlane* bulletGround;
cBulletStaticPlane* bulletInvisibleWall1;
cBulletStaticPlane* bulletInvisibleWall2;
cBulletStaticPlane* bulletInvisibleWall3;
cBulletStaticPlane* bulletInvisibleWall4;
cBulletStaticPlane* bulletInvisibleWall5;
double planeWidth = 1.0;
//------------------------------------------------------------------------------
// CHAI3D
//------------------------------------------------------------------------------

// a world that contains all objects of the virtual environment
cBulletWorld* world;

// a camera to render the world in the window display
cCamera* camera;

// a light source
cSpotLight* light;

// number of haptic devices detected
int numHapticDevices = 0;

// a pointer to the current haptic device
cGenericHapticDevicePtr hapticDevice[MAX_DEVICES];

// labels to display each haptic device model
cLabel* labelHapticDeviceModel[MAX_DEVICES];

// labels to display the position [m] of each haptic device
cLabel* labelHapticDevicePosition[MAX_DEVICES];

// global variable to store the position [m] of each haptic device
cVector3d hapticDevicePosition[MAX_DEVICES];
// a haptic device handler
cHapticDeviceHandler* handler;

double workspaceScaleFactor;

// stiffness of virtual spring
double linGain = 0.05;
double angGain = 0.03;
double linG;
double angG;
double linStiffness = 4000;
double angStiffness = 30;

// a virtual tool representing the haptic device in the scene
cToolCursor* tool[MAX_DEVICES];

// a label to display the rates [Hz] at which the simulation is running
cLabel* labelRates;

// a virtual mesh like object
cBulletMultiMesh* object;
cMultiMesh* target;
cMultiMesh* needle;
cMultiMesh* classroom;
cBulletBox* bulletBox0;
cBulletBox* bulletBox1;
cBulletBox* test;

// indicates if the haptic simulation currently running
bool simulationRunning = false;

// indicates if the haptic simulation has terminated
bool simulationFinished = false;

bool camSim = false;


// a frequency counter to measure the simulation graphic rate
cFrequencyCounter freqCounterGraphics;

// a frequency counter to measure the simulation haptic rate
cFrequencyCounter freqCounterHaptics;

// haptic thread
cThread* hapticsThread;

//STATES

enum MouseStates
{
	MOUSE_IDLE,
	MOUSE_MOVE_CAMERA
};

// mouse state
MouseStates mouseState = MOUSE_IDLE;

// last mouse position
double mouseX, mouseY;

// a handle to window display context
GLFWwindow* window = NULL;

// current width of window
int width = 0;

// current height of window
int height = 0;

//delta time variables

double currentFrame;
double lastFrame;
double deltaTime;

//movement variables

double moveSpeed = 5;
cVector3d movementVector;
int pressedKey;

//------------------------------------------------------------------------------
// OCULUS RIFT
//------------------------------------------------------------------------------

// display context
cOVRRenderContext renderContext;

// oculus device
cOVRDevice oculusVR;


//------------------------------------------------------------------------------
// DECLARED MACROS
//------------------------------------------------------------------------------

// convert to resource path
#define RESOURCE_PATH(p)    (char*)((resourceRoot+string(p)).c_str())


//------------------------------------------------------------------------------
// DECLARED FUNCTIONS
//------------------------------------------------------------------------------

// callback when an error GLFW occurs
void errorCallback(int error, const char* a_description);

// callback when a key is pressed
void keyCallback(GLFWwindow* a_window, int a_key, int a_scancode, int a_action, int a_mods);

// callback to handle mouse click
void mouseButtonCallback(GLFWwindow* a_window, int a_button, int a_action, int a_mods);

// callback to handle mouse motion
void mouseMotionCallback(GLFWwindow* a_window, double a_posX, double a_posY);

// function that closes the application
void close(void);

// main haptics simulation loop
void updateHaptics(void);

void moveCamera(void);

//==============================================================================
/*
	DEMO:   01-cube.cpp
	This example illustrates how to build a small mesh cube.
	The applications also presents the use of texture properties by defining
	a texture image and associated texture coordinates for each of the vertices.
	A bump map is also used in combination with a shader to create a more
	realistic rendering of the surface.
*/
//==============================================================================

int main(int argc, char** argv)
{
	//--------------------------------------------------------------------------
	// INITIALIZATION
	//--------------------------------------------------------------------------

	cout << endl;
	cout << "-----------------------------------" << endl;
	cout << "CHAI3D" << endl;
	cout << "Demo: 01-cube" << endl;
	cout << "Copyright 2003-2016" << endl;
	cout << "-----------------------------------" << endl << endl << endl;
	cout << "Keyboard Options:" << endl << endl;
	cout << "[ ] - Recenter view point" << endl;
	cout << "[q] - Exit application" << endl;
	cout << endl << endl;

	// parse first arg to try and locate resources
	string resourceRoot = string(argv[0]).substr(0, string(argv[0]).find_last_of("/\\") + 1);
	cStereoMode stereoMode = C_STEREO_DISABLED;

	// fullscreen mode
	bool fullscreen = false;

	// mirrored display
	bool mirroredDisplay = false;

	//--------------------------------------------------------------------------
	// SETUP DISPLAY CONTEXT
	//--------------------------------------------------------------------------

	// initialize GLFW library
	if (!glfwInit())
	{
		cout << "failed initialization" << endl;
		cSleepMs(1000);
		return 1;
	}

	// set error callback
	glfwSetErrorCallback(errorCallback);

	// set OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	// create display context
	width = 1200;
	height = 960;
	window = glfwCreateWindow(width, height, "MicroSim", NULL, NULL);
	if (!window)
	{
		cout << "failed to create window" << endl;
		cSleepMs(1000);
		glfwTerminate();
		return 1;
	}

	// set key callback
	glfwSetKeyCallback(window, keyCallback);

	// set mouse position callback
	glfwSetCursorPosCallback(window, mouseMotionCallback);

	// set mouse button callback
	glfwSetMouseButtonCallback(window, mouseButtonCallback);

	// set current display context
	glfwMakeContextCurrent(window);

	// sets the swap interval for the current display context
	glfwSwapInterval(0);

#ifdef GLEW_VERSION
	// initialize GLEW library
	if (glewInit() != GLEW_OK)
	{
		oculusVR.destroyVR();
		renderContext.destroy();
		glfwTerminate();
		return 1;
	}
#endif

	// create a new world.
	world = new cBulletWorld();

	// create a camera and insert it into the virtual world
	camera = new cCamera(world);
	world->addChild(camera);
	// initialize oculus
	if (!oculusVR.initVR())
	{
		cout << "failed to initialize Oculus" << endl;
		cSleepMs(1000);
		camSim = true;
		// a camera to render the world in the window display

		// position and orient the camera
		camera->set(cVector3d(2.5, 0.0, 0.3),    // camera position (eye)
			cVector3d(0.0, 0.0, -0.5),    // lookat position (target)
			cVector3d(0.0, 0.0, 1.0));   // direction of the "up" vector

										 // set the near and far clipping planes of the camera
		camera->setClippingPlanes(0.01, 10.0);

		// set stereo mode
		camera->setStereoMode(stereoMode);

		// set stereo eye separation and focal length (applies only if stereo is enabled)
		camera->setStereoEyeSeparation(0.02);
		camera->setStereoFocalLength(2.0);

		// set vertical mirrored display mode
		camera->setMirrorVertical(mirroredDisplay);
	}
	ovrSizei windowSize;
	// get oculus display resolution
	ovrSizei hmdResolution = oculusVR.getResolution();
	if (!camSim) {
		// setup mirror display on computer screen
		windowSize = { hmdResolution.w / 2, hmdResolution.h / 2 };
	}
	else {
		windowSize = { width,height };
	}
	// inialize buffers
	//if (!oculusVR.initVRBuffers(windowSize.w, windowSize.h) && !camSim)
	if (!oculusVR.initVRBuffers(windowSize.w, windowSize.h) && !camSim)
	{
		cout << "failed to initialize Oculus buffers" << endl;
		cSleepMs(1000);
		oculusVR.destroyVR();
		renderContext.destroy();
		glfwTerminate();
		return 1;
	}

	// set window size
	glfwSetWindowSize(window, windowSize.w, windowSize.h);


	//--------------------------------------------------------------------------
	// WORLD - CAMERA - LIGHTING
	//--------------------------------------------------------------------------

	world->setGravity(0, 0, -9.8);
	// set the background color of the environment
	// the color is defined by its (R,G,B) components.
	world->m_backgroundColor.setWhite();

	// position and orient the camera
	camera->set(cVector3d(0.5, 0.0, 0.2),    // camera position (eye)
		cVector3d(0.0, 0.0, 0.2),    // lookat position (target)
		cVector3d(0.0, 0.0, 1.0));   // direction of the "up" vector

// set the near and far clipping planes of the camera
// anything in front/behind these clipping planes will not be rendered
	camera->setClippingPlanes(0.01, 20.0);

	// create a light source
	light = new cSpotLight(world);

	// add light to world
	world->addChild(light);

	// enable light source
	light->setEnabled(true);

	// position the light source
	light->setLocalPos(3.5, 2.0, 0.0);

	// define the direction of the light beam
	light->setDir(-3.5, -2.0, 0.0);

	// set light cone half angle
	light->setCutOffAngleDeg(50);


	//--------------------------------------------------------------------------
	// HAPTIC DEVICES / TOOLS
	//--------------------------------------------------------------------------

	// create a haptic device handler
	handler = new cHapticDeviceHandler();
	// define the radius of the tool (sphere)
	double toolRadius = 0.02;
	numHapticDevices = handler->getNumDevices();
	if (numHapticDevices == 0) {
		cout << "No haptic device detected" << endl;
	}
	else {
		cout << "Number of haptic devices : " << numHapticDevices << endl;
	}
	for (int i = 0; i < numHapticDevices; i++) {

		// get access to the first available haptic device
		handler->getDevice(hapticDevice[i], i);


		// create a 3D tool and add it to the world
		tool[i] = new cToolCursor(world);
		world->addChild(tool[i]);

		// connect the haptic device to the tool
		tool[i]->setHapticDevice(hapticDevice[i]);


		// if the haptic device has a gripper, enable it as a user switch
		hapticDevice[i]->setEnableGripperUserSwitch(true);
		// define a radius for the tool
		tool[i]->setRadius(toolRadius);

		// enable if objects in the scene are going to rotate of translate
		// or possibly collide against the tool. If the environment
		// is entirely static, you can set this parameter to "false"
		tool[i]->enableDynamicObjects(true);

		// map the physical workspace of the haptic device to a larger virtual workspace.
		tool[i]->setWorkspaceRadius(0.3);

		// haptic forces are enabled only if small forces are first sent to the device;
		// this mode avoids the force spike that occurs when the application starts when 
		// the tool is located inside an object for instance. 
		tool[i]->setWaitForSmallForce(true);

		// start the haptic tool
		tool[i]->start();
	}



	// create a font
	cFontPtr font = NEW_CFONTCALIBRI20();

	// create a label to display the haptic and graphic rate of the simulation
	labelRates = new cLabel(font);
	labelRates->m_fontColor.setBlack();
	labelRates->setFontScale(10);
	camera->m_frontLayer->addChild(labelRates);

	// retrieve information about the current haptic device


	//--------------------------------------------------------------------------
	// CREATE OBJECT
	//--------------------------------------------------------------------------


	//
	// Box
	//

	double maxStiffness;

	// read the scale factor between the physical workspace of the haptic
	// device and the virtual workspace defined for the tool
	if (numHapticDevices > 0) {
		cHapticDeviceInfo hapticDeviceInfo = hapticDevice[0]->getSpecifications();
		workspaceScaleFactor = tool[0]->getWorkspaceScaleFactor();
		maxStiffness = hapticDeviceInfo.m_maxLinearStiffness / workspaceScaleFactor;
	}
	else {
		maxStiffness = 3;
	}


	bulletBox0 = new cBulletBox(world, 0.2, 0.2, 0.2);
	//world->addChild(bulletBox0);
	cMaterial mat0;
	mat0.setRedIndian();
	mat0.setStiffness(0.3 * maxStiffness);
	mat0.setDynamicFriction(0.6);
	mat0.setStaticFriction(0.6);
	bulletBox0->setMaterial(mat0);
	bulletBox0->setMass(0.05);
	bulletBox0->estimateInertia();
	bulletBox0->buildDynamicModel();
	bulletBox0->createAABBCollisionDetector(toolRadius);
	bulletBox0->setSurfaceFriction(0.4);
	bulletBox0->setLocalPos(0, 0, 2);
	bulletBox0->m_name = "box0";
	bulletBox0->m_userName = "catchable";

	bulletBox1 = new cBulletBox(world, 0.2, 0.2, 0.2);
	//world->addChild(bulletBox1);
	cMaterial mat1;
	mat0.setGreen();
	mat0.setStiffness(0.3 * maxStiffness);
	mat0.setDynamicFriction(0.6);
	mat0.setStaticFriction(0.6);
	bulletBox1->setMaterial(mat0);
	bulletBox1->setMass(0.05);
	bulletBox1->estimateInertia();
	bulletBox1->buildDynamicModel();
	bulletBox1->createAABBCollisionDetector(20);
	bulletBox1->setSurfaceFriction(0.4);
	bulletBox1->setLocalPos(0, 0.3, 2);
	bulletBox1->m_userName = "catchable";
	bulletBox1->m_name = "box1";

	//
	//Needle
	//
	bool fileload;
	double size;
	cMaterial mat;
	// create a texture
	cTexture2dPtr texture = cTexture2d::create();

	// stiffness properties

	// create a virtual mesh
	object = new cBulletMultiMesh(world);

	// add object to world
	world->addChild(object);

	// set the position of the object at the center of the world
	object->setLocalPos(0.0, 0.0, 0.0);
	fileload = object->loadFromFile(ROOT_DIR "Resources/Models/Needle/needle_model.obj");
	if (!fileload)
	{
		cout << resourceRoot << "\n";
#if defined(_MSVC)
		fileload = object->loadFromFile(ROOT_DIR "Resources/Models/Needle/needle_model.obj");
#endif
	}
	if (!fileload)
	{
		cout << "Error - 3D Model failed to load correctly." << endl;
		close();
		return (-1);
	}

	// compute a boundary box
	object->computeBoundaryBox(true);
	// get dimensions of object
	size = cSub(object->getBoundaryMax(), object->getBoundaryMin()).length();

	// resize object to screen
	if (size > 0)
	{
		if (numHapticDevices > 0) {
			object->scale(2.0 * tool[0]->getWorkspaceRadius() / size);
		}
	}

	// compute collision detection algorithm
	mat.setHapticTriangleSides(true, false);
	object->createAABBCollisionDetector(toolRadius);
	object->setMaterial(mat);



	// enable texture rendering 
	object->setUseTexture(true);

	// Since we don't need to see our polygons from both sides, we enable culling.
	object->setUseCulling(false, true);

	// set material properties to light gray
	object->m_material->setWhite();

	// compute collision detection algorithm

	// define a default stiffness for the object
	object->m_material->setStiffness(0.3 * maxStiffness);

	// define some static friction
	object->m_material->setStaticFriction(0.2);

	// define some dynamic friction
	object->m_material->setDynamicFriction(0.2);

	// define some texture rendering
	object->m_material->setTextureLevel(0.1);

	// render triangles haptically on front side only
	object->setMass(0.05);
	object->estimateInertia();
	object->buildContactTriangles(0.0001);
	object->buildDynamicModel();
	object->setSurfaceFriction(0.4);
	object->m_userName = "catchable";
	cColorf color = cColorf(1.0, 0.0, 0.0);
	object->setCollisionDetectorProperties(3, color, true);
	object->setShowCollisionDetector(true, true);
	//-----------------------------------------------------------------
	//Aiguille
	//-----------------------------------------------------------------
	target = new cMultiMesh();
	// add target to world
	//world->addChild(target);

	// set the position of the target at the center of the world
	target->setLocalPos(.2, 0.2, 0.0);
	fileload = target->loadFromFile(ROOT_DIR "Resources/Models/Needle/needle_hole_model.obj");
	if (!fileload)
	{
		cout << resourceRoot;
#if defined(_MSVC)
		fileload = target->loadFromFile(ROOT_DIR "Resources/Models/Needle/needle_hole_model.obj");
#endif
	}
	if (!fileload)
	{
		cout << "Error - 3D Model failed to load correctly." << endl;
		close();
		return (-1);
	}

	// compute a boundary box
	target->computeBoundaryBox(true);
	// get dimensions of target
	size = cSub(target->getBoundaryMax(), target->getBoundaryMin()).length();

	// resize target to screen
	if (size > 0)
	{
		if (numHapticDevices > 0) {
			target->scale(2.0 * tool[0]->getWorkspaceRadius() / size);
		}
		else {
			target->scale(2.0 / size);
		}
	}

	// compute collision detection algorithm
	mat.setHapticTriangleSides(true, true);
	target->setMaterial(mat);


	// enable texture rendering 
	target->setUseTexture(true);

	// Since we don't need to see our polygons from both sides, we enable culling.
	target->setUseCulling(false, true);

	// set material properties to light gray
	target->m_material->setWhite();

	// compute collision detection algorithm
	target->createAABBCollisionDetector(toolRadius);

	// define a default stiffness for the target
	target->m_material->setStiffness(0.3 * maxStiffness);

	// define some static friction
	target->m_material->setStaticFriction(0.2);

	// define some dynamic friction
	target->m_material->setDynamicFriction(0.2);

	// define some texture rendering
	target->m_material->setTextureLevel(0.1);

	// render triangles haptically on front side only
	target->m_material->setHapticTriangleSides(true, false);
	target->m_userName = "target";
	//-----------------------------------------------------------------
	//Class
	//-----------------------------------------------------------------
//	classroom = new cMultiMesh();
//	// add target to world
//	world->addChild(classroom);
//
//	// set the position of the target at the center of the world
//	classroom->setLocalPos(0.0, 0.0, -1.4);
//	fileload = classroom->loadFromFile(RESOURCE_PATH("../resources/models/classroom/sala.obj"));
//	if (!fileload)
//	{
//		cout << resourceRoot;
//#if defined(_MSVC)
//		fileload = classroom->loadFromFile("../../../bin/resources/models/classroom/sala.obj");
//#endif
//	}
//	if (!fileload)
//	{
//		cout << "Error - 3D Model failed to load correctly." << endl;
//		close();
//		return (-1);
//	}
//
//	// compute a boundary box
//	//classroom->computeBoundaryBox(true);
//	// get dimensions of target
//	size = cSub(classroom->getBoundaryMax(), classroom->getBoundaryMin()).length();
//
//	// resize target to screen
//	if (size > 0)
//	{
//		classroom->scale(10.0 / size);
//	}
//
//
//	classroom->rotateAboutGlobalAxisDeg(cVector3d(1, 0, 0), 90);
//	// render triangles haptically on front side only
//	classroom->m_material->setHapticTriangleSides(false, false);
//	cMaterial matClass;
//	matClass.setWhite();
//	//classroom->setMaterial(matClass);

	//////////////////////////////////////////////////////////////////////////
	// GROUND
	//////////////////////////////////////////////////////////////////////////

	// create ground plane
	//bulletGround = new cBulletStaticPlane(world, cVector3d(0.0, 0.0, 1.0), -planeWidth / 3);
	//bulletGround->m_userName = "plan";

	//bulletInvisibleWall1 = new cBulletStaticPlane(world, cVector3d(0.0, 0.0, -1.0), -2.0 * planeWidth / 1.4);
	//bulletInvisibleWall2 = new cBulletStaticPlane(world, cVector3d(0.0, -1.0, 0.0), -planeWidth / 1.4);
	//bulletInvisibleWall3 = new cBulletStaticPlane(world, cVector3d(0.0, 1.0, 0.0), -planeWidth / 1.4);
	//bulletInvisibleWall4 = new cBulletStaticPlane(world, cVector3d(-1.0, 0.0, 0.0), -planeWidth / 1.4);
	//bulletInvisibleWall5 = new cBulletStaticPlane(world, cVector3d(1.0, 0.0, 0.0), -0.8 * planeWidth / 1.4);
	//bulletInvisibleWall1->m_userName = "plan";
	//bulletInvisibleWall2->m_userName = "plan";
	//bulletInvisibleWall3->m_userName = "plan";
	//bulletInvisibleWall4->m_userName = "plan";
	//bulletInvisibleWall5->m_userName = "plan";
	//// add plane to world as we will want to make it visibe
	//world->addChild(bulletGround);

	//// create a mesh plane where the static plane is located
	//cCreatePlane(bulletGround, 3.0, 3.0, bulletGround->getPlaneConstant() * bulletGround->getPlaneNormal());

	//// define some material properties and apply to mesh
	//cMaterial matGround;
	//matGround.setStiffness(0.3 * maxStiffness);
	//matGround.setDynamicFriction(0.2);
	//matGround.setStaticFriction(0.0);
	//matGround.setWhite();
	//matGround.m_emission.setGrayLevel(0.3);
	//bulletGround->setMaterial(matGround);

	//// setup collision detector for haptic interaction
	//bulletGround->createAABBCollisionDetector(toolRadius);

	//// set friction values
	//bulletGround->setSurfaceFriction(0.4);

	//--------------------------------------------------------------------------
	// CREATE SHADERS
	//--------------------------------------------------------------------------

	// create vertex shader
	cShaderPtr vertexShader = cShader::create(C_VERTEX_SHADER);

	// load vertex shader from file
	fileload = vertexShader->loadSourceFile(ROOT_DIR "Resources/Shaders/bump.vert");
	if (!fileload)
	{
#if defined(_MSVC)
		fileload = vertexShader->loadSourceFile(ROOT_DIR "Resources/Shaders/bump.vert");
#endif
	}

	// create fragment shader
	cShaderPtr fragmentShader = cShader::create(C_FRAGMENT_SHADER);

	// load fragment shader from file
	fileload = fragmentShader->loadSourceFile(ROOT_DIR "Resources/Shaders/bump.frag");
	if (!fileload)
	{
#if defined(_MSVC)
		fileload = fragmentShader->loadSourceFile(ROOT_DIR "Resources/Shaders/bump.frag");
#endif
	}

	// create program shader
	cShaderProgramPtr programShader = cShaderProgram::create();

	// assign vertex shader to program shader
	programShader->attachShader(vertexShader);

	// assign fragment shader to program shader
	programShader->attachShader(fragmentShader);

	// link program shader
	programShader->linkProgram();

	// set uniforms
	programShader->setUniformi("uColorMap", 0);
	programShader->setUniformi("uShadowMap", 0);
	programShader->setUniformi("uNormalMap", 2);
	programShader->setUniformf("uInvRadius", 0.0f);


	//--------------------------------------------------------------------------
	// CREATE ENVIRONMENT GLOBE
	//--------------------------------------------------------------------------

	// create a virtual mesh
	cMesh* globe = new cMesh();

	// add object to world
	world->addChild(globe);

	// set the position of the object at the center of the world
	globe->setLocalPos(0.0, 0.0, 0.0);

	// create cube
	cCreateSphere(globe, 6.0, 360, 360);
	globe->setUseDisplayList(true);
	globe->deleteCollisionDetector();

	// create a texture
	cTexture2dPtr textureSpace = cTexture2d::create();

	fileload = textureSpace->loadFromFile(ROOT_DIR "Resources/Images/sky.jpg");
	if (!fileload)
	{
#if defined(_MSVC)
		fileload = textureSpace->loadFromFile(ROOT_DIR "Resources/Images/sky.jpg");
#endif
	}
	if (!fileload)
	{
		cout << "Error - Texture image failed to load correctly." << endl;
		close();
		return (-1);
	}

	// apply texture to object
	globe->setTexture(textureSpace);

	// enable texture rendering 
	globe->setUseTexture(true);

	// Since we don't need to see our polygons from both sides, we enable culling.
	globe->setUseCulling(false);

	// disable material properties and lighting
	globe->setUseMaterial(false);


	//--------------------------------------------------------------------------
	// START SIMULATION
	//--------------------------------------------------------------------------

	// create a thread which starts the main haptics rendering loop
	hapticsThread = new cThread();
	hapticsThread->start(updateHaptics, CTHREAD_PRIORITY_HAPTICS);

	// setup callback when application exits
	atexit(close);


	//--------------------------------------------------------------------------
	// MAIN GRAPHIC LOOP
	//--------------------------------------------------------------------------

	// recenter oculus
	oculusVR.recenterPose();



	test = new cBulletBox(world, 0.2, 0.2, 0.2);
	world->addChild(test);
	cMaterial mattest;
	mattest.setBlack();
	mattest.setStiffness(0.3 * maxStiffness);
	mattest.setDynamicFriction(0.6);
	mattest.setStaticFriction(0.6);
	test->setMaterial(mattest);
	test->setMass(0.05);
	test->estimateInertia();
	test->buildDynamicModel();
	test->setSurfaceFriction(0.4);
	test->setLocalPos(tool[0]->getLocalPos());
	test->deleteCollisionDetector(true);
	test->m_bulletRigidBody->setGravity(btVector3(0.0, 0.0, 0.0));


	// main graphic rendering loop
	while (!glfwWindowShouldClose(window) && !simulationFinished)
	{
		cout.precision(2);
		cout << test->getLocalPos().x() << ";" << test->getLocalPos().y() << ";" << test->getLocalPos().z() << endl;

		/////////////////////////////////////////////////////////////////////
		// UPDATE WIDGETS
		/////////////////////////////////////////////////////////////////////

		// update haptic and graphic rate data
		globe->rotateAboutGlobalAxisDeg(cVector3d(0, 0, 1), 0.003);
		labelRates->setText(cStr(freqCounterGraphics.getFrequency(), 0) + " Hz / " +
			cStr(freqCounterHaptics.getFrequency(), 0) + " Hz");

		// update position of label
		labelRates->setLocalPos((int)(0.5 * (width - labelRates->getWidth())), 900);

		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		//deltaTimeCalc

		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		if (!camSim) {
			// start rendering
			oculusVR.onRenderStart();

			// render frame for each eye
			for (int eyeIndex = 0; eyeIndex < ovrEye_Count; eyeIndex++)
			{
				// retrieve projection and modelview matrix from oculus
				cTransform projectionMatrix, modelViewMatrix;
				oculusVR.onEyeRender(eyeIndex, projectionMatrix, modelViewMatrix);

				camera->m_useCustomProjectionMatrix = true;
				camera->m_projectionMatrix = projectionMatrix;

				camera->m_useCustomModelViewMatrix = true;
				camera->m_modelViewMatrix = modelViewMatrix;

				// render world
				ovrSizei size = oculusVR.getEyeTextureSize(eyeIndex);
				camera->renderView(size.w, size.h, C_STEREO_LEFT_EYE, false);

				// finalize rendering  
				oculusVR.onEyeRenderFinish(eyeIndex);
			}

			// update frames
			oculusVR.submitFrame();
			oculusVR.blitMirror();
		}
		else {
			moveCamera();
			camera->renderView(width, height);

		}

		glfwSwapBuffers(window);
		glfwPollEvents();
		freqCounterGraphics.signal(1);
	}

	// cleanup
	if (!camSim) {
		oculusVR.destroyVR();
		renderContext.destroy();
	}

	glfwDestroyWindow(window);

	// exit glfw
	glfwTerminate();

	return (0);
}

//------------------------------------------------------------------------------

void errorCallback(int a_error, const char* a_description)
{
	cout << "Error: " << a_description << endl;
}

//------------------------------------------------------------------------------

void keyCallback(GLFWwindow* a_window, int a_key, int a_scancode, int a_action, int a_mods)
{
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

	// option - spacebar
	else if (a_key == GLFW_KEY_SPACE)
	{
		oculusVR.recenterPose();
	}
	else if (a_key == GLFW_KEY_R)
	{
		bulletBox0->setLocalPos(0, 0, 2);
		bulletBox1->setLocalPos(0, 0.3, 2);
		bulletBox0->m_bulletRigidBody->clearForces();
		bulletBox1->m_bulletRigidBody->clearForces();
	}

	pressedKey = a_key;


}

//------------------------------------------------------------------------------

void mouseButtonCallback(GLFWwindow* a_window, int a_button, int a_action, int a_mods)
{
	if (a_button == GLFW_MOUSE_BUTTON_RIGHT && a_action == GLFW_PRESS)
	{
		// store mouse position
		glfwGetCursorPos(window, &mouseX, &mouseY);

		// update mouse state
		mouseState = MOUSE_MOVE_CAMERA;
	}

	else
	{
		// update mouse state
		mouseState = MOUSE_IDLE;
	}
}

//------------------------------------------------------------------------------

void mouseMotionCallback(GLFWwindow* a_window, double a_posX, double a_posY)
{
	if (mouseState == MOUSE_MOVE_CAMERA)
	{
		// compute mouse motion
		int dx = a_posX - mouseX;
		int dy = a_posY - mouseY;
		mouseX = a_posX;
		mouseY = a_posY;
		cVector3d cameraPos = camera->getLocalPos();


		// compute new camera angles
		double azimuthDeg = camera->getSphericalAzimuthDeg() - 0.5 * dx;
		double polarDeg = camera->getSphericalPolarDeg() - 0.5 * dy;

		// assign new angles
		camera->setSphericalAzimuthDeg(azimuthDeg);
		camera->setSphericalPolarDeg(polarDeg);

		camera->setLocalPos(cameraPos);

	}
}

//------------------------------------------------------------------------------

void close(void)
{
	// stop the simulation
	simulationRunning = false;

	// wait for graphics and haptics loops to terminate
	while (!simulationFinished) { cSleepMs(100); }

	// close haptic device
	for (int i = 0; i < numHapticDevices; i++)
	{
		tool[i]->stop();
	}

	// delete resources
	delete hapticsThread;
	delete world;
	delete handler;
}

//------------------------------------------------------------------------------
enum cMode
{
	IDLE,
	SELECTION
};
void updateHaptics(void)
{
	// angular velocity of object
	cVector3d angVel(0.0, 0.2, 0.3);

	// reset clock 
	cMode state[MAX_DEVICES];
	cGenericObject* selectedObject[MAX_DEVICES];
	cTransform world_T_object[MAX_DEVICES];
	cMaterialPtr mat[MAX_DEVICES];
	for (int i = 0; i < numHapticDevices; i++)
	{
		state[i] = IDLE;
		selectedObject[i] = NULL;
	}
	cTransform tool_T_object[MAX_DEVICES];
	cTransform tool_T_world[MAX_DEVICES];
	cTransform world_T_tool[MAX_DEVICES];
	cTransform parent_T_world[MAX_DEVICES];
	cTransform parent_T_object[MAX_DEVICES];
	cPrecisionClock clock;
	clock.start(true);

	// simulation in now running
	simulationRunning = true;
	simulationFinished = false;

	// main haptic simulation loop
	while (simulationRunning)
	{
		/////////////////////////////////////////////////////////////////////
		// SIMULATION TIME
		/////////////////////////////////////////////////////////////////////

		// stop the simulation clock

		// read the time increment in seconds
		double timeInterval = clock.stop();

		// restart the simulation clock
		clock.start(true);

		// update frequency counter
		freqCounterHaptics.signal(1);
		// compute global reference frames for each object
		world->computeGlobalPositions(true);

		double nextSimInterval = 0.0005;//cClamp(time, 0.00001, 0.0002);
		for (int i = 0; i < numHapticDevices; i++)
		{


			/////////////////////////////////////////////////////////////////////
			// HAPTIC FORCE COMPUTATION
			/////////////////////////////////////////////////////////////////////


			// update position and orientation of tool
			tool[i]->updateFromDevice();

			// compute interaction forces
			tool[i]->computeInteractionForces();
			world_T_tool[i] = tool[i]->getDeviceGlobalTransform();

			// get status of user switch
			bool button = tool[i]->getUserSwitch(0);

			//
			// STATE 1:
			// Idle mode - user presses the user switch
			//
			if ((state[i] == IDLE) && (button == true))
			{
				// check if at least one contact has occurred
				if (tool[i]->m_hapticPoint->getNumCollisionEvents() > 0)
				{
					// get contact event
					cCollisionEvent* collisionEvent = tool[i]->m_hapticPoint->getCollisionEvent(0);

					// get object from contact event
					selectedObject[i] = collisionEvent->m_object;
					//if UI Element...
					std::cout << "parent : " << selectedObject[i]->getOwner()->m_userName << endl;
					std::cout << "m_userName : " << selectedObject[i]->m_userName << endl;
					std::cout << "m_name : " << selectedObject[i]->m_name << endl;
					if (selectedObject[i] == target) {
						std::cout << "target";
						selectedObject[i] = NULL;
					}
					//endif
					for (int j = 0; j < numHapticDevices; j++)
					{
						if (selectedObject[i] == selectedObject[j] && i != j) {
							selectedObject[j] = NULL;
							state[j] = IDLE;
						}
					}
#
				}
				else
				{
					selectedObject[i] = NULL;
				}

				if (selectedObject[i] != NULL && (selectedObject[i]->m_userName == "catchable" || selectedObject[i]->getOwner()->m_userName == "catchable")) {
					// get transformation from object
					if (selectedObject[i]->m_userName != "catchable" && selectedObject[i]->getOwner()->m_userName == "catchable") {
						selectedObject[i] = selectedObject[i]->getOwner();
					}
					world_T_object[i] = selectedObject[i]->getGlobalTransform();
					mat[i] = selectedObject[i]->m_material->copy();
					// compute inverse transformation from contact point to object 
					tool_T_world[i] = world_T_tool[i];
					tool_T_world[i].invert();

					// store current transformation tool
					tool_T_object[i] = tool_T_world[i] * world_T_object[i];

					// update state
					state[i] = SELECTION;

					//remove from bullet simulation
					cBulletGenericObject* bulletobject = dynamic_cast<cBulletGenericObject*>(selectedObject[i]);
					if (bulletobject != NULL) {
						bulletobject->setStatic(true);
						//bulletobject->m_material->setBlack();
						bulletobject->m_bulletRigidBody->setGravity(btVector3(0, 0, 0));
					}
				}
				else {
					selectedObject[i] = NULL;
				}
			}


			//
			// STATE 2:
			// Selection mode - operator maintains user switch enabled and moves object
			//
			else if ((state[i] == SELECTION) && (button == true))
			{
				// compute new transformation of object in global coordinates
				world_T_object[i] = world_T_tool[i] * tool_T_object[i];

				// compute new transformation of object in local coordinates
				parent_T_world[i] = selectedObject[i]->getParent()->getLocalTransform();
				parent_T_world[i].invert();
				parent_T_object[i] = parent_T_world[i] * world_T_object[i];

				// assign new local transformation to object
				selectedObject[i]->setLocalTransform(parent_T_object[i]);

				// set zero forces when manipulating objects
				tool[i]->setDeviceGlobalForce(0.0, 0.0, 0.0);

				tool[i]->initialize();
			}

			//
			// STATE 3:
			// Finalize Selection mode - operator releases user switch.
			//
			else
			{
				state[i] = IDLE;
				if (selectedObject[i] != NULL) {
					selectedObject[i]->m_material = mat[i];
					cBulletGenericObject* bulletobject = dynamic_cast<cBulletGenericObject*>(selectedObject[i]);
					if (bulletobject != NULL) {
						bulletobject->setStatic(false);
						//bulletobject->m_material->setWhite();
						world->removeChild(selectedObject[i]);
						btVector3 zeroVector(0, 0, 0);
						bulletobject->m_bulletRigidBody->setLinearVelocity(zeroVector);
						bulletobject->m_bulletRigidBody->setAngularVelocity(zeroVector);
						bulletobject->m_bulletRigidBody->clearForces();
						bulletobject->m_bulletRigidBody->setGravity(btVector3(0, 0, -9.8));
						world->addChild(selectedObject[i]);
					}
					selectedObject[i] = NULL;
				}
			}
			// send forces to haptic device
			tool[i]->applyToDevice();


			/////////////////////////////////////////////////////////////////////
			// DYNAMIC SIMULATION
			/////////////////////////////////////////////////////////////////////
			int numInteractionPoints = tool[i]->getNumHapticPoints();

			if ((state[i] != SELECTION) || (button == false)) {

				for (int j = 0; j < numInteractionPoints; j++)
				{
					// get pointer to next interaction point of tool
					cHapticPoint* interactionPoint = tool[i]->getHapticPoint(j);

					// check all contact points
					int numContacts = interactionPoint->getNumCollisionEvents();
					for (int k = 0; k < numContacts; k++)
					{
						cCollisionEvent* collisionEvent = interactionPoint->getCollisionEvent(k);

						// given the mesh object we may be touching, we search for its owner which
						// could be the mesh itself or a multi-mesh object. Once the owner found, we
						// look for the parent that will point to the Bullet object itself.
						cGenericObject* object = collisionEvent->m_object->getOwner()->getOwner();

						// cast to Bullet object
						cBulletGenericObject* bulletobject = dynamic_cast<cBulletGenericObject*>(object);

						// if Bullet object, we apply interaction forces
						if (bulletobject != NULL)
						{
							bulletobject->addExternalForceAtPoint(-interactionPoint->getLastComputedForce(),
								collisionEvent->m_globalPos - object->getLocalPos());
						}
					}
				}
			}
		}
		world->updateDynamics(timeInterval);
	}

	// exit haptics thread
	simulationFinished = true;
}

void moveCamera() {

	cVector3d cameraPos = camera->getLocalPos();

	if (pressedKey == GLFW_KEY_W) {
		movementVector = camera->getLookVector();
	}
	else if (pressedKey == GLFW_KEY_D) {
		movementVector = camera->getRightVector();
	}
	else if (pressedKey == GLFW_KEY_A) {
		movementVector = camera->getRightVector();
		movementVector.negate();
	}
	else if (pressedKey == GLFW_KEY_S) {

		movementVector = camera->getLookVector();
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
	camera->setLocalPos(cameraPos);

}

//------------------------------------------------------------------------------
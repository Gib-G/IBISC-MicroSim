#include <chai3d.h>
//---------------------------------------------------------------------------
#include <GLFW/glfw3.h>
//---------------------------------------------------------------------------
using namespace chai3d;
using namespace std; // Pas bien ça... :(
//---------------------------------------------------------------------------
#include <CODE.h>
#include <stdlib.h>
//------------------------------------------------------------------------------
#include <COculus.h>
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// GENERAL SETTINGS
//---------------------------------------------------------------------------

// stereo Mode
/*
	C_STEREO_DISABLED:            Stereo is disabled
	C_STEREO_ACTIVE:              Active stereo for OpenGL NVDIA QUADRO cards
	C_STEREO_PASSIVE_LEFT_RIGHT:  Passive stereo where L/R images are rendered next to each other
	C_STEREO_PASSIVE_TOP_BOTTOM:  Passive stereo where L/R images are rendered above each other
*/
cStereoMode stereoMode = C_STEREO_DISABLED;

// fullscreen mode
bool fullscreen = false;

// mirrored display
bool mirroredDisplay = false;

// maximum number of devices supported by this application
const int MAX_DEVICES = 2;

//---------------------------------------------------------------------------
// CHAI3D VARIABLES
//---------------------------------------------------------------------------

// a world that contains all objects of the virtual environment
cWorld* world;

// a camera to render the world in the window display
cCamera* camera;

// a light source to illuminate the objects in the world
cSpotLight* light;



// number of haptic devices detected
int numHapticDevices = 0;

// a pointer to the current haptic device
cGenericHapticDevicePtr hapticDevice[MAX_DEVICES];

// a haptic device handler
cHapticDeviceHandler* handler;

// a virtual tool representing the haptic device in the scene
cToolGripper* tool[MAX_DEVICES];

// a label to display the rate [Hz] at which the simulation is running
cLabel* labelRates;

bool previousframecaught[MAX_DEVICES];
int gripperCatchingIndex = -1;
cMatrix3d startrotGripper[MAX_DEVICES];
cVector3d startposGripper[MAX_DEVICES];
cVector3d startposCube;
cMatrix3d startrotCube;

//---------------------------------------------------------------------------
// ODE MODULE VARIABLES
//---------------------------------------------------------------------------

// ODE world
cODEWorld* ODEWorld;

// ODE objects
cODEGenericBody* ODEBody0;
cODEGenericBody* ODENeedleRotationCenter;
cODEGenericBody* ODEBodytest;
cODEGenericBody* ODEBody1[12];
cODEGenericBody* ODEBody2[MAX_DEVICES];
cODEGenericBody* ODEBody3[MAX_DEVICES];;
cMesh* object0;
cMesh* objecttest;
cMesh* object1[12];
cMesh* object2[MAX_DEVICES];
cMesh* object3[MAX_DEVICES];
cMesh* ground;
// ODE objects
cODEGenericBody* ODEGPlane0;
cODEGenericBody* ODEGPlane1;
cODEGenericBody* ODEGPlane2;
cODEGenericBody* ODEGPlane3;
cODEGenericBody* ODEGPlane4;
cODEGenericBody* ODEGPlane5;

cCollisionRecorder recorder;
cCollisionSettings settings;
cMesh* DetectSphere[5];
cMesh* DetectionPlanes[12];
bool DetectionPlanesFinished[12];
cMesh* testPlane;
bool crossing[12];
bool end1[12];
bool end2[12];
float sphereSize = 0.01;

//---------------------------------------------------------------------------
// GENERAL VARIABLES
//---------------------------------------------------------------------------

double currentFrame;
double lastFrame;
double deltaTime;
// flag to indicate if the haptic simulation currently running
bool simulationRunning = false;

// flag to indicate if the haptic simulation has terminated
bool simulationFinished = true;

// a frequency counter to measure the simulation graphic rate
cFrequencyCounter freqCounterGraphics;

// a frequency counter to measure the simulation haptic rate
cFrequencyCounter freqCounterHaptics;

// haptic thread
cThread* hapticsThread;

// a handle to window display context
GLFWwindow* window = NULL;

// current width of window
int width = 0;

// current height of window
int height = 0;

// swap interval for the display context (vertical synchronization)
int swapInterval = 1;

double crossTimer = 0;

cMesh* resetButton;
//delta time variables
double timeInterval;
bool start = false;
float lastSave;
float timerNum = 0.0;
bool startgrab = false;
bool pressed[MAX_DEVICES];
//Sauvegarde
std::stringstream streamstr;
std::tuple<float, cVector3d> posData[MAX_DEVICES];
std::string NumCandidate;
std::ofstream myfile[MAX_DEVICES];
std::ofstream tempfile[MAX_DEVICES];
std::string pathname;
//------------------------------------------------------------------------------
// OCULUS RIFT
//------------------------------------------------------------------------------

// display context
cOVRRenderContext renderContext;

// oculus device
cOVRDevice oculusVR;

cVector3d defaultPos = cVector3d(1.6, 0, -5);

bool camSim;
//------------------------------------------------------------------------------
// DECLARED FUNCTIONS
//------------------------------------------------------------------------------

// callback when the window display is resized
void windowSizeCallback(GLFWwindow* a_window, int a_width, int a_height);

// callback when an error GLFW occurs
void errorCallback(int error, const char* a_description);

// callback when a key is pressed
void keyCallback(GLFWwindow* a_window, int a_key, int a_scancode, int a_action, int a_mods);

// this function renders the scene
void updateGraphics(void);

// this function contains the main haptics simulation loop
void updateHaptics(void);
void InitializeNeedleDetect(void);
void AddDetectionPlane(int i, cODEGenericBody* ring);
void ComputeCrossing(cMesh* spheres[], int i);
// this function closes the application
void close(void);
void ResetSim(void);
void SaveData(void);
void Start(void);
void SaveResults(void);
cVector3d toAxisAngleVec(cMatrix3d m);
double toAxisAngleAngle(cMatrix3d m);

//===========================================================================
/*
	DEMO:    01-ODE-cubic.cpp

	This example illustrates the use of the ODE framework for simulating
	haptic interaction with dynamic bodies. In this scene we create 3
	cubic meshes that we individually attach to ODE bodies. Haptic interactions
	are computer by using the finger-proxy haptic model and forces are
	propagated to the ODE representation.
 */
 //===========================================================================

int main(int argc, char* argv[])
{
	//-----------------------------------------------------------------------
	// INITIALIZATION
	//-----------------------------------------------------------------------

	cout << endl;
	cout << "-----------------------------------" << endl;
	cout << "CHAI3D" << endl;
	cout << "Demo: 01-ODE-cubic" << endl;
	cout << "Copyright 2003-2016" << endl;
	cout << "-----------------------------------" << endl << endl << endl;
	cout << "Keyboard Options:" << endl << endl;
	cout << "[g] - Enable/Disable gravity" << endl;
	cout << "[f] - Enable/Disable full screen mode" << endl;
	cout << "[m] - Enable/Disable vertical mirroring" << endl;
	cout << "[q] - Exit application" << endl;
	cout << endl << endl;


	//--------------------------------------------------------------------------
	// OPEN GL - WINDOW DISPLAY
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

	// compute desired size of window
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	int w = 0.8 * mode->height;
	int h = 0.5 * mode->height;
	int x = 0.5 * (mode->width - w);
	int y = 0.5 * (mode->height - h);

	// set OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

	// set active stereo mode
	if (stereoMode == C_STEREO_ACTIVE)
	{
		glfwWindowHint(GLFW_STEREO, GL_TRUE);
	}
	else
	{
		glfwWindowHint(GLFW_STEREO, GL_FALSE);
	}

	// create display context
	window = glfwCreateWindow(w, h, "CHAI3D", NULL, NULL);
	if (!window)
	{
		cout << "failed to create window" << endl;
		cSleepMs(1000);
		glfwTerminate();
		return 1;
	}

	// get width and height of window
	glfwGetWindowSize(window, &width, &height);

	// set position of window
	glfwSetWindowPos(window, x, y);

	// set key callback
	glfwSetKeyCallback(window, keyCallback);

	// set resize callback
	glfwSetWindowSizeCallback(window, windowSizeCallback);

	// set current display context
	glfwMakeContextCurrent(window);

	// sets the swap interval for the current display context
	glfwSwapInterval(swapInterval);

	// initialize GLEW library
#ifdef GLEW_VERSION
	if (glewInit() != GLEW_OK)
	{
		cout << "failed to initialize GLEW library" << endl;
		glfwTerminate();
		return 1;
	}
#endif


	//-----------------------------------------------------------------------
	// WORLD - CAMERA - LIGHTING
	//-----------------------------------------------------------------------

	// create a new world.
	world = new cWorld();
	// set the background color of the environment
	world->m_backgroundColor.setWhite();

	// create a camera and insert it into the virtual world
	camera = new cCamera(world);
	world->addChild(camera);
	if (!oculusVR.initVR())
	{
		cout << "failed to initialize Oculus" << endl;
		cSleepMs(1000);
		camSim = true;

		// position and orient the camera
		camera->set(cVector3d(2.5, 0.0, 0.3),    // camera position (eye)
			cVector3d(0.0, 0.0, -0.5),    // lookat position (target)
			cVector3d(0.0, 0.0, 1.0));   // direction of the "up" vector

										 // set the near and far clipping planes of the camera
		camera->setClippingPlanes(0.01, 10.0);

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

	if (!camSim) {

		// position and orient the camera
		camera->set(cVector3d(0.5, 0.0, 0.2),    // camera position (eye)
			cVector3d(0.0, 0.0, 0.2),    // lookat position (target)
			cVector3d(0.0, 0.0, 1.0));   // direction of the "up" vector

	// set the near and far clipping planes of the camera
	// anything in front/behind these clipping planes will not be rendered
		camera->setClippingPlanes(0.01, 20.0);


		// set stereo mode
		camera->setStereoMode(stereoMode);

		// set stereo eye separation and focal length (applies only if stereo is enabled)
		camera->setStereoEyeSeparation(0.02);
		camera->setStereoFocalLength(2.0);
	}

	// create a light source
	light = new cSpotLight(world);

	// attach light to camera
	world->addChild(light);

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


	//-----------------------------------------------------------------------
	// HAPTIC DEVICES / TOOLS
	//-----------------------------------------------------------------------
	// create a haptic device handler
	handler = new cHapticDeviceHandler();
	cHapticDeviceInfo hapticDeviceInfo;
	double workspaceScaleFactor = 1;
	numHapticDevices = handler->getNumDevices();
	double toolRadius = 0.01;
	for (int i = 0; i < numHapticDevices; i++) {
		previousframecaught[i] = false;

		// get access to the first available haptic device
		handler->getDevice(hapticDevice[i], i);


		// create a 3D tool and add it to the world
		tool[i] = new cToolGripper(world);
		world->addChild(tool[i]);

		// connect the haptic device to the tool
		tool[i]->setHapticDevice(hapticDevice[i]);


		// if the haptic device has a gripper, enable it as a user switch
		hapticDevice[i]->setEnableGripperUserSwitch(true);
		// define a radius for the tool
		tool[i]->setRadius(toolRadius);
		tool[i]->setGripperWorkspaceScale(.06);
		//tool[i]->setGripperWorkspaceScale(.03);
		tool[i]->setShowContactPoints(true, false);
		// enable if objects in the scene are going to rotate of translate
		// or possibly collide against the tool. If the environment
		// is entirely static, you can set this parameter to "false"
		tool[i]->enableDynamicObjects(true);

		// map the physical workspace of the haptic device to a larger virtual workspace.
		tool[i]->setWorkspaceRadius(6);
		//tool[i]->setWorkspaceRadius(12);

		// haptic forces are enabled only if small forces are first sent to the device;
		// this mode avoids the force spike that occurs when the application starts when 
		// the tool is located inside an object for instance. 
		tool[i]->setWaitForSmallForce(true);

		// start the haptic tool
		tool[i]->start();
		tool[i]->translate(0, (1 - 2 * i) * 0, -5);
		//tool[i]->translate(0, (1 - 2 * i) * 12.5, 7);
		streamstr << ROOT_DIR "Resources/CSV/Temp/temp_" << "trajectory-Arm_";
		pathname = streamstr.str();
		streamstr << i << ".csv";
		tempfile[i].open(streamstr.str());
		streamstr.str("");
		streamstr.clear();
	}


	if (numHapticDevices == 0) {
		cout << "No haptic device detected" << endl;
	}
	else {

		// read the scale factor between the physical workspace of the haptic
		// device and the virtual workspace defined for the tool
		workspaceScaleFactor = tool[0]->getWorkspaceScaleFactor();
		// retrieve information about the current haptic device
		hapticDeviceInfo = hapticDevice[0]->getSpecifications();
		cout << "Number of haptic devices : " << numHapticDevices << endl;
	}

	//--------------------------------------------------------------------------
	// WIDGETS
	//--------------------------------------------------------------------------

	// create a font
	cFontPtr font = NEW_CFONTCALIBRI20();

	// create a label to display the haptic and graphic rate of the simulation
	labelRates = new cLabel(font);
	labelRates->m_fontColor.setBlack();
	camera->m_frontLayer->addChild(labelRates);

	//-----------------------------------------------------------------------
	// CREATE ODE WORLD AND OBJECTS
	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// ODE WORLD
	//////////////////////////////////////////////////////////////////////////


	// stiffness properties
	double maxStiffness = hapticDeviceInfo.m_maxLinearStiffness / workspaceScaleFactor;

	// create an ODE world to simulate dynamic bodies
	ODEWorld = new cODEWorld(world);

	// add ODE world as a node inside world
	world->addChild(ODEWorld);

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
	mat0.setStiffness(maxStiffness);
	mat0.setDynamicFriction(3);
	mat0.setStaticFriction(3);
	object0->setMaterial(mat0);

	mat1.setBlueRoyal();
	mat1.setStiffness(0.3 * maxStiffness);
	mat1.setDynamicFriction(3);
	mat1.setStaticFriction(3);

	mat2.setGrayDim();
	mat2.setStiffness(0.3 * maxStiffness);
	mat2.setDynamicFriction(3);
	mat2.setStaticFriction(3);
	objecttest->setMaterial(mat2);
	objecttest->m_material->setGreen();

	// add mesh to ODE object
	ODEBody0->setImageModel(object0);
	//ODEBodytest->setImageModel(objecttest);

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

	for (int i = 0; i < numHapticDevices; i++) {
		object2[i] = new cMesh();
		cCreateBox(object2[i], size, 0.01, 0.01);
		object2[i]->setMaterial(mat2);
		tool[i]->addChild(object2[i]);
		object3[i] = new cMesh();
		cCreateBox(object3[i], size, 0.01, 0.01);
		object3[i]->setMaterial(mat2);
		tool[i]->addChild(object3[i]);
		pressed[i] = false;
	}

	for (int i = 0; i < 12; i++) {
		ODEBody1[i] = new cODEGenericBody(ODEWorld);
		object1[i] = new cMesh();
		cCreateRing(object1[i], 0.01, 0.1);
		object1[i]->setMaterial(mat1);
		ODEBody1[i]->setImageModel(object1[i]);
		ODEBody1[i]->createDynamicMesh(true);
		ODEBody1[i]->rotateAboutGlobalAxisRad(1, 0, 1, M_PI);
		ODEBody1[i]->rotateAboutGlobalAxisRad(0, 0, 1, i * 2 * M_PI / 12);
		object1[i]->rotateAboutGlobalAxisRad(0, 0, 1, i * 2 * M_PI / 12);
		ODEBody1[i]->setLocalPos(cos(i * 2 * M_PI / 12) * 0.7, sin(i * 2 * M_PI / 12) * 0.7, -7);
		object1[i]->createAABBCollisionDetector(toolRadius);
		AddDetectionPlane(i, ODEBody1[i]);
		crossing[i] = false;
		end1[i] = false;
		end2[i] = false;

	}
	resetButton = new cMesh();
	world->addChild(resetButton);
	cMaterial mat;
	cCreatePanel(resetButton, .5, .5, .1, 8, cVector3d(0, 0, 0), cMatrix3d(0,1,0,90));
	resetButton->rotateAboutGlobalAxisDeg(cVector3d(1, 0, 0), 90);
	resetButton->translate(cVector3d(-1.5, 0, -6.5));
	mat.setHapticTriangleSides(true, true);
	mat.setDynamicFriction(0.2);
	mat.setStaticFriction(0.2);
	mat.setStiffness(0.3 * maxStiffness);
	mat.setTextureLevel(1);
	mat.setHapticTriangleSides(true, false);
	resetButton->setMaterial(mat);
	resetButton->m_texture = cTexture2d::create();
	bool fileload;
	fileload = resetButton->m_texture->loadFromFile(ROOT_DIR "Resources/Images/resetButton.png");
	if (!fileload)
	{
#if defined(_MSVC)
		fileload = resetButton->m_texture->loadFromFile(ROOT_DIR "Resources/Images/resetButton.png");
#endif
	}
	if (!fileload)
	{
		cout << "Error - Texture image failed to load correctly." << endl;
		close();
		return (-1);
	}
	resetButton->m_texture->setEnvironmentMode(GL_DECAL);
	// enable texture rendering 
	resetButton->setUseTexture(true);

	// Since we don't need to see our polygons from both sides, we enable culling.
	resetButton->setUseCulling(false, true);


	// compute collision detection algorithm
	resetButton->createAABBCollisionDetector(toolRadius);
	resetButton->setEnabled(false);
	//////////////////////////////////////////////////////////////////////////
	// 6 ODE INVISIBLE WALLS
	//////////////////////////////////////////////////////////////////////////

	// we create 6 static walls to contains the 3 cubes within a limited workspace
  //  ODEGPlane0 = new cODEGenericBody(ODEWorld);
	ODEGPlane1 = new cODEGenericBody(ODEWorld);
	ODEGPlane1->createStaticPlane(cVector3d(0.0, 0.0, -7.5), cVector3d(0.0, 0.0, 1.0));
	 //////////////////////////////////////////////////////////////////////////
	 // GROUND
	 //////////////////////////////////////////////////////////////////////////

	 // create a mesh that represents the ground
	ground = new cMesh();
	ODEWorld->addChild(ground);

	// create a plane
	double groundSize = 3.0;
	cCreatePlane(ground, groundSize, groundSize);

	// position ground in world where the invisible ODE plane is located (ODEGPlane1)
	ground->setLocalPos(0.0, 0.0, -7.5);

	// define some material properties and apply to mesh
	cMaterial matGround;
	matGround.setStiffness(maxStiffness);
	matGround.setDynamicFriction(0.2);
	matGround.setStaticFriction(0.2);
	matGround.setGray();
	matGround.m_emission.setGrayLevel(0.3);
	ground->setMaterial(matGround);

	// setup collision detector
	ground->createAABBCollisionDetector(toolRadius);

	//-----------------------------------------------------------------------
	// START SIMULATION
	//-----------------------------------------------------------------------

	// create a thread which starts the main haptics rendering loop
	hapticsThread = new cThread();
	hapticsThread->start(updateHaptics, CTHREAD_PRIORITY_HAPTICS);

	// setup callback when application exits
	atexit(close);


	//--------------------------------------------------------------------------
	// MAIN GRAPHIC LOOP
	//--------------------------------------------------------------------------

	// call window size callback at initialization
	windowSizeCallback(window, width, height);
	// recenter oculus
	if (!camSim) {
		camera->setLocalPos(defaultPos);
		oculusVR.recenterPose();
	}
	else {
		camera->setLocalPos(cVector3d(2.7, 0, -6.6));
	}

	// main graphic loop
	while (!glfwWindowShouldClose(window))
	{
		// get width and height of window
		glfwGetWindowSize(window, &width, &height);

		// render graphics
		updateGraphics();

		// swap buffers
		glfwSwapBuffers(window);

		// process events
		glfwPollEvents();

		// signal frequency counter
		freqCounterGraphics.signal(1);
	}

	// close window
	glfwDestroyWindow(window);

	// terminate GLFW library
	glfwTerminate();

	// exit
	return 0;
}

//---------------------------------------------------------------------------

void windowSizeCallback(GLFWwindow* a_window, int a_width, int a_height)
{
	// update window size
	width = a_width;
	height = a_height;
}

//------------------------------------------------------------------------------

void errorCallback(int a_error, const char* a_description)
{
	cout << "Error: " << a_description << endl;
}

//---------------------------------------------------------------------------

void keyCallback(GLFWwindow* a_window, int a_key, int a_scancode, int a_action, int a_mods)
{
	// filter calls that only include a key press
	if ((a_action != GLFW_PRESS) && (a_action != GLFW_REPEAT))
	{
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
	// option - enable/disable gravity
	else if (a_key == GLFW_KEY_G)
	{
		if (ODEWorld->getGravity().length() > 0.0)
		{
			ODEWorld->setGravity(cVector3d(0.0, 0.0, 0.0));
		}
		else
		{
			ODEWorld->setGravity(cVector3d(0.0, 0.0, -9.81));
		}
	}

	// option - toggle fullscreen
	else if (a_key == GLFW_KEY_F)
	{
		// toggle state variable
		fullscreen = !fullscreen;

		// get handle to monitor
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();

		// get information about monitor
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);

		// set fullscreen or window mode
		if (fullscreen)
		{
			glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
			glfwSwapInterval(swapInterval);
		}
		else
		{
			int w = 0.8 * mode->height;
			int h = 0.5 * mode->height;
			int x = 0.5 * (mode->width - w);
			int y = 0.5 * (mode->height - h);
			glfwSetWindowMonitor(window, NULL, x, y, w, h, mode->refreshRate);
			glfwSwapInterval(swapInterval);
		}
	}

	// option - toggle vertical mirroring
	else if (a_key == GLFW_KEY_M)
	{
		mirroredDisplay = !mirroredDisplay;
		camera->setMirrorVertical(mirroredDisplay);
	}
	// option - toggle vertical mirroring
	else if (a_key == GLFW_KEY_D)
	{
		ODEBody0->translate(cVector3d(0, 0.02, 0));
	}
	else if (a_key == GLFW_KEY_A)
	{
		ODEBody0->translate(cVector3d(0, -0.02, 0));
	}
}

//---------------------------------------------------------------------------

void close(void)
{
	// stop the simulation
	simulationRunning = false;

	// wait for graphics and haptics loops to terminate
	while (!simulationFinished) { cSleepMs(100); }

	// close haptic device
	for (int i = 0; i < numHapticDevices; i++) {
		tool[i]->stop();
	}

	// delete resources
	delete hapticsThread;
	delete world;
	delete handler;
}

//---------------------------------------------------------------------------

void AddDetectionPlane(int i, cODEGenericBody* ring) {
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

void ComputeCrossing(cMesh* spheres[], int i) {
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
void updateGraphics(void)
{
	/////////////////////////////////////////////////////////////////////
	// UPDATE WIDGETS
	/////////////////////////////////////////////////////////////////////

	// update haptic and graphic rate data
	labelRates->setText(cStr(freqCounterGraphics.getFrequency(), 0) + " Hz / " +
		cStr(freqCounterHaptics.getFrequency(), 0) + " Hz");

	// update position of label
	labelRates->setLocalPos((int)(0.5 * (width - labelRates->getWidth())), 15);


	currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	if (start) {
		timerNum += deltaTime;
	}
	/////////////////////////////////////////////////////////////////////
	// RENDER SCENE
	/////////////////////////////////////////////////////////////////////

	// update shadow maps (if any)
	world->updateShadowMaps(false, mirroredDisplay);
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
		camera->renderView(width, height);
	}

	// wait until all GL commands are completed
	glFinish();

	// check for any OpenGL errors
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) printf("Error:  %s\n", gluErrorString(err));
}

//---------------------------------------------------------------------------

void updateHaptics(void)
{
	// simulation in now running
	simulationRunning = true;
	simulationFinished = false;

	// reset clock
	cPrecisionClock clock;
	clock.reset();

	// main haptic simulation loop
	while (simulationRunning)
	{
		/////////////////////////////////////////////////////////////////////
		// SIMULATION TIME
		/////////////////////////////////////////////////////////////////////

		// stop the simulation clock
		clock.stop();
		// read the time increment in seconds
		timeInterval = cClamp(clock.getCurrentTimeSeconds(), 0.0001, 0.001);
		for (int i = 0; i < 12; i++) {
			ComputeCrossing(DetectSphere, i);
		}

		// restart the simulation clock
		clock.reset();
		clock.start();

		// signal frequency counter
		freqCounterHaptics.signal(1);

		// compute global reference frames for each object
		world->computeGlobalPositions(true);
		bool caught[MAX_DEVICES];
		int vientdegrip = -1;
		for (int i = 0; i < numHapticDevices; i++) {
			// get status of user switch
			object2[i]->setLocalRot(tool[i]->getDeviceLocalRot());
			object2[i]->setLocalPos(tool[i]->m_hapticPointFinger->getLocalPosProxy() + object2[i]->getLocalRot() * cVector3d(0.2, 0, 0));
			object3[i]->setLocalRot(tool[i]->getDeviceLocalRot());
			object3[i]->setLocalPos(tool[i]->m_hapticPointThumb->getLocalPosProxy() + object3[i]->getLocalRot() * cVector3d(0.2, 0, 0));
			/////////////////////////////////////////////////////////////////////
			// HAPTIC FORCE COMPUTATION
			/////////////////////////////////////////////////////////////////////



			// update position and orientation of tool[i]
			tool[i]->updateFromDevice();
			tool[i]->setDeviceLocalPos(cClamp(tool[i]->getDeviceLocalPos().x(), -1.5, 1.5), cClamp(tool[i]->getDeviceLocalPos().y(), -1.5, 1.5), cClamp(tool[i]->getDeviceLocalPos().z(), -3.0, -1.5));
			// compute interaction forces
			tool[i]->computeInteractionForces();

			// send forces to haptic device
			tool[i]->applyToDevice();
			caught[i] = true;
			int numInteractionPoints = tool[i]->getNumHapticPoints();
			for (int j = 0; j < numInteractionPoints; j++)
			{
				// get pointer to next interaction point of tool[i]
				cHapticPoint* interactionPoint = tool[i]->getHapticPoint(j);

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
			if (start) {
				bool button = tool[i]->getUserSwitch(0);
				if (tool[i]->isInContact(resetButton) && button == true && !pressed[i]) {
					ResetSim();
					pressed[i] = true;
				}
			}
			else pressed[i] = false;

		}
		if (vientdegrip != -1) {
			gripperCatchingIndex = vientdegrip;
		}
		if (gripperCatchingIndex != -1 && !caught[gripperCatchingIndex]) {
			gripperCatchingIndex = -1;
			for (int i = 0; i < numHapticDevices; i++) {
				if (caught[i]) {
					gripperCatchingIndex = i;
				}
			}
		}
		if (gripperCatchingIndex == -1) {
			ODEWorld->setGravity(cVector3d(0, 0, -45.81));

		}
		for (int i = 0; i < numHapticDevices; i++) {
			/////////////////////////////////////////////////////////////////////
			// DYNAMIC SIMULATION
			/////////////////////////////////////////////////////////////////////
			if (gripperCatchingIndex == i) {
				if (vientdegrip == i) {
					startrotGripper[i].copyfrom(tool[i]->getDeviceGlobalRot());
					startrotCube.copyfrom(ODEBody0->getLocalRot());
					startposGripper[i].copyfrom(tool[i]->getDeviceLocalPos());
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
					ArmT.copyfrom(tool[i]->getDeviceLocalRot());
					ArmT0Invert.copyfrom(startrotGripper[i]);
					ArmT0Invert.invert();
					ObjT0.copyfrom(startrotCube);

					cVector3d rotvec = toAxisAngleVec(ObjT0Invert * ArmT * ArmT0Invert * ObjT0);
					double rotangle = toAxisAngleAngle(ObjT0Invert * ArmT * ArmT0Invert * ObjT0);
					if (isnan(rotangle) || rotangle > 0.174533) {
						rotangle = 0;
					}
					cMatrix3d temp = ODEBody0->getLocalRot();
					bool test = false;
					if (test) {
						ODEBody0->setLocalRot(startrotCube);
						ODEBody0->rotateAboutLocalAxisRad(rotvec, rotangle);
					}
					else {
						ODEBody0->setLocalRot(startrotCube * ObjT0Invert * ArmT * ArmT0Invert * ObjT0);
					}

					bool iltouche = false;
					for (int z = -1; z <= 1; z ++) {
						for (int y = -1; y <= 1; y ++) {
							double size = 0.4;
							double step = 0.1;
							DetectSphere[0]->setLocalPos(DetectSphere[0]->getLocalPos() + cVector3d(0, y * size* step, z * size* step));
							DetectSphere[4]->setLocalPos(DetectSphere[4]->getLocalPos() + cVector3d(0, y * size * step, z * size * step));
							if (ground->computeCollisionDetection(DetectSphere[0]->getGlobalPos(), DetectSphere[4]->getGlobalPos(), recorder, settings)) {
								iltouche = true;
								cout << "il touche ground";
							}
							for (int j = 0; j < 12; j++) {
								object1[j]->setShowCollisionDetector(true, true);
								if (object1[j]->computeCollisionDetection(DetectSphere[0]->getGlobalPos(), DetectSphere[4]->getGlobalPos(), recorder, settings)) {
									iltouche = true;
									cout << "il touche object1["<<j<<"],y="<<y<<",z="<<z<<" ";
									object1[j]->m_material->setYellow();
								}
								else {

									object1[j]->m_material->setBlue();
								}
							}

							DetectSphere[0]->setLocalPos(DetectSphere[0]->getLocalPos() - cVector3d(0, y * size* step, z * size* step));
							DetectSphere[4]->setLocalPos(DetectSphere[4]->getLocalPos() - cVector3d(0, y * size* step, z * size* step));
						}
					}
					if (iltouche) {
						ODEBody0->setLocalRot(temp);
					}

					startrotGripper[i].copyfrom(tool[i]->getDeviceGlobalRot());
					startrotCube.copyfrom(ODEBody0->getLocalRot());
					startposGripper[i].copyfrom(tool[i]->getDeviceLocalPos());
					startposCube.copyfrom(ODEBody0->getLocalPos());
				}

			}
			// for each interaction point of the tool[i] we look for any contact events
			// with the environment and apply forces accordingly
			int numInteractionPoints = tool[i]->getNumHapticPoints();
			for (int j = 0; j < numInteractionPoints; j++)
			{
				// get pointer to next interaction point of tool[i]
				cHapticPoint* interactionPoint = tool[i]->getHapticPoint(j);



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
		if (start && timerNum > lastSave)SaveData();
		// update simulation
		ODEWorld->updateDynamics(timeInterval);
	}

	// exit haptics thread
	simulationFinished = true;
}


cVector3d toAxisAngleVec(cMatrix3d m) {
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

double toAxisAngleAngle(cMatrix3d m) {
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

void InitializeNeedleDetect() {
	double size = 0.4;
	for (int i = 0; i < 5; i++) {
		DetectSphere[i] = new cMesh();
		cCreateSphere(DetectSphere[i], sphereSize);
		ODEBody0->addChild(DetectSphere[i]);
		DetectSphere[i]->translate(size * (-1 + (double)i / (double)2), 0, 0);
	}
}
void ResetSim() {
	for (int k = 0; k < numHapticDevices; k++) {
		std::stringstream temp;
		tempfile[k].close();
		temp << pathname << k << ".csv";
		tempfile[k].open(temp.str());
		temp.str("");
		temp.clear();
	}
}
void Start() {
	resetButton->setEnabled(true);
	start = true;
}
void SaveData() {
	lastSave = timerNum;
	for (int k = 0; k < numHapticDevices; k++) {
		tempfile[k] << std::get<0>(posData[k]) << " , " << std::get<1>(posData[k]) << endl;
	}
}
void SaveResults() {
	std::stringstream temp;
	//DisplayTimer(timerNum);
	start = false;
	for (int k = 0; k < numHapticDevices; k++) {
		tempfile[k].close();
		std::ifstream readfile;
		bool firstline = true;
		temp << ROOT_DIR "Resources/CSV/" << (!NumCandidate.empty() ? NumCandidate + "-" : "") << "Aroundtheclock-trajectory-Arm_" << k << ".csv";
		std::cout << "Saving trajectory into /Resources/CSV/" << (!NumCandidate.empty() ? NumCandidate + "-" : "") << "Aroundtheclock-trajectory-Arm_" << k << ".csv\n";
		myfile[k].open(temp.str());
		temp.str("");
		temp.clear();
		string line;
		temp << pathname << k << ".csv";
		readfile.open(temp.str());
		temp.str("");
		temp.clear();
		myfile[k] << "Temps" << " , " << "Position - x" << " , " << "Position - y" << " , " << "Position - z" << "\n";
		while (getline(readfile, line)) {
			if (firstline) {
				myfile[k] << line << " , " << timerNum << "\n";
				firstline = false;
			}
			else myfile[k] << line << "\n";
		}
		myfile[k].close();
	}
}
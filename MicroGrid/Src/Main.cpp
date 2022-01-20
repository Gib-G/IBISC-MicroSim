#include <chai3d.h>
//------------------------------------------------------------------------------
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
//------------------------------------------------------------------------------
#include <COculus.h>
//------------------------------------------------------------------------------
using namespace chai3d;
using namespace std;
//------------------------------------------------------------------------------
// maximum number of devices supported by this application
const int MAX_DEVICES = 2;
double toolRadius;
//game variables
int numCube = 14;
float errorPixel;
float totalColoredPixels;
float errorPercent = 0;
float greenPixels;
float goalPixels;
float correctPercent = 0;
float cubesize;
int pattern = 0;
int MAX_PATTERN = 1;
cColorb errorColor;
bool start = false;
//Paint variables
const double K_INK = 30;
double K_SIZE = 10;
int BRUSH_SIZE = 600;
float timerNum = 0.0f;
map<float, cVector3d> posData[MAX_DEVICES];
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// CHAI3D
//------------------------------------------------------------------------------

// a world that contains all objects of the virtual environment
cWorld* world;

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
// a mesh object to model a piece of canvas
cMesh* canvas;
cMesh* board;
cMesh* timer1, * timer2, * timer3, * timer4;
cMultiMesh* timer;
cMesh* startButton;
cMesh* changeButton;
cMesh* resetButton;
cMesh* saveButton;
// copy of blank canvas texture
cImagePtr canvasOriginal;

// selected paint color
cColorb paintColor;

// a virtual tool representing the haptic device in the scene
cToolCursor* tool[MAX_DEVICES];

string savePath;
// indicates if the haptic simulation currently running
bool simulationRunning = false;

// indicates if the haptic simulation has terminated
bool simulationFinished = false;

bool camSim = false;
// frequency counter to measure the simulation haptic rate
cFrequencyCounter frequencyCounter;

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

double moveSpeed = 1;
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

void ResetCanvas(int pattern);

void Start(void);

void SaveCanvas(void);

void ResetSim(int pattern);

void ChangePattern(void);

bool PaintCanvas(int x, int y, int pattern);

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

void DisplayTimer(float time) {
	std::stringstream temp;
	temp << (int)time;
	string str = temp.str();
	cout << str << endl;
	bool fileload=false;
	string folder = ROOT_DIR "Resources\\Images\\";
	for (int i = 0; i < str.length(); i++) {
		string stringnum = folder + str[str.length()-1-i] + ".png";
		switch (i) {
		case(0):
			fileload = timer1->m_texture->loadFromFile(stringnum);
			timer1->m_texture->markForUpdate();
			cout << stringnum << endl;
			break;
		case(1):
			fileload = timer2->m_texture->loadFromFile(stringnum);
			timer2->m_texture->markForUpdate();
			cout << stringnum << endl;
			break;
		case(2):
			fileload = timer3->m_texture->loadFromFile(stringnum);
			timer3->m_texture->markForUpdate();
			cout << stringnum << endl;
			break;
		case(3):
			fileload = timer4->m_texture->loadFromFile(stringnum);
			timer4->m_texture->markForUpdate();
			cout << stringnum << endl;
			break;
		}
		if (!fileload)
		{
			cout << "Error - Texture image failed to load correctly : " << stringnum << endl;
		}
	}
	for (int j = str.length(); j < 4; j++) {
		string stringnum = folder + "0.png";
		switch (j) {
		case(0):
			fileload = timer1->m_texture->loadFromFile(stringnum);
			timer1->m_texture->markForUpdate();
			break;
		case(1):
			fileload = timer2->m_texture->loadFromFile(stringnum);
			timer2->m_texture->markForUpdate();
			break;
		case(2):
			fileload = timer3->m_texture->loadFromFile(stringnum);
			timer3->m_texture->markForUpdate();
			break;
		case(3):
			fileload = timer4->m_texture->loadFromFile(stringnum);
			timer4->m_texture->markForUpdate();
			break;
		}
		if (!fileload)
		{
			cout << "Error - Texture image failed to load correctly." << endl;
		}

	}
}

void GetResult() {
	errorColor.setRed();
	errorPixel = 0;
	totalColoredPixels = 0;
	greenPixels = 0;
	for (int k = 0; k < 1024; k++) {
		for (int l = 0; l < 1024; l++) {
			// get color at location
			cColorb getcolor;
			canvas->m_texture->m_image->getPixelColor(k, l, getcolor);
			if (getcolor == errorColor || getcolor == paintColor) {
				if (getcolor == errorColor) {
					errorPixel++;
				}
				if (getcolor == paintColor) {
					greenPixels++;
				}
				totalColoredPixels++;
			}
		}
	}
	bool hit = false;
	errorPercent = (totalColoredPixels ? errorPixel / totalColoredPixels * 100 : 0);
	correctPercent = greenPixels / goalPixels * 100;
	cout.precision(10);
	cout << "Pourcentage d'erreurs : " << errorPercent << "%" << endl;
	cout << "Pourcentage de complétion : " << correctPercent << "%" << endl;
	cout << "greenPixels | goalPixels : " << greenPixels << "|" << goalPixels << endl;
}
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
	cout << "[1] - Lower friction" << endl;
	cout << "[2] - Augment friction" << endl;
	cout << "[3] - Lower tool radius" << endl;
	cout << "[4] - Augment tool radius" << endl;
	cout << "[5] - Lower brush size" << endl;
	cout << "[6] - Augment brush size" << endl;
	cout << "[h] - Print this message" << endl;
	cout << endl << endl;

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
	world = new cWorld();
	// initialize oculus
	// create a camera and insert it into the virtual world
	camera = new cCamera(world);
	world->addChild(camera);
	if (!oculusVR.initVR())
	{
		cout << "failed to initialize Oculus" << endl;
		cSleepMs(1000);
		camSim = true;

		// position and orient the camera
		camera->set(cVector3d(1.65, -0.35, 0.2),    // camera position (eye)
			cVector3d(0, -.6, .15),    // lookat position (target)
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


	//--------------------------------------------------------------------------
	// WORLD - CAMERA - LIGHTING
	//--------------------------------------------------------------------------

	// set the background color of the environment
	// the color is defined by its (R,G,B) components.
	world->m_backgroundColor.setWhite();
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

	// add light to world
	world->addChild(light);

	// enable light source
	light->setEnabled(true);

	// position the light source
	light->setLocalPos(3.5, 2.0, 2.0);

	// define the direction of the light beam
	light->setDir(-3.5, -2.0, -2.0);

	// set light cone half angle
	light->setCutOffAngleDeg(50);


	//--------------------------------------------------------------------------
	// HAPTIC DEVICES / TOOLS
	//--------------------------------------------------------------------------

	// create a haptic device handler
	handler = new cHapticDeviceHandler();
	// define the radius of the tool (sphere)
	toolRadius = 0.01;
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
		tool[i]->setShowContactPoints(true, false);
		// enable if objects in the scene are going to rotate of translate
		// or possibly collide against the tool. If the environment
		// is entirely static, you can set this parameter to "false"
		tool[i]->enableDynamicObjects(true);

		// map the physical workspace of the haptic device to a larger virtual workspace.
		tool[i]->setWorkspaceRadius(1);

		// haptic forces are enabled only if small forces are first sent to the device;
		// this mode avoids the force spike that occurs when the application starts when 
		// the tool is located inside an object for instance. 
		tool[i]->setWaitForSmallForce(true);

		paintColor.setGreen();
		tool[i]->m_hapticPoint->m_sphereProxy->m_material->setColor(paintColor);
		// start the haptic tool
		tool[i]->start();
	}

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
		double workspaceScaleFactor = tool[0]->getWorkspaceScaleFactor();
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
	/*board = new cMesh();
	cCreatePlane(board, 1, 1, cVector3d(-1, 0, -1));
	board->rotateAboutGlobalAxisRad(cVector3d(0, 1, 0), cDegToRad(130));
	board->rotateAboutGlobalAxisRad(cVector3d(1, 0, 0), cDegToRad(90));
	world->addChild(board);*/
	canvas = new cMesh();
	cCreatePlane(canvas, 1, 1, cVector3d(-0.3, 0, -0.45));
	world->addChild(canvas);
	// create collision detector
	canvas->createBruteForceCollisionDetector();
	canvas->setFriction(0.3, 0.3, true);
	canvas->rotateAboutGlobalAxisDeg(cVector3d(1, 0, 0), 50);
	/*
	canvas->rotateAboutGlobalAxisRad(cVector3d(0, 1, 0), cDegToRad(90));
	canvas->rotateAboutGlobalAxisRad(cVector3d(1, 0, 0), cDegToRad(90));*/

	// set graphic properties
	canvas->m_texture = cTexture2d::create();
	fileload = canvas->m_texture->loadFromFile(ROOT_DIR "Resources/Images/grid.jpg"); // Images/grid.jpg
	if (!fileload)
	{
#if defined(_MSVC)
		fileload = canvas->m_texture->loadFromFile(ROOT_DIR "Resources/Images/grid.jpg");
#endif
	}
	if (!fileload)
	{
		cout << "Error - Texture image failed to load correctly." << endl;
		close();
		return (-1);
	}

	// create a copy of canvas so that we can clear page when requested
	canvasOriginal = canvas->m_texture->m_image->copy();

	// we disable lighting properties for canvas
	canvas->setUseMaterial(false);

	// enable texture mapping
	canvas->setUseTexture(true);

	// set haptic properties
	canvas->m_material->setStiffness(0.5 * maxStiffness);
	canvas->m_material->setStaticFriction(0.30);
	canvas->m_material->setDynamicFriction(0.30);
	canvas->m_material->setTextureLevel(1.5);
	ResetCanvas(pattern);


	//----------------------------------------------------------------------
	// Haptic Buttons in Scene
	//----------------------------------------------------------------------
	//----------------------------------------------------------------------
	// TIMER in Scene
	//----------------------------------------------------------------------
	timer = new cMultiMesh();
	world->addChild(timer);
	timer1 = new cMesh();
	timer->addChild(timer1);
	cMatrix3d rot;
	rot.rotateAboutGlobalAxisDeg(cVector3d(0, 0, 1), 90);
	cCreatePlane(timer1, .5, 1, cVector3d(0, 0, 0));
	timer1->rotateAboutGlobalAxisDeg(cVector3d(0, 0, 1), 90);
	timer1->translate(cVector3d(0, .75, 0));
	timer2 = new cMesh();
	timer->addChild(timer2);
	cCreatePlane(timer2, .5, 1, cVector3d(0, 0, 0));
	timer2->rotateAboutGlobalAxisDeg(cVector3d(0, 0, 1), 90);
	timer2->translate(cVector3d(0, .25, 0));
	timer3 = new cMesh();
	timer->addChild(timer3);
	cCreatePlane(timer3, .5, 1, cVector3d(0, 0, 0));
	timer3->rotateAboutGlobalAxisDeg(cVector3d(0, 0, 1), 90);
	timer3->translate(cVector3d(0, -0.25, 0));
	timer4 = new cMesh();
	timer->addChild(timer4);
	cCreatePlane(timer4, .5, 1, cVector3d(0, 0, 0));
	timer4->rotateAboutGlobalAxisDeg(cVector3d(0, 0, 1), 90);
	timer4->translate(cVector3d(0, -0.75, 0));
	timer->rotateAboutGlobalAxisDeg(cVector3d(0, 1, 0), 90);
	timer->translate(cVector3d(-3, 0, 1.5));
	//--------------------------------------------------------------
	//Individual timer textures
	//--------------------------------------------------------------
	timer1->m_texture = cTexture2d::create();
	fileload = timer1->m_texture->loadFromFile(ROOT_DIR "Resources/Images/0.png"); // Images/0.png
	if (!fileload)
	{
#if defined(_MSVC)
		fileload = timer1->m_texture->loadFromFile(ROOT_DIR "Resources/Images/0.png");
#endif
	}
	if (!fileload)
	{
		cout << "Error - Texture image failed to load correctly." << endl;
		close();
		return (-1);
	}
	timer1->m_texture->setEnvironmentMode(GL_DECAL);
	// enable texture rendering 
	timer1->setUseTexture(true);
	timer2->m_texture = cTexture2d::create();
	fileload = timer2->m_texture->loadFromFile(ROOT_DIR "Resources/Images/0.png"); // Images/0.png
	if (!fileload)
	{
#if defined(_MSVC)
		fileload = timer2->m_texture->loadFromFile(ROOT_DIR "Resources/Images/0.png");
#endif
	}
	if (!fileload)
	{
		cout << "Error - Texture image failed to load correctly." << endl;
		close();
		return (-1);
	}
	timer2->m_texture->setEnvironmentMode(GL_DECAL);
	// enable texture rendering 
	timer2->setUseTexture(true);
	timer3->m_texture = cTexture2d::create();
	fileload = timer3->m_texture->loadFromFile(ROOT_DIR "Resources/Images/0.png");
	if (!fileload)
	{
#if defined(_MSVC)
		fileload = timer3->m_texture->loadFromFile(ROOT_DIR "Resources/Images/0.png");
#endif
	}
	if (!fileload)
	{
		cout << "Error - Texture image failed to load correctly." << endl;
		close();
		return (-1);
	}
	timer3->m_texture->setEnvironmentMode(GL_DECAL);
	// enable texture rendering 
	timer3->setUseTexture(true);
	timer4->m_texture = cTexture2d::create();
	fileload = timer4->m_texture->loadFromFile(ROOT_DIR "Resources/Images/0.png");
	if (!fileload)
	{
#if defined(_MSVC)
		fileload = timer4->m_texture->loadFromFile(ROOT_DIR "Resources/Images/0.png");
#endif
	}
	if (!fileload)
	{
		cout << "Error - Texture image failed to load correctly." << endl;
		close();
		return (-1);
	}
	timer4->m_texture->setEnvironmentMode(GL_DECAL);
	// enable texture rendering 
	timer4->setUseTexture(true);
	//----------------------------------------------------------------------
	//----------------------------------------------------------------------
	resetButton = new cMesh();
	world->addChild(resetButton);
	cMaterial mat;
	rot.identity();
	rot.rotateAboutGlobalAxisDeg(cVector3d(0, 0, 1), 90);
	cCreatePanel(resetButton, .5, .5, .1, 8, cVector3d(0, 0, 0), rot);
	resetButton->translate(cVector3d(-.4, 0.5, -0.45));
	/*
	cFontPtr font = NEW_CFONTCALIBRI20();
	cLabel* label = new cLabel(font);
	label->m_fontColor.setBlack();
	label->setText("Reset");
	resetButton->addChild(label);*/

	// compute collision detection algorithm
	mat.setHapticTriangleSides(true, true);
	mat.setDynamicFriction(0.2);
	mat.setStaticFriction(0.2);
	mat.setStiffness(0.3 * maxStiffness);
	mat.setTextureLevel(1);
	mat.setHapticTriangleSides(true, false);
	resetButton->setMaterial(mat);
	resetButton->m_texture = cTexture2d::create();
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


	saveButton = new cMesh();
	world->addChild(saveButton);
	rot.identity();
	rot.rotateAboutGlobalAxisDeg(cVector3d(0, 0, 1), 90);
	cCreatePanel(saveButton, .5, .5, .1, 8, cVector3d(0, 0, 0), rot);
	saveButton->translate(cVector3d(-.4, -1.05, -0.45));
	saveButton->setMaterial(mat);
	// set graphic properties
	saveButton->m_texture = cTexture2d::create();
	fileload = saveButton->m_texture->loadFromFile(ROOT_DIR "Resources/Images/saveButton.png");
	if (!fileload)
	{
#if defined(_MSVC)
		fileload = saveButton->m_texture->loadFromFile(ROOT_DIR "Resources/Images/saveButton.png");
#endif
	}
	if (!fileload)
	{
		cout << "Error - Texture image failed to load correctly." << endl;
		close();
		return (-1);
	}

	saveButton->m_texture->setEnvironmentMode(GL_DECAL);
	// enable texture rendering 
	saveButton->setUseTexture(true);

	// Since we don't need to see our polygons from both sides, we enable culling.
	saveButton->setUseCulling(false, true);


	// compute collision detection algorithm
	saveButton->createAABBCollisionDetector(toolRadius);


	startButton = new cMesh();
	world->addChild(startButton);
	cCreatePanel(startButton, .5, .5, .1, 8, cVector3d(0, 0, 0), rot);
	startButton->translate(cVector3d(.2, -.525, -0.45));
	startButton->setMaterial(mat);
	startButton->m_texture = cTexture2d::create();
	fileload = startButton->m_texture->loadFromFile(ROOT_DIR "Resources/Images/startButton.png");
	if (!fileload)
	{
#if defined(_MSVC)
		fileload = startButton->m_texture->loadFromFile(ROOT_DIR "Resources/Images/startButton.png");
#endif
	}
	if (!fileload)
	{
		cout << "Error - Texture image failed to load correctly." << endl;
		close();
		return (-1);
	}

	startButton->m_texture->setEnvironmentMode(GL_DECAL);
	// enable texture rendering 
	startButton->setUseTexture(true);

	// Since we don't need to see our polygons from both sides, we enable culling.
	startButton->setUseCulling(false, true);


	// compute collision detection algorithm
	startButton->createAABBCollisionDetector(toolRadius);

	changeButton = new cMesh();
	world->addChild(changeButton);
	cCreatePanel(changeButton, .5, .5, .1, 8, cVector3d(0, 0, 0), rot);
	changeButton->translate(cVector3d(.2, -.005, -0.45));
	changeButton->setMaterial(mat);
	changeButton->m_texture = cTexture2d::create();
	fileload = changeButton->m_texture->loadFromFile(ROOT_DIR "Resources/Images/1.png");
	if (!fileload)
	{
#if defined(_MSVC)
		fileload = changeButton->m_texture->loadFromFile(ROOT_DIR "Resources/Images/1.png");
#endif
	}
	if (!fileload)
	{
		cout << "Error - Texture image failed to load correctly." << endl;
		close();
		return (-1);
	}

	changeButton->m_texture->setEnvironmentMode(GL_DECAL);
	// enable texture rendering 
	changeButton->setUseTexture(true);

	// Since we don't need to see our polygons from both sides, we enable culling.
	changeButton->setUseCulling(false, true);


	// compute collision detection algorithm
	changeButton->createAABBCollisionDetector(toolRadius);

	changeButton->translate(cVector3d(0.1, 0.3, -0.8));
	startButton->translate(cVector3d(0.1, 0.3, -0.8));
	saveButton->translate(cVector3d(0.1, 0.3, -0.8));
	resetButton->translate(cVector3d(0.1, 0.3, -0.8));
	canvas->rotateAboutGlobalAxisDeg(cVector3d(0, 0, 1), 90);
	canvas->translate(cVector3d(-0.2, 0.31, -0.8));
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

	// main graphic rendering loop
	while (!glfwWindowShouldClose(window) && !simulationFinished)
	{
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		//deltaTimeCalc

		globe->rotateAboutGlobalAxisDeg(cVector3d(0, 0, 1), 0.001);
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		if (start) {
			timerNum += deltaTime;
		}
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


	// option - help
	else if ((a_key == GLFW_KEY_H))
	{
		cout << endl;
		cout << "-----------------------------------" << endl;
		cout << "CHAI3D" << endl;
		cout << "Demo: 01-cube" << endl;
		cout << "Copyright 2003-2016" << endl;
		cout << "-----------------------------------" << endl << endl << endl;
		cout << "Keyboard Options:" << endl << endl;
		cout << "[ ] - Recenter view point" << endl;
		cout << "[q] - Exit application" << endl;
		cout << "[1] - Lower friction" << endl;
		cout << "[2] - Augment friction" << endl;
		cout << "[3] - Lower tool radius" << endl;
		cout << "[4] - Augment tool radius" << endl;
		cout << "[5] - Lower brush size" << endl;
		cout << "[6] - Augment brush size" << endl;
		cout << "[h] - Print this message" << endl;
		cout << endl << endl;
	}
	// option - spacebar
	else if (a_key == GLFW_KEY_SPACE)
	{
		oculusVR.recenterPose();
	}
	else if (a_key == GLFW_KEY_R)
	{
		// copy original image of canvas to texture
		canvasOriginal->copyTo(canvas->m_texture->m_image);

		// update texture
		canvas->m_texture->markForUpdate();

		// update console message
		cout << "> Canvas has been erased.            \r";
	}
	if (a_key == GLFW_KEY_F) {
		GetResult();
	}

	if (a_key == GLFW_KEY_4) {
		BRUSH_SIZE += 10;
		numHapticDevices = handler->getNumDevices();
		if (toolRadius < 0.3) toolRadius += 0.01;
		for (int i = 0; i < numHapticDevices; i++) {
			tool[i]->setRadius(toolRadius);
			canvas->createAABBCollisionDetector(toolRadius);
		}
		cout << "Brush size : " << K_SIZE << " | " << "tool Radius : " << toolRadius << endl;
	}
	if (a_key == GLFW_KEY_3) {
		numHapticDevices = handler->getNumDevices();
		if (toolRadius > 0.01) toolRadius -= 0.01;
		for (int i = 0; i < numHapticDevices; i++) {
			tool[i]->setRadius(toolRadius);
			canvas->createAABBCollisionDetector(toolRadius);
		}
		cout << "Brush size : " << K_SIZE << " | " << "tool Radius : " << toolRadius << endl;
	}

	if (a_key == GLFW_KEY_5) {
		if (K_SIZE > 0)		K_SIZE--;
		cout << "Brush size : " << K_SIZE << " | " << "tool Radius : " << toolRadius << endl;
	}
	if (a_key == GLFW_KEY_6) {
		if (K_SIZE < BRUSH_SIZE)		K_SIZE++;
		cout << "Brush size : " << K_SIZE << " | " << "tool Radius : " << toolRadius << endl;
	}
	if (a_key == GLFW_KEY_2) {
		canvas->setFriction(cClamp(canvas->m_material->getStaticFriction() + 0.01, 0.0, 1.5), cClamp(canvas->m_material->getStaticFriction() + 0.01, 0.0, 1.5));
		cout << "Friction : " << canvas->m_material->getStaticFriction() << endl;

	}
	if (a_key == GLFW_KEY_1) {
		canvas->setFriction(cClamp(canvas->m_material->getStaticFriction() - 0.01, 0.0, 1.5), cClamp(canvas->m_material->getStaticFriction() - 0.01, 0.0, 1.5));
		cout << "Friction : " << canvas->m_material->getStaticFriction() << endl;

	}
	if (a_key == GLFW_KEY_7) {
		pattern = (pattern != MAX_PATTERN ? pattern + 1 : 0);
		ResetCanvas(pattern);
		cout << "pattern : " << pattern << endl;
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
		double azimuthDeg = camera->getSphericalAzimuthDeg() - 0.5 * dx / 3;
		double polarDeg = camera->getSphericalPolarDeg() - 0.5 * dy / 3;

		// assign new angles
		camera->setSphericalAzimuthDeg(azimuthDeg);
		camera->setSphericalPolarDeg(polarDeg);

		camera->setLocalPos(cameraPos);

	}
}

bool CompareVectors(cVector3d v1, cVector3d v2) {
	if (v1.x() != v2.x() || v1.y() != v2.y() || v1.z() != v2.z()) return false;
	else return true;
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
	cVector3d InitialPos[MAX_DEVICES];
	bool touching[MAX_DEVICES];
	bool pressed[MAX_DEVICES];

	for (int i = 0; i < numHapticDevices; i++)
	{
		state[i] = IDLE;
		selectedObject[i] = NULL;
		InitialPos[i] = tool[i]->getDeviceGlobalPos();
		pressed[i] = false;
		touching[i] = false;
	}
	cTransform tool_T_object[MAX_DEVICES];
	cTransform tool_T_world[MAX_DEVICES];
	cTransform world_T_tool[MAX_DEVICES];
	cTransform parent_T_world[MAX_DEVICES];
	cTransform parent_T_object[MAX_DEVICES];
	double force[MAX_DEVICES];
	double size[MAX_DEVICES];
	double distance[MAX_DEVICES];
	cPrecisionClock clock;
	clock.reset();

	// simulation in now running
	simulationRunning = true;
	simulationFinished = false;
	timerNum = 0;
	// main haptic simulation loop
	while (simulationRunning)
	{
		/////////////////////////////////////////////////////////////////////
		// SIMULATION TIME
		/////////////////////////////////////////////////////////////////////

		// stop the simulation clock
		clock.stop();

		// read the time increment in seconds
		double timeInterval = clock.getCurrentTimeSeconds();

		// restart the simulation clock
		clock.reset();
		clock.start();
		errorColor.setRed();
		// update frequency counter
		frequencyCounter.signal(1);
		for (int i = 0; i < numHapticDevices; i++)
		{
			/////////////////////////////////////////////////////////////////////
			// HAPTIC FORCE COMPUTATION
			/////////////////////////////////////////////////////////////////////

			// compute global reference frames for each object
			world->computeGlobalPositions(true);

			// update position and orientation of tool
			tool[i]->updateFromDevice();

			// compute interaction forces
			tool[i]->computeInteractionForces();

			// get interaction forces magnitude
			//force[i] = 5 * tool[i]->getDeviceGlobalForce().length();
			// send forces to haptic device
			tool[i]->applyToDevice();

			// get status of user switch
			bool button = tool[i]->getUserSwitch(0);
			/////////////////////////////////////////////////////////////////////
		// INTERACTION WITH CANVAS
		/////////////////////////////////////////////////////////////////////
			if (tool[i]->isInContact(canvas))
			{
				cCollisionEvent* contact = tool[i]->m_hapticPoint->getCollisionEvent(0);
				if (contact != NULL)
				{
					// retrieve contact information
					cVector3d localPos = contact->m_localPos;
					unsigned int triangleIndex = contact->m_index;
					cTriangleArrayPtr triangles = contact->m_triangles;

					// retrieve texture coordinate
					cVector3d texCoord = triangles->getTexCoordAtPosition(triangleIndex, localPos);
					// retrieve pixel information
					int px, py;
					canvas->m_texture->m_image->getPixelLocation(texCoord, px, py);
					cout << "px : " << px << "py : " << py << endl;
					size[i] = cClamp((K_SIZE), 1.0, (double)(BRUSH_SIZE));
					for (int x = -BRUSH_SIZE; x < BRUSH_SIZE; x++)
					{
						for (int y = -BRUSH_SIZE; y < BRUSH_SIZE; y++)
						{
							// compute new color percentage
							distance[i] = sqrt((double)2 * (x * x + y * y));
							if (distance[i] <= size[i])
							{
								// get color at location
								cColorb color, newColor;
								canvas->m_texture->m_image->getPixelColor(px + x, py + y, color);
								if (color != paintColor || color != errorColor) {
									bool hit;
									hit = PaintCanvas(px + x, py + y, pattern);
									if (hit) {
										newColor = paintColor;
									}
									else {
										newColor = errorColor;
									}
									// assign new color to pixel
									int pixelX = px + x;
									int pixelY = py + y;
									if ((pixelX >= 0) && (pixelY >= 0) && (pixelX < canvas->m_texture->m_image->getWidth()) && (pixelY < canvas->m_texture->m_image->getHeight()))
									{
										canvas->m_texture->m_image->setPixelColor(pixelX, pixelY, newColor);
									}
								}
							}
						}
					}
					// update texture
					canvas->m_texture->markForUpdate();
				}
			}
			////////////////////////////////////////////////////////////////////
			//INTERACTIONS WITH BUTTONS
			////////////////////////////////////////////////////////////////////
			if (tool[i]->getHapticPoint(0)->getNumCollisionEvents()>0) {
				touching[i] = true;
			}
			else {
				touching[i] = false;
				pressed[i] = false;
			}
			if (tool[i]->isInContact(resetButton) && button == true && !pressed[i]) {
				ResetSim(pattern);
				pressed[i] = true;
			}
			else if (tool[i]->isInContact(saveButton) && button == true && !pressed[i]) {
				SaveCanvas();
				pressed[i] = true;
			}
			else if(tool[i]->isInContact(startButton) && button == true && !pressed[i]) {
				Start();
				pressed[i] = true;
			}
			else if (tool[i]->isInContact(changeButton) && button == true && !pressed[i]) {
				ChangePattern();
				pressed[i] = true;
			}
			if(start) posData[i].insert(pair<float, cVector3d>(timerNum, tool[i]->getDeviceGlobalPos()));
		}
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
void SaveCanvas() {
	GetResult();
	canvas->m_texture->m_image->saveToFile(ROOT_DIR "Resources/Images/myPicture.jpg");
	string path = ROOT_DIR "Resources/CSV/trajectory-Arm_";
	DisplayTimer(timerNum);
	for (int k = 0; k < numHapticDevices; k++) {
		std::stringstream temp;
		temp << path << k << ".csv";
		std::ofstream myfile;
		std::cout << "Saving trajectory into /Resources/CSV/trajectory-Arm_"<<k<<".csv\n";
		myfile.open(temp.str());
		myfile << "Temps" << " , " << "Position - x" << " , " << "Position - y" << " , " << "Position - z" << " , " << "Temps total" << " , " << "Pourcentage d'erreur" << " , " << "Pourcentage de completion" << "\n";
		std::map<float, cVector3d>::iterator it = posData[k].begin();
		myfile << it->first << " , " << it->second << " , " << timerNum << " , " << errorPercent << " , " << correctPercent << "\n";
		it++;
		for (it; it != posData[k].end(); ++it)
			myfile << it->first << ',' << it->second << '\n';
		myfile.close();
	}
}
void ResetSim(int pattern) {
	if (start) {
		startButton->setEnabled(true);
		changeButton->setEnabled(true);
		start = false;
		timerNum = 0;
		DisplayTimer(timerNum);
	}
	ResetCanvas(pattern);
}

void ChangePattern() {
	pattern = (pattern != MAX_PATTERN ? pattern + 1 : 0);
	ResetCanvas(pattern);
	std::stringstream temp;
	bool fileload = false;
	string folder = ROOT_DIR "Resources\\Images\\";
	temp << folder << pattern + 1 << ".png";
	fileload = changeButton->m_texture->loadFromFile(temp.str());
	changeButton->m_texture->markForUpdate();
}
void ResetCanvas(int pattern) {
	for (int k = 0; k < numHapticDevices; k++) {
		posData[k].clear();
	}
	canvasOriginal->copyTo(canvas->m_texture->m_image);
	cubesize = 1024.0f / numCube;
	cout << pattern << endl;
	switch (pattern) {
	case 1:
		for (int j = 0; j < numCube; j++) {
			for (int x = 1023 + (j == 0 ? 0 : 1) - (j * cubesize); x > 1024 - (j + 1) * cubesize; x--) {
				for (int y = 1023 + (j == 0 ? 0 : 1) - (j * cubesize); y > 1024 - ((j + 1) * cubesize); y--) {
					cColorb getcolor;
					canvas->m_texture->m_image->getPixelColor(x, y, getcolor);
					if (getcolor != paintColor) {
						cColorb yellow;
						yellow.setYellow();
						canvas->m_texture->m_image->setPixelColor(x, y, yellow);
					}
					goalPixels++;
				}
			}
		}
		canvas->m_texture->markForUpdate();
		break;
	default:
		for (int j = 0; j < numCube; j++) {
			for (int x = j * cubesize + 1; x < (j + 1) * cubesize; x++) {
				for (int y = 1023 + (j == 0 ? 0 : 1) - (j * cubesize); y > 1024 - ((j + 1) * cubesize); y--) {
					cColorb getcolor;
					canvas->m_texture->m_image->getPixelColor(x, y, getcolor);
					if (getcolor != paintColor) {
						cColorb yellow;
						yellow.setYellow();
						canvas->m_texture->m_image->setPixelColor(x, y, yellow);
					}
					goalPixels++;
				}
			}
		}
		canvas->m_texture->markForUpdate();
		break;
	}
}

void Start() {
	timerNum = 0;
	start = true;
	ResetCanvas(pattern);
	startButton->setEnabled(false);
	changeButton->setEnabled(false);
}
bool PaintCanvas(int x, int y, int pattern) {
	bool hit = false;
	switch (pattern) {
	case 1:
		for (int j = 0; j < numCube; j++) {
			if (x >= 1024 - cubesize * (j + 1) && x <= 1024 - cubesize * j && y >= 1024 - cubesize * (j + 1) && y <= 1024 - cubesize * j) {
				hit = true;
				break;
			}
		}
		break;
	default:
		for (int j = 0; j < numCube; j++) {
			if (x <= cubesize * (j + 1) && x >= cubesize * j && y >= 1024 - cubesize * (j + 1) && y <= 1024 - cubesize * j) {
				hit = true;
				break;
			}
		}
		break;
	}
	return hit;
}

//------------------------------------------------------------------------------